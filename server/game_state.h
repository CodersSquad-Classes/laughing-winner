#ifndef GAME_STATE_H
#define GAME_STATE_H

// Define game state

typedef struct InvaderPosition {
    int x;
    int y;
} InvaderPosition;


struct GameState {
    int numInvaders;
    //int *invaderPositions;
    InvaderPosition* invaderPositions;
    int shooterPosition;
    int score;
    int lives;
};

extern struct GameState game;  // declare game state as extern

#endif
