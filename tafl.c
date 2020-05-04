#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

struct State {
    char board[11][11];
    char white[12][2];
    char black[24][2];
    char king[2];
    char white_count;
    char black_count;
};

struct MoveSet {
    char moves[25][120][2];
    char moves_per_piece[25];
    short int move_count;
};

void add_piece(char x, char y, char type, struct State *state) {
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

void move_piece(char from_x, char from_y, char to_x, char to_y, char side, struct State *state) {
    //printf("%d, %d | %d, %d\n", from_x, from_y, to_x, to_y);
    state->board[from_y][from_x] = 0;
    state->board[to_y][to_x] = side;

    switch(side) {
        case 1:
            for(char i=0;i<state->white_count;i++) {
                if(state->white[i][0] == from_x && state->white[i][1] == from_y) {
                    state->white[i][0] = to_x;
                    state->white[i][1] = to_y;
                }
            }
            break;
        case 2:
            for(char i=0;i<state->white_count;i++) {
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

void initialise_state(struct State *state) {
    state->white_count = 0;
    state->black_count = 0;

    for(short int x=0; x<11; x++) {
        for(short int y=0; y<11; y++) {
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

char is_restricted_tile(char x, char y, char type) {
    if(type == 3) return 0;
    if(x == 0 && y == 0) return 1;
    if(x == 0 && y == 10) return 1;
    if(x == 10 && y == 0) return 1;
    if(x == 10 && y == 10) return 1;
    if(x == 5 && y == 5) return 1;
    return 0;
}

char list_legal_moves_for_piece(char p_x, char p_y, char type, struct State *state, char moves[120][2]) {
    char moves_found = 0;

    for(char x=p_x; x>=0; x--) {
        if(x != p_x && state->board[p_y][x] != 0) {
            break;
        }
        if(x != p_x && state->board[p_y][x] == 0 && !is_restricted_tile(x, p_y, type)) {
            moves[moves_found][0] = x;
            moves[moves_found][1] = p_y;
            moves_found++;
        }
    }

    for(char x=p_x; x<11; x++) {
        if(x != p_x && state->board[p_y][x] != 0) {
            break;
        }
        if(x != p_x && state->board[p_y][x] == 0 && !is_restricted_tile(x, p_y, type)) {
            moves[moves_found][0] = x;
            moves[moves_found][1] = p_y;
            moves_found++;
        }
    }

    for(char y=p_y; y>=0; y--) {
        if(y != p_y && state->board[y][p_x] != 0) {
            break;
        }
        if(y != p_y && state->board[y][p_x] == 0 && !is_restricted_tile(p_x, y, type)) {
            moves[moves_found][0] = p_x;
            moves[moves_found][1] = y;
            moves_found++;
        }
    }

    for(char y=p_y; y<11; y++) {
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

void list_legal_moves(char side, struct State *state, struct MoveSet *legal_moves) {
    char pieces[25][2];
    char piece_count;
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

    for(char i=0;i<piece_count;i++) {
        moves_found = list_legal_moves_for_piece(pieces[i][0], pieces[i][1], side, state, legal_moves->moves[i]);
        legal_moves->move_count += moves_found;
        legal_moves->moves_per_piece[i] = moves_found;
    }

    if(side == 1) {
        moves_found = list_legal_moves_for_piece(state->king[0], state->king[1], 3, state, legal_moves->moves[piece_count]);
        legal_moves->move_count += moves_found;
        legal_moves->moves_per_piece[piece_count] = moves_found;
    }
}

char state_of_game(struct State *state) {
    // 0 - continue play
    // 1 - draw
    // 2 - white win
    // 3 - black win
    
    char k_x = state->king[0];
    char k_y = state->king[1];
    char w_pc = state->white_count;
    
    // King escaped
    if(k_x == 0 && k_y == 0) return 2;
    if(k_x == 0 && k_y == 10) return 2;
    if(k_x == 10 && k_y == 0) return 2;
    if(k_x == 10 && k_y == 10) return 2;

    // King surrounded
    if((state->board[k_y][k_x-1] == 2 || k_x == 6 || (w_pc == 0 && k_x == 0)) &&
        (state->board[k_y][k_x+1] == 2 || k_x == 4 || (w_pc == 0 && k_x == 10)) &&
        (state->board[k_y-1][k_x] == 2 || k_y == 6 || (w_pc == 0 && k_y == 0)) &&
        (state->board[k_y+1][k_x] == 2 || k_y == 6 || (w_pc == 0 && k_y == 10))) {
        return 3;
    }

    // White surrounded
    char seen;
    char unbroken = 0;
    for(char y=0;y<11;y++) {
        seen = 0;
        for(char x=0;x<11;x++) {
            switch (state->board[y][x]) {
                case 0:
                    break;
                case 1:
                    if(seen == 0) seen = 1; // Far left counter is white
                    if(seen == 2) seen = 3;
                    break;
                case 2:
                    seen = 2;
                    break;
            }
            if(seen == 1) {
                break;
            }
        }
        if(seen == 0 && unbroken) unbroken = 2; // A line lacking tiles after one surrounded with black
        if(seen == 1 || seen == 3) return 0; // Outside white counter
        if(seen == 2 && unbroken == 2) return 0; // We have a gap
        if(seen == 2) unbroken = 1; // Black on the outside
    }

    unbroken = 0;
    for(char x=0;x<11;x++) {
        seen = 0;
        for(char y=0;y<11;y++) {
            switch (state->board[y][x]) {
                case 0:
                    break;
                case 1:
                    if(seen == 0) seen = 1; // Far left counter is white
                    if(seen == 2) seen = 3;
                    break;
                case 2:
                    seen = 2;
                    break;
            }
            if(seen == 1) {
                break;
            }
        }
        if(seen == 0 && unbroken) unbroken = 2; // A line lacking tiles after one surrounded with black
        if(seen == 1 || seen == 3) return 0; // Outside white counter
        if(seen == 2 && unbroken == 2) return 0; // We have a gap
        if(seen == 2) unbroken = 1; // Black on the outside
    }

    return 0;
}

void draw_board(struct State *state) {
    for(char y=0; y<11; y++) {
        for(char x=0; x<11; x++) {
            if (state->board[y][x] == 0) printf(".");
            else printf("%d", state->board[y][x]);
        }
        printf("\n");
    }
    for(char x=0; x<11; x++) printf("=");
    printf("\n\n");
}

int main(int argc, char **argv) {
    struct State gameState;
    struct MoveSet legal_moves;
    char piece;
    char counter;
    char good_piece;
    short int move;
    
    initialise_state(&gameState);
    draw_board(&gameState);

    state_of_game(&gameState);
    int i;
    for(i=0;i<100000;i++){
        list_legal_moves(1, &gameState, &legal_moves);
        good_piece = 0;
        counter = 0;
        while(!good_piece && counter < 100) {
            piece = rand() % gameState.white_count+1; 
            if(legal_moves.moves_per_piece[piece] > 0) good_piece = 1;
            counter++;
        }
        if(counter == 100) break;
        move = rand() % legal_moves.moves_per_piece[piece];
        if(piece < gameState.white_count)
            move_piece(gameState.white[piece][0], gameState.white[piece][1], legal_moves.moves[piece][move][0], legal_moves.moves[piece][move][1], 1, &gameState);
        else
            move_piece(gameState.king[0], gameState.king[1], legal_moves.moves[piece][move][0], legal_moves.moves[piece][move][1], 3, &gameState);
        if(state_of_game(&gameState)) break;

        list_legal_moves(2, &gameState, &legal_moves);
        good_piece = 0;
        counter = 0;
        while(!good_piece && counter < 100) {
            piece = rand() % gameState.black_count;
            if(legal_moves.moves_per_piece[piece] > 0) good_piece = 1;
            counter++;
        }
        if(counter == 100) break;
        move = rand() % legal_moves.moves_per_piece[piece];
        move_piece(gameState.black[piece][0], gameState.black[piece][1], legal_moves.moves[piece][move][0], legal_moves.moves[piece][move][1], 2, &gameState);
        if(state_of_game(&gameState)) break;
    }

    draw_board(&gameState);
    printf("Moves: %d\n", i);

    return 0;
}
