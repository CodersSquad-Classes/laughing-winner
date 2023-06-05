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
#include <time.h> // For random number generation

#define MAX_X 10 // adjust as per your game settings
#define MAX_Y 10 // adjust as per your game settings

struct GameState game;
Queue* commandQueue; // Define the variable

// Mutex for synchronization
pthread_mutex_t lock;


void* websocket_server_thread(void* arg) {
    printf("empezando el server\n");
    start_websocket_server();
    return NULL;
}

void playerShot() {
    int nearestInvaderIndex = -1;
    int max_y = -1;

    // Find the nearest invader in the same x lane as the shooter
    for (int i = 0; i < game.numInvaders; i++) {
        if (game.invaderPositions[i].x == game.shooterPosition && game.invaderPositions[i].y > max_y) {
            max_y = game.invaderPositions[i].y;
            nearestInvaderIndex = i;
        }
    }

    // If an invader was found, remove it
    if (nearestInvaderIndex != -1) {
        for (int j = nearestInvaderIndex; j < game.numInvaders - 1; j++) {
            game.invaderPositions[j] = game.invaderPositions[j + 1];
        }

        game.numInvaders--;
        game.score += 100;
    }
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
                playerShot();
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

typedef struct {
    int index;
} invader_args;

void* invader_behavior(void* arg) {
    srand(time(NULL)); // Seed the random number generator
    sleep(4); // Pause for a while before next update
    // Each invader needs to know its index
    int invaderIndex = *((int*)arg);
    free(arg); // We're done with the argument now

    // Logic for individual invader
    while (game.lives > 0 && game.invaderPositions[invaderIndex].y < MAX_Y) { // Also stop if invader reaches the end
        pthread_mutex_lock(&lock);

        // Decide randomly if this invader is going to chase the player or move randomly
        int chasePlayer = rand() % 2; // This will be 0 or 1

        // Calculate new X position
        int newX = game.invaderPositions[invaderIndex].x;
        if (chasePlayer && game.lives > 0) { // This invader is going to chase the player
            if (newX < game.shooterPosition) {
                newX++;
            } else if (newX > game.shooterPosition) {
                newX--;
            }
        } else { // This invader is going to move randomly
            // Randomly change the invader's x position, within the range of the screen
            int direction = rand() % 2 ? 1 : -1; // Randomly choose a direction, either 1 or -1
            newX += direction;
        }

        // Ensure the new position is within the game screen
        if (newX < 0) newX = 0;
        if (newX >= MAX_X) newX = MAX_X - 1;

        // Calculate new Y position
        int newY = game.invaderPositions[invaderIndex].y + 1;

        // Check if the new position is occupied by another invader
        int positionOccupied = 0;
        for (int i = 0; i < game.numInvaders; i++) {
            if (i != invaderIndex && game.invaderPositions[i].x == newX && game.invaderPositions[i].y == newY) {
                positionOccupied = 1;
                break;
            }
        }

        // Only update the position if it's not occupied
        if (!positionOccupied) {
            game.invaderPositions[invaderIndex].x = newX;
            game.invaderPositions[invaderIndex].y = newY;
        }

        sendDataToClient();
        pthread_mutex_unlock(&lock);
        sleep(4); // Pause for a while before next update
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

    pthread_t* invader_threads = malloc(sizeof(pthread_t) * game.numInvaders);
    invader_args* args = malloc(sizeof(invader_args) * game.numInvaders);

    for (int i = 0; i < game.numInvaders; i++) {
        int *arg = malloc(sizeof(*arg));
        if (arg == NULL) {
            fprintf(stderr, "Couldn't allocate memory for thread arg.\n");
            exit(EXIT_FAILURE);
        }

        *arg = i;
        pthread_create(&threads[i], NULL, invader_behavior, arg);
    }

    // Join the threads after they have finished
    for (int i = 0; i < game.numInvaders; i++) {
        pthread_join(invader_threads[i], NULL);
    }

    // Wait for all threads to finish
    for (int i = 0; i < game.numInvaders + 2; i++) { // Offset by 2 for WebSocket and command threads
        pthread_join(threads[i], NULL);
    }

    // Cleanup
    pthread_mutex_destroy(&lock);
    free(game.invaderPositions);
    free(threads);
    free(invader_threads);
    free(args);
    return 0;
}
