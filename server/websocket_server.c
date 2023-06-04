#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include "game_state.h"
#include "game.h"
#include <pthread.h>
#include <stdlib.h>

#define MAX_PAYLOAD_SIZE 4096

struct Payload {
    unsigned char data[LWS_SEND_BUFFER_PRE_PADDING + MAX_PAYLOAD_SIZE + LWS_SEND_BUFFER_POST_PADDING];
    size_t len;
} payload;

int callback_echo(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_RECEIVE: {
            char* message = (char*) in;
            char command[5];
            strncpy(command, message, 4);
            command[4] = '\0'; // Null-terminate the string
            pthread_mutex_lock(&lock);

            if (strcmp(message, "start") == 0) {
                // Initialize game state
            } else if (strcmp(command, "left") == 0) {
                // Move shooter left
                if (game.shooterPosition > 0) {  // Ensure the shooter doesn't go off the left edge
                    game.shooterPosition--;
                }
            } else if  (strcmp(command, "righ") == 0) {
                // Move shooter right
                if (game.shooterPosition < 9) {  // Assume game width is 10; adjust as necessary
                    game.shooterPosition++;
                }
            } else if (strcmp(command, "shoo") == 0) {
        printf("Shoot\n");
        // Shoot
        for (int i = 0; i < game.numInvaders; i++) {
            // Check if the shot hits any invader
            if (game.invaderPositions[i].x == game.shooterPosition) {
                // If hit, remove this invader by shifting the remaining invaders and reducing the count
                for (int j = i + 1; j < game.numInvaders; j++) {
                    game.invaderPositions[j - 1] = game.invaderPositions[j];
                }
                game.numInvaders--;
                break;
            }
        }
    }

            pthread_mutex_unlock(&lock);

            // Schedule write callback to send updated game state
            lws_callback_on_writable(wsi);
        }
        case LWS_CALLBACK_SERVER_WRITEABLE: {
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

            break;
        }

        default:
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
