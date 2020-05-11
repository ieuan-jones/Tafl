#ifndef AI_H
#define AI_H

#include "taflboard.h"

struct AIMoveTree {
    uint8_t piece;
    uint8_t move;

    struct AIMoveList *moves;

    int visits;
    int score;
};

uint8_t ai_pick_move(struct MoveSet *moves, struct State *state, unsigned int *seed, struct TaflMove *picked_move);
uint8_t ai_pick_winning_move(struct MoveSet *moves, struct State *state, struct TaflMove *picked_move);
int8_t ai_pick_piece(struct MoveSet *moves, struct State *state, unsigned int *seed);

#endif
