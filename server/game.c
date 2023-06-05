//game.c
#include <pthread.h>
#include <stdlib.h>
#include "game_state.h"
#include <stdio.h>
#include <unistd.h>
#include "game.h"
#include "websocket_server.h"
#include <unistd.h>
#include <string.h>
#include "queueStruct.h"
#include "shared.h" // Include the header file

struct GameState game;
Queue* commandQueue; // Define the variable

// Mutex for synchronization
pthread_mutex_t lock;


void* websocket_server_thread(void* arg) {
    printf("empezando el server\n");
    start_websocket_server();
    return NULL;
}


void* process_commands(void* arg) {
    Queue* queue = (Queue*) arg;
    while (1) {
        char* command = deQueue(queue);
        if (command != NULL) {
            // Process the command
            pthread_mutex_lock(&lock);
            printf("comando: %s\n", command);
            if (strcmp(command, "start") == 0) {
                // Initialize game state
            } else if (strncmp(command, "left", 4) == 0) {
                // Move shooter left
                if (game.shooterPosition > 0) {  // Ensure the shooter doesn't go off the left edge
                    game.shooterPosition--;
                }
            } else if (strncmp(command, "right", 5) == 0) {
                // Move shooter right
                if (game.shooterPosition < 9) {  // Assume game width is 10; adjust as necessary
                    game.shooterPosition++;
                }
            } else if (strncmp(command, "shoot",5) == 0) {
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
            sendDataToClient();
            pthread_mutex_unlock(&lock);

            // Free the command string after using it
            free(command);
        }

        // Sleep a little to prevent busy-waiting
        usleep(1000);
    }

    return NULL;
}



void* invader_behavior(void* arg) {
    while (game.lives > 0) {
        pthread_mutex_lock(&lock);

        for (int i = 0; i < game.numInvaders; i++) {
            if (game.invaderPositions[i].y < 9) {
                game.invaderPositions[i].y++;
            }
        }

        printf("\n Mandare data\n");
        sendDataToClient();
        pthread_mutex_unlock(&lock);

        sleep(1); // Pause for a while before next update
    }

    return NULL;
}





int main() {
    // Initialize game state
    int i;
    game.numInvaders = 10;
    game.invaderPositions = malloc(game.numInvaders * sizeof(InvaderPosition));

    game.shooterPosition = 5;
    game.score = 0;
    game.lives = 10;

    // Initialize invaders positions
    for (int i = 0; i < game.numInvaders; i++) {
        game.invaderPositions[i].x = i;
        game.invaderPositions[i].y = 0;  // Assuming 0 is the initial y-position for all invaders
    }

    pthread_t *threads = malloc((game.numInvaders + 2) * sizeof(pthread_t)); // For invader threads, WebSocket thread and command thread

    // Initialize the mutex
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n Mutex init failed\n");
        return 1;
    }

    commandQueue = createQueue();

    // Create WebSocket server thread
    pthread_create(&threads[0], NULL, websocket_server_thread, NULL);

    // Create command processing thread
    pthread_create(&threads[1], NULL, process_commands, commandQueue);

    // Create threads for invaders
    /*for (int i = 0; i < game.numInvaders; i++) {
        int *arg = malloc(sizeof(*arg));
        *arg = i;
        pthread_create(&threads[i + 2], NULL, invader_behavior, arg); // Offset by 2 for WebSocket and command threads
    }*/
    //pthread_create(&threads[2], NULL, invader_behavior, NULL);


    // Wait for all threads to finish
    for (int i = 0; i < game.numInvaders + 2; i++) { // Offset by 2 for WebSocket and command threads
        pthread_join(threads[i], NULL);
    }

    // Cleanup
    pthread_mutex_destroy(&lock);
    free(game.invaderPositions);
    free(threads);

    return 0;
}
