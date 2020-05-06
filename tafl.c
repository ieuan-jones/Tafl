#include "tafl.h"
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

char type_to_side(char type) {
    switch(type) {
        case 1:
            return 1;
            break;
        case 2:
            return 2;
            break;
        case 3:
            return 1;
            break;
        default:
            return 0;
            break;
    }
}

char opposite_side(char type) {
    switch(type) {
        case 1:
            return 2;
            break;
        case 2:
            return 1;
            break;
        case 3:
            return 2;
            break;
        default:
            return 0;
            break;
    }
}

uint8_t is_restricted_tile(uint8_t x, uint8_t y, uint8_t type) {
    if(type == 3) return 0;
    if(x == 0 && y == 0) return 1;
    if(x == 0 && y == 10) return 1;
    if(x == 10 && y == 0) return 1;
    if(x == 10 && y == 10) return 1;
    if(x == 5 && y == 5) return 1;
    return 0;
}

void add_piece(uint8_t x, uint8_t y, uint8_t type, struct State *state) {
    state->board[y][x] = type;

    switch (type) {
        case 1:
            state->white[state->white_count][0] = x;
            state->white[state->white_count][1] = y;
            state->white_count++;
            break;
        case 2:
            state->black[state->black_count][0] = x;
            state->black[state->black_count][1] = y;
            state->black_count++;
            break;
        case 3:
            state->king[0] = x;
            state->king[1] = y;
            break;
    }
}

void remove_piece(uint8_t x, uint8_t y, uint8_t type, struct State *state) {
    //printf("%d, %d, %d\n", x, y, type);
    if(state->board[y][x] != type) printf("PIECE NOT ON BOARD!\n");
    state->board[y][x] = 0;
    int8_t index = -1;

    switch (type) {
        case 1:
            for(int8_t i=0;i<state->white_count;i++) {
                if(state->white[i][0] == x && state->white[i][1] == y) {
                    index = i;
                    break;
                }
            }

            if(index != -1) {
                state->white[index][0] = state->white[state->white_count-1][0];
                state->white[index][1] = state->white[state->white_count-1][1];
                state->white_count--;
            }
            break;
        case 2:
            for(int8_t i=0;i<state->black_count;i++) {
                if(state->black[i][0] == x && state->black[i][1] == y) {
                    index = i;
                    break;
                }
            }

            if(index != -1) {
                state->black[index][0] = state->black[state->black_count-1][0];
                state->black[index][1] = state->black[state->black_count-1][1];
                state->black_count--;
            }
            break;
    }
}

void move_piece(uint8_t from_x, uint8_t from_y, uint8_t to_x, uint8_t to_y, uint8_t type, struct State *state) {
    state->board[from_y][from_x] = 0;
    state->board[to_y][to_x] = type;

    switch(type) {
        case 1:
            for(int8_t i=0;i<state->white_count;i++) {
                if(state->white[i][0] == from_x && state->white[i][1] == from_y) {
                    state->white[i][0] = to_x;
                    state->white[i][1] = to_y;
                }
            }
            break;
        case 2:
            for(int8_t i=0;i<state->black_count;i++) {
                if(state->black[i][0] == from_x && state->black[i][1] == from_y) {
                    state->black[i][0] = to_x;
                    state->black[i][1] = to_y;
                }
            }
            break;
        case 3:
            state->king[0] = to_x;
            state->king[1] = to_y;
            break;
    }
}

char state_of_game(struct State *state) {
    // 0 - continue play
    // 1 - draw
    // 2 - white win
    // 3 - black win
    
    uint8_t k_x = state->king[0];
    uint8_t k_y = state->king[1];
    uint8_t w_pc = state->white_count;
    

    if(state->black_count == 0) return 2;

    // King escaped
    if(k_x == 0 && k_y == 0) return 2;
    if(k_x == 0 && k_y == 10) return 2;
    if(k_x == 10 && k_y == 0) return 2;
    if(k_x == 10 && k_y == 10) return 2;

    // King surrounded
    if(state->turn == 1 &&
        ((k_x > 0 && state->board[k_y][k_x-1] == 2) || (k_x == 6 && k_y == 5) || (w_pc == 0 && k_x == 0)) &&
        ((k_x < 10 && state->board[k_y][k_x+1] == 2) || (k_x == 4 && k_y == 5) || (w_pc == 0 && k_x == 10)) &&
        ((k_y > 0 && state->board[k_y-1][k_x] == 2) || (k_x == 5 && k_y == 6) || (w_pc == 0 && k_y == 0)) &&
        ((k_y < 10 && state->board[k_y+1][k_x] == 2) || (k_x == 5 && k_y == 4) || (w_pc == 0 && k_y == 10))) {
        return 3;
    }

    // White surrounded
    uint8_t broken = 0;
    uint8_t first_colour = 0;
    uint8_t last_colour = 0;
    int8_t left = 0;
    int8_t right = 0;
    int8_t p_left = 0;
    int8_t p_right = 0;
    for(int8_t x=0;x<11;x++) {
        first_colour = 0;
        last_colour = 0;
        p_left = left;
        p_right = right;
        left = -1;
        right = -1;
        for(int8_t y=0;y<11;y++) {
            switch(type_to_side(state->board[y][x])) {
                case 0:
                    break;
                case 1:
                    if(first_colour == 0) first_colour = 1;
                    last_colour = 1;
                    break;
                case 2:
                    if(left == -1) left = x;
                    if(first_colour == 0) first_colour = 2;
                    right = x;
                    last_colour = 2;
                    break;
            }
        }
        if(first_colour == 1) {
            broken = 1;
            break;
        }
        if(last_colour == 1) {
            broken = 1;
            break;
        }
        if(x > 0 && left > -1 && abs(p_left-left) > 1) {
            broken = 1;
            break;
        }
        if(x > 0 && right > -1 && abs(p_right-right) > 1) {
            broken = 1;
            break;
        }
        if(broken) return 0;
    }

    for(int8_t y=0;y<11;y++) {
        first_colour = 0;
        last_colour = 0;
        p_left = left;
        p_right = right;
        left = -1;
        right = -1;
        for(int8_t x=0;x<11;x++) {
            switch(type_to_side(state->board[y][x])) {
                case 0:
                    break;
                case 1:
                    if(first_colour == 0) first_colour = 1;
                    last_colour = 1;
                    break;
                case 2:
                    if(left == -1) left = x;
                    if(first_colour == 0) first_colour = 2;
                    right = x;
                    last_colour = 2;
                    break;
            }
        }
        if(first_colour == 1) {
            broken = 1;
            break;
        }
        if(y > 0 && left > -1 && abs(p_left-left) > 1) {
           broken = 1;
           break;
        }
        if(y > 0 && right > -1 && abs(p_right-right) > 1) {
            broken = 1;
            break;
        }
        if(broken) return 0;
    }

    if(!broken) {
        printf("Surrounded!");
        return 3;
    }

    return 0;
}

uint8_t tile_has_takeable_piece(uint8_t x, uint8_t y, uint8_t type, struct State *state) {
    // Does the tile have a piece that can be taken?
    uint8_t checked_square = state->board[y][x];

    if(checked_square != 0 && checked_square != 3 && type_to_side(checked_square) != type_to_side(type))
        return 1;

    return 0;
}

uint8_t tile_has_defender(uint8_t x, uint8_t y, uint8_t type, struct State *state) {
    // Can tile can be used for taking?
    uint8_t checked_square = type_to_side(state->board[y][x]);

    if((checked_square != 0 && checked_square == type_to_side(type)) ||
            is_restricted_tile(x, y, 1))
            return 1;

    return 0;
}

int do_captures(uint8_t to_x, uint8_t to_y, uint8_t type, struct State *state) {
    if(to_y > 1 && to_y < 9) {
        if(tile_has_takeable_piece(to_x, to_y-1, type, state) && tile_has_defender(to_x, to_y-2, type, state)) {
            remove_piece(to_x, to_y-1, opposite_side(type), state);
            return 1;
        }
        if(tile_has_takeable_piece(to_x, to_y+1, type, state) && tile_has_defender(to_x, to_y+2, type, state)) {
            remove_piece(to_x, to_y+1, opposite_side(type), state);
            return 1;
        }
    }
    if(to_x > 1 && to_x < 9) {
        if(tile_has_takeable_piece(to_x-1, to_y, type, state) && tile_has_defender(to_x-2, to_y, type, state)) {
            remove_piece(to_x-1, to_y, opposite_side(type), state);
            return 1;
        }
        if(tile_has_takeable_piece(to_x+1, to_y, type, state) && tile_has_defender(to_x+2, to_y, type, state)) {
            remove_piece(to_x+1, to_y, opposite_side(type), state);
            return 1;
        }
    }

    return 0;
}

int do_move(uint8_t piece, uint8_t move, struct MoveSet *moves, struct State *state) {
    uint8_t from_x = 0;
    uint8_t from_y = 0;
    uint8_t type = 0;
    uint8_t result = 0;

    switch(state->turn) {
        case 0:
            if(piece < state->white_count) {
                from_x = state->white[piece][0];
                from_y = state->white[piece][1];
                type = 1;
            } else {
                from_x = state->king[0];
                from_y = state->king[1];
                type = 3;
            }
            break;
        case 1:
            from_x = state->black[piece][0];
            from_y = state->black[piece][1];
            type = 2;
            break;
    }

    move_piece(
        from_x,
        from_y,
        moves->moves[piece][move][0],
        moves->moves[piece][move][1],
        type,
        state
    );

    do_captures(
        moves->moves[piece][move][0],
        moves->moves[piece][move][1],
        type,
        state
    );

    result = state_of_game(state);

    state->turn = 1-state->turn;

    return result;
}

void initialise_state(struct State *state) {
    state->white_count = 0;
    state->black_count = 0;
    state->turn = 0;

    for(int8_t x=0; x<11; x++) {
        for(int8_t y=0; y<11; y++) {
            add_piece(x, y, 0, state);
        }
    }

    add_piece(3, 0, 2, state);
    add_piece(4, 0, 2, state);
    add_piece(5, 0, 2, state);
    add_piece(6, 0, 2, state);
    add_piece(7, 0, 2, state);
    add_piece(5, 1, 2, state);

    add_piece(3, 10, 2, state);
    add_piece(4, 10, 2, state);
    add_piece(5, 10, 2, state);
    add_piece(6, 10, 2, state);
    add_piece(7, 10, 2, state);
    add_piece(5, 9, 2, state);

    add_piece(0, 3, 2, state);
    add_piece(0, 4, 2, state);
    add_piece(0, 5, 2, state);
    add_piece(0, 6, 2, state);
    add_piece(0, 7, 2, state);
    add_piece(1, 5, 2, state);

    add_piece(10, 3, 2, state);
    add_piece(10, 4, 2, state);
    add_piece(10, 5, 2, state);
    add_piece(10, 6, 2, state);
    add_piece(10, 7, 2, state);
    add_piece(9, 5, 2, state);

    add_piece(5, 3, 1, state);
    add_piece(4, 4, 1, state);
    add_piece(5, 4, 1, state);
    add_piece(6, 4, 1, state);
    add_piece(3, 5, 1, state);
    add_piece(4, 5, 1, state);
    add_piece(6, 5, 1, state);
    add_piece(7, 5, 1, state);
    add_piece(4, 6, 1, state);
    add_piece(5, 6, 1, state);
    add_piece(6, 6, 1, state);
    add_piece(5, 7, 1, state);

    add_piece(5, 5, 3, state);
}

uint8_t list_legal_moves_for_piece(uint8_t p_x, uint8_t p_y, uint8_t type, struct State *state, uint8_t moves[120][2]) {
    uint8_t moves_found = 0;

    for(int8_t x=p_x; x>=0; x--) {
        if(x != p_x && state->board[p_y][x] != 0) {
            break;
        }
        if(x != p_x && state->board[p_y][x] == 0 && !is_restricted_tile(x, p_y, type)) {
            moves[moves_found][0] = x;
            moves[moves_found][1] = p_y;
            moves_found++;
        }
    }

    for(int8_t x=p_x; x<11; x++) {
        if(x != p_x && state->board[p_y][x] != 0) {
            break;
        }
        if(x != p_x && state->board[p_y][x] == 0 && !is_restricted_tile(x, p_y, type)) {
            moves[moves_found][0] = x;
            moves[moves_found][1] = p_y;
            moves_found++;
        }
    }

    for(int8_t y=p_y; y>=0; y--) {
        if(y != p_y && state->board[y][p_x] != 0) {
            break;
        }
        if(y != p_y && state->board[y][p_x] == 0 && !is_restricted_tile(p_x, y, type)) {
            moves[moves_found][0] = p_x;
            moves[moves_found][1] = y;
            moves_found++;
        }
    }

    for(int8_t y=p_y; y<11; y++) {
        if(y != p_y && state->board[y][p_x] != 0) {
            break;
        }
        if(y != p_y && state->board[y][p_x] == 0 && !is_restricted_tile(p_x, y, type)) {
            moves[moves_found][0] = p_x;
            moves[moves_found][1] = y;
            moves_found++;
        }
    }

    return moves_found;
}

void list_legal_moves(uint8_t side, struct State *state, struct MoveSet *legal_moves) {
    uint8_t pieces[BLACK_PIECE_COUNT][2];
    uint8_t piece_count = 0;
    short int moves_found = 0;
    legal_moves->move_count = 0;

    switch(side) {
        case 1:
            memcpy(&pieces, &state->white, sizeof state->white);
            piece_count = state->white_count;
            break;
        case 2:
            memcpy(&pieces, &state->black, sizeof state->black);
            piece_count = state->black_count;
            break;
    }

    for(int8_t i=0;i<piece_count;i++) {
        moves_found = list_legal_moves_for_piece(pieces[i][0], pieces[i][1], side, state, legal_moves->moves[i]);
        legal_moves->move_count += moves_found;
        legal_moves->moves_per_piece[i] = moves_found;
    }

    if(side == 1) {
        moves_found = list_legal_moves_for_piece(state->king[0], state->king[1], 3, state, legal_moves->moves[piece_count]);
        legal_moves->move_count += moves_found;
        legal_moves->moves_per_piece[piece_count] = moves_found;
    }

    /*if(side == 2) {
        for(int8_t i=0;i<state->black_count;i++) {
            if(side == 1) printf("%d, %d\n", state->black[i][0], state->black[i][1]);
            for(int8_t j=0;j<legal_moves->moves_per_piece[i];j++)
                printf("[%d,%d], ", legal_moves->moves[i][j][0], legal_moves->moves[i][j][1]);
            printf("\n");
        }
    }*/
}

void draw_board(struct State *state) {
    for(int8_t y=0; y<11; y++) {
        for(int8_t x=0; x<11; x++) {
            if (state->board[y][x] == 0) printf(".");
            else printf("%d", state->board[y][x]);
        }
        printf("\n");
    }
    for(int x=0; x<11; x++) printf("=");
    printf("\n\n");
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

    for(int i=0;i<100000;i++) {
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

int main(int argc, char **argv) {
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
