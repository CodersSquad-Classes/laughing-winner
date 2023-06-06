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
#include "shared.h"
#include <time.h>

#define MAX_X 10
#define MAX_Y 10
#define SHOOTING_COOLDOWN 5
struct GameState game;
Queue* commandQueue;

// Mutex for synchronization
pthread_mutex_t lock_game;
pthread_mutex_t lock_invaders;

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

void* invader_behavior(void* arg) {
    srand(time(NULL)); // Seed the random number generator
    sleep(3); // Pause for a while before next update

    // Each invader needs to know its index
    int invaderIndex = *((int*)arg);
    free(arg); // We're done with the argument now

    // Logic for individual invader
    while (game.lives > 0 && game.invaderPositions[invaderIndex].y < MAX_Y) { // Also stop if invader reaches the end
        pthread_mutex_lock(&lock_invaders);
        //printf("moviendo a los bots\n");

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

        // Check if the new position collides with the shooter
        if (newY == MAX_Y - 1 && newX == game.shooterPosition) {
            game.lives--; // Subtract a life from the player

            // Remove the invader from the game
            for (int j = invaderIndex; j < game.numInvaders - 1; j++) {
                game.invaderPositions[j] = game.invaderPositions[j + 1];
            }

            game.numInvaders--;
        } else {
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
        }

        // Check if the invader reaches the player's y position
        if (newY == MAX_Y - 1 && newX == game.shooterPosition) {
            game.lives--; // Subtract a life from the player
        }

        game.count++;
        pthread_cond_signal(&game.cond);
        pthread_mutex_unlock(&lock_invaders);
        sleep(3); // Pause for a while before next update
        sendDataToClient();
    }

    // Unlock the mutex if the invader reaches the end or collides with the shooter
    pthread_mutex_unlock(&lock_invaders);

    return NULL;
}


void process_start_command(char* command) {
    int numInvaders = atoi(&command[6]); // Extract the number of invaders from the command message
    if (numInvaders >= 1 && numInvaders <= 20) {
        // Initialize game state with the specified number of invaders
        game.numInvaders = numInvaders;
        game.invaderPositions = realloc(game.invaderPositions, game.numInvaders * sizeof(InvaderPosition));

        // Initialize invaders positions
        for (int i = 0; i < game.numInvaders; i++) {
            game.invaderPositions[i].x = i % MAX_X;
            game.invaderPositions[i].y = i / MAX_X;
            game.invaderPositions[i].id = i;
        }

        // Send game state to the client
        sendDataToClient();

        // Create invader threads
        pthread_t* invader_threads = malloc(sizeof(pthread_t) * game.numInvaders);
        for (int i = 0; i < game.numInvaders; i++) {
            int* arg = malloc(sizeof(*arg));
            if (arg == NULL) {
                fprintf(stderr, "Couldn't allocate memory for thread arg.\n");
                exit(EXIT_FAILURE);
            }

            *arg = i;
            pthread_create(&invader_threads[i], NULL, invader_behavior, arg);
        }
    }
}

void* process_commands(void* arg) {
    Queue* queue = (Queue*) arg;
    while (1) {
        char* command = deQueue(queue);
        if (command != NULL) {
            // Process the command
            pthread_mutex_lock(&lock_game);
            if (strncmp(command, "start", 5) == 0) {
                // The "start" command is now processed in the WebSocket server callback
                process_start_command(command);
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
            } else if (strncmp(command, "shoot", 5) == 0) {
                // Shoot
                playerShot();
            }
            sendDataToClient();
            pthread_mutex_unlock(&lock_game);

            // Free the command string after using it
            free(command);
        }

        // Sleep a little to prevent busy-waiting
        usleep(1000);
    }

    return NULL;
}

void* send_data_to_client(void* arg) {
    while (1) {
        // Wait for all invader threads to finish
        pthread_mutex_lock(&lock_invaders);
        while (game.count < game.numInvaders) {
            pthread_cond_wait(&game.cond, &lock_invaders);
        }
        game.count = 0;  // Reset the counter
        pthread_mutex_unlock(&lock_invaders);

        // Send data to client
        sendDataToClient();

        // Sleep a little before the next iteration
        usleep(1000);
    }
    return NULL;
}

int main() {
    // Initialize game state
    game.shooterPosition = 5;
    game.score = 0;
    game.lives = 10;
    game.numInvaders = 0;
    game.invaderPositions = NULL;

    pthread_t *threads = malloc(2 * sizeof(pthread_t)); // For WebSocket thread and command thread

    // Initialize the mutexes
    if (pthread_mutex_init(&lock_game, NULL) != 0 || pthread_mutex_init(&lock_invaders, NULL) != 0) {
        printf("\nMutex init failed\n");
        return 1;
    }

    commandQueue = createQueue();

    // Create WebSocket server thread
    pthread_create(&threads[0], NULL, websocket_server_thread, NULL);

    // Create command processing thread
    pthread_create(&threads[1], NULL, process_commands, commandQueue);

    // Wait for all threads to finish
    for (int i = 0; i < 2; i++) {
        pthread_join(threads[i], NULL);
    }

    // Cleanup
    pthread_mutex_destroy(&lock_game);
    pthread_mutex_destroy(&lock_invaders);
    free(game.invaderPositions);
    free(threads);

    return 0;
}
