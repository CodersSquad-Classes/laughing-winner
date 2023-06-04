#include <pthread.h>
#include <stdlib.h>
#include "game_state.h"
#include <stdio.h>
#include <unistd.h>
#include "game.h"
#include "websocket_server.h"

struct GameState game;

// Mutex for synchronization
pthread_mutex_t lock;

void* invader_behavior(void* arg) {
    int invaderIndex = *(int*) arg; // Retrieve the invader index from the argument

    while (1) {
        pthread_mutex_lock(&lock);

        // Update the invader's position in the game state
        // Example: Move the invader down the board by incrementing the y-coordinate
        game.invaderPositions[invaderIndex].y++;

        pthread_mutex_unlock(&lock);
        sleep(1);
    }

    return NULL;
}



int main() {
    // Initialize game state
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

    pthread_t *threads = malloc(game.numInvaders * sizeof(pthread_t));
    int i;
    start_websocket_server();

    // Initialize the mutex
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n Mutex init failed\n");
        return 1;
    }

    // Create threads for invaders
    for (i = 0; i < game.numInvaders; i++) {
        int *arg = malloc(sizeof(*arg));
        *arg = i;
        pthread_create(&threads[i], NULL, invader_behavior, arg);
    }

    // Main game loop (simplified)
    while (game.lives > 0 && game.numInvaders > 0) {
        // Handle user input and update game state
        // Send game state to client
    }

    // Cleanup
    pthread_mutex_destroy(&lock);
    free(game.invaderPositions);
    free(threads);
    return 0;
}
