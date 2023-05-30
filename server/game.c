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
    // This is the function that will be executed by each invader thread.
    // Each invader will act independently according to some pre-defined logic.

    // This is a simple example where the invader just moves down the board every second.
    while (1) {
        pthread_mutex_lock(&lock);
        // update the invader's position in the game state
        pthread_mutex_unlock(&lock);
        sleep(1);
    }

    return NULL;
}

int main() {
    // Initialize game state
    game.numInvaders = 10;
    game.invaderPositions = malloc(game.numInvaders * sizeof(int));
    game.shooterPosition = 5;
    game.score = 0;
    game.lives = 10;

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
