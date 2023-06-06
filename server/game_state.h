#ifndef GAME_STATE_H
#define GAME_STATE_H
#include <pthread.h>

// Define game state

typedef struct InvaderPosition {
    int x;
    int y;
    int id;
    time_t lastShootTime; // time of last shot
} InvaderPosition;


struct GameState {
    int numInvaders;
    InvaderPosition* invaderPositions;
    int shooterPosition;
    int score;
    int lives;
    int invaderAboutToShoot;
    pthread_cond_t cond;  // The condition variable
    int count;  // The counter
};

extern struct GameState game;  // declare game state as extern

#endif
