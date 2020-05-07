#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "tafl.h"
#include "taflboard.h"
#include "taflutil.h"

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

void *calculate_results_thread(void *arg) {
    struct ComputeThreadResults *ctr = (struct ComputeThreadResults *)arg;
    struct State game_state;
    struct MoveSet legal_moves;
    int8_t piece;
    short int move;
    uint8_t result;
    short int moves;
    int white_wins = 0;
    int draws = 0;
    int black_wins = 0;
    int total_moves = 0;
    unsigned int seed = time(NULL)+ctr->t_id;

    for(int i=0;i<10000;i++) {
        initialise_state(&game_state);
        moves = 0;
        while(1) {
            list_legal_moves(game_state.turn+1, &game_state, &legal_moves);
            piece = ai_pick_piece(&legal_moves, &game_state, &seed);

            if(piece != -1) {
                move = rand_r(&seed) % legal_moves.moves_per_piece[piece];
                result = do_move(piece, move, &legal_moves, &game_state);
                if(result == 1) {
                    draws++;
                    break;
                } else if(result == 2) {
                    white_wins++;
                    break;
                } else if(result == 3) {
                    black_wins++;
                    break;
                }
            } else {
                switch(game_state.turn) {
                    case 0:
                        black_wins++;
                        break;
                    case 1:
                        white_wins++;
                        break;
                }
                break;
            }
            moves++;
            total_moves++;
            if(moves > 150) {
                draws++;
                break;
            }
        }
    }

    ctr->white_wins = white_wins;
    ctr->draws = draws;
    ctr->black_wins = black_wins;
    ctr->total_moves = total_moves;

    pthread_exit(NULL);
}

int main() {
    //int white_wins = 0;
    //int black_wins = 0;
    //int draws = 0;

    pthread_t threads[NUM_THREADS];
    struct ComputeThreadResults ctrs[NUM_THREADS];
    for(int i=0;i<NUM_THREADS;i++) {
        ctrs[i].white_wins = 0;
        ctrs[i].draws = 0;
        ctrs[i].black_wins = 0;
        ctrs[i].total_moves = 0;
        ctrs[i].t_id = i;
        pthread_create(&threads[i], NULL, calculate_results_thread, (void *)&ctrs[i]);
    }
    for(int i=0;i<NUM_THREADS;i++) {
        pthread_join(threads[i], NULL);
        printf("Moves / W|D|L: %d / %d, %d, %d\n", ctrs[i].total_moves, ctrs[i].white_wins, ctrs[i].draws, ctrs[i].black_wins);
    }

    return 0;
}
