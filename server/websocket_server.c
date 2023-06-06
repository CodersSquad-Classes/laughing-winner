//websocket.c
#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include "game_state.h"
#include "game.h"
#include <pthread.h>
#include <stdlib.h>
#include "shared.h"

#define MAX_PAYLOAD_SIZE 4096

struct lws *client_wsi;
#define MAX_X 10
#define MAX_Y 10
struct Payload {
    unsigned char data[LWS_SEND_BUFFER_PRE_PADDING + MAX_PAYLOAD_SIZE + LWS_SEND_BUFFER_POST_PADDING];
    size_t len;
} payload;

void prepare_and_send_data(struct lws *wsi) {
    // Convert game state to JSON string
    char json[1024]; // Adjust size as needed. Made it larger to accommodate invader positions.
    char invaderPositionsJson[512] = ""; // A buffer to hold the JSON array of invader positions. Adjust size as needed.

    // Convert invader positions to JSON array
    for (int i = 0; i < game.numInvaders; i++) {
        char pos[256]; // Buffer for one position. Adjust size as needed.
        sprintf(pos, "{\"x\": %d, \"y\": %d,\"id\": %d}",
                game.invaderPositions[i].x, game.invaderPositions[i].y, game.invaderPositions[i].id);
        strcat(invaderPositionsJson, pos);
        if (i < game.numInvaders - 1) {
            strcat(invaderPositionsJson, ", "); // Add comma between elements, but not after last element
        }
    }

    sprintf(json, "{\"numInvaders\": %d, \"invaderPositions\": [%s], \"shooterPosition\": %d, \"score\": %d, \"lives\": %d, \"invaderAboutToShoot\": %d}",
            game.numInvaders, invaderPositionsJson, game.shooterPosition, game.score, game.lives, game.invaderAboutToShoot);

    // Send game state
    memcpy(&payload.data[LWS_SEND_BUFFER_PRE_PADDING], json, strlen(json));
    payload.len = strlen(json);
    lws_write(wsi, &payload.data[LWS_SEND_BUFFER_PRE_PADDING], payload.len, LWS_WRITE_TEXT);
}

void sendDataToClient() {
    if (client_wsi != NULL) {
        prepare_and_send_data(client_wsi);
    }
}

void set_client(struct lws *wsi) {
    pthread_mutex_lock(&lock_game);
    client_wsi = wsi;
    pthread_mutex_unlock(&lock_game);
}

void remove_client(struct lws *wsi) {
    pthread_mutex_lock(&lock_game);
    if (client_wsi == wsi) {
        client_wsi = NULL;
    }
    pthread_mutex_unlock(&lock_game);
}

int callback_echo(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED: {
            set_client(wsi);
            break;
        }
        case LWS_CALLBACK_CLOSED: {
            remove_client(wsi);
            break;
        }
        case LWS_CALLBACK_RECEIVE: {
            char* message = (char*) in;
            enQueue(commandQueue, message);
            break;
        }
        case LWS_CALLBACK_SERVER_WRITEABLE: {
            prepare_and_send_data(wsi);
            break;
        }

        default:
            //printf("callback_echo: unexpected reason: %d\n", reason);
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
