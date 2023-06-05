//websocket.c
#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include "game_state.h"
#include "game.h"
#include <pthread.h>
#include <stdlib.h>
#include "shared.h" // Include the header file

#define MAX_PAYLOAD_SIZE 4096

struct lws *client_wsi;

struct Payload {
    unsigned char data[LWS_SEND_BUFFER_PRE_PADDING + MAX_PAYLOAD_SIZE + LWS_SEND_BUFFER_POST_PADDING];
    size_t len;
} payload;

void set_client(struct lws *wsi) {
    pthread_mutex_lock(&lock);
    client_wsi = wsi;
    pthread_mutex_unlock(&lock);
}

void remove_client(struct lws *wsi) {
    pthread_mutex_lock(&lock);
    if (client_wsi == wsi) {
        client_wsi = NULL;
    }
    pthread_mutex_unlock(&lock);
}

void prepare_and_send_data(struct lws *wsi) {
    printf("Sending data\n");
    // Convert game state to JSON string
    char json[512]; // Adjust size as needed. Made it larger to accommodate invader positions.
    char invaderPositionsJson[256] = ""; // A buffer to hold the JSON array of invader positions. Adjust size as needed.

    // Convert invader positions to JSON array
    for (int i = 0; i < game.numInvaders; i++) {
        char pos[64]; // Buffer for one position. Adjust size as needed.
        sprintf(pos, "{\"x\": %d, \"y\": %d}", game.invaderPositions[i].x, game.invaderPositions[i].y);
        strcat(invaderPositionsJson, pos);
        if (i < game.numInvaders - 1) {
            strcat(invaderPositionsJson, ", "); // Add comma between elements, but not after last element
        }
    }

    sprintf(json, "{\"numInvaders\": %d, \"invaderPositions\": [%s], \"shooterPosition\": %d, \"score\": %d, \"lives\": %d}", game.numInvaders, invaderPositionsJson, game.shooterPosition, game.score, game.lives);

    // Send game state
    memcpy(&payload.data[LWS_SEND_BUFFER_PRE_PADDING], json, strlen(json));
    payload.len = strlen(json);
    lws_write(wsi, &payload.data[LWS_SEND_BUFFER_PRE_PADDING], payload.len, LWS_WRITE_TEXT);
}

void sendDataToClient() {
    printf("boa a mandar\n");
    if (client_wsi != NULL) {
        prepare_and_send_data(client_wsi);
    }
}

int callback_echo(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED: {
            printf("New client\n");
            set_client(wsi);
            break;
        }
        case LWS_CALLBACK_CLOSED: {
            printf("Client connection terminated\n");
            remove_client(wsi);
            break;
        }
        case LWS_CALLBACK_RECEIVE: {
            char* message = (char*) in;
            printf("Received data\n %s\n",message);

            enQueue(commandQueue, message);
            break;
        }
        case LWS_CALLBACK_SERVER_WRITEABLE: {
            prepare_and_send_data(wsi);
            break;
        }

        default:
            printf("callback_echo: unexpected reason: %d\n", reason);
            break;
    }

    return 0;
}

static struct lws_protocols protocols[] = {
        {
                "echo-protocol",
                callback_echo,
                      0,
                MAX_PAYLOAD_SIZE,
        },
        { NULL, NULL, 0, 0 } // terminator
};

void start_websocket_server() {
    struct lws_context_creation_info info = {
            .port = 9000,
            .protocols = protocols,
            .gid = -1,
            .uid = -1,
    };

    struct lws_context *context = lws_create_context(&info);
    if (!context) {
        fprintf(stderr, "Failed to create context\n");
    }

    while (1) {
        lws_service(context, 50);
    }

    lws_context_destroy(context);

}
