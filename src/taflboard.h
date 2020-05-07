#ifndef TAFLBOARD_H
#define TAFLBOARD_H

#include <stdint.h>

#define BLACK_PIECE_COUNT 24

struct State {
    uint8_t board[11][11];
    uint8_t white[12][2];
    uint8_t black[BLACK_PIECE_COUNT][2];
    uint8_t king[2];
    uint8_t white_count;
    uint8_t black_count;
    uint8_t turn;
};

struct MoveSet {
    uint8_t moves[BLACK_PIECE_COUNT][120][2];
    uint8_t moves_per_piece[BLACK_PIECE_COUNT];
    short int move_count;
};

struct ComputeThreadResults {
    int white_wins;
    int draws;
    int black_wins;
    int total_moves;
    int t_id;
};

uint8_t is_restricted_tile(uint8_t x, uint8_t y, uint8_t type);
uint8_t tile_has_takeable_piece(uint8_t x, uint8_t y, uint8_t type, struct State *state);
uint8_t tile_has_defender(uint8_t x, uint8_t y, uint8_t type, struct State *state);

void add_piece(uint8_t x, uint8_t y, uint8_t type, struct State *state);
void remove_piece(uint8_t x, uint8_t y, uint8_t type, struct State *state);
void move_piece(uint8_t from_x, uint8_t from_y, uint8_t to_x, uint8_t to_y, uint8_t type, struct State *state);
int do_move(uint8_t piece, uint8_t move, struct MoveSet *moves, struct State *state);
int do_captures(uint8_t to_x, uint8_t to_y, uint8_t type, struct State *state);

char state_of_game(struct State *state);
uint8_t list_legal_moves_for_piece(uint8_t p_x, uint8_t p_y, uint8_t type, struct State *state, uint8_t moves[120][2]);
void list_legal_moves(uint8_t side, struct State *state, struct MoveSet *legal_moves);

void initialise_state(struct State *state);
void draw_board(struct State *state);

#endif
