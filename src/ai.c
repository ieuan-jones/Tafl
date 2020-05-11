#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ai.h"
#include "utils.h"

uint8_t ai_pick_move(struct MoveSet *moves, struct State *state, unsigned int *seed, short int max_depth, struct TaflMove *picked_move) {
    if(ai_pick_winning_move(moves, state, picked_move)) return 1;
    
    struct AIMoveTree move_tree;
    struct AIMoveList move_list;
    move_list.next = NULL;
    uint8_t piece;
    uint8_t move;

    for(int i=0; i<1000; i++) {
        piece = ai_pick_piece(moves, state, seed);
        move = rand_r(seed) % moves->moves_per_piece[piece];

        move_tree.piece = piece;
        move_tree.move = move;
        move_tree.moves = &move_list;
        move_tree.visits = 0;
        move_tree.score = 0;

        return move;
    }

    return 0;
}

uint8_t ai_pick_winning_move(struct MoveSet *moves, struct State *state, struct TaflMove *picked_move) {
    uint8_t piece_count = 0;
    struct State moved_state;
    memcpy(&moved_state, state, sizeof(*state));

    switch(state->turn) {
        case 0:
            piece_count = state->white_count+1;
            break;
        case 1:
            piece_count = state->white_count;
            break;
    }

    for(int8_t piece=piece_count; piece>=0; piece--) {
        for(uint8_t move=0; move<moves->moves_per_piece[piece]; move++) {
            if(do_move(piece, move, moves, &moved_state) > 1) {
                picked_move->piece = piece;
                picked_move->move = move;

                return 1;
            }
        }
    }

    return 0;
}

int8_t ai_pick_piece(struct MoveSet *moves, struct State *state, unsigned int *seed) {
    int8_t good_piece = -1;
    uint8_t i = 0;
    uint8_t piece = 0;

    while(good_piece == -1 && i < 100) {
        switch(state->turn) {
            case 0:
                piece = rand_r(seed) % (state->white_count+1);
                break;
            case 1:
                piece = rand_r(seed) % (state->black_count);
                break;
        }
        if(moves->moves_per_piece[piece] > 0) good_piece = piece;
        i++;
    }

    return good_piece;
}
