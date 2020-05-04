#include <stdio.h>
#include <string.h>

struct State {
    char board[11][11];
    char white[12][2];
    char black[24][2];
    char king[2];
    char white_count;
    char black_count;
};

struct MoveSet {
    char moves[24][120][2];
    char moves_per_piece[24];
    short int move_count;
};

void add_piece(char type, char x, char y, struct State *state) {
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

void initialise_state(struct State *state) {
    state->white_count = 0;
    state->black_count = 0;

    for(short int x=0; x<11; x++) {
        for(short int y=0; y<11; y++) {
            add_piece(0, x, y, state);
        }
    }

    add_piece(2, 3, 0, state);
    add_piece(2, 4, 0, state);
    add_piece(2, 5, 0, state);
    add_piece(2, 6, 0, state);
    add_piece(2, 7, 0, state);
    add_piece(2, 5, 1, state);

    add_piece(2, 3, 10, state);
    add_piece(2, 4, 10, state);
    add_piece(2, 5, 10, state);
    add_piece(2, 6, 10, state);
    add_piece(2, 7, 10, state);
    add_piece(2, 5, 9, state);

    add_piece(2, 0, 3, state);
    add_piece(2, 0, 4, state);
    add_piece(2, 0, 5, state);
    add_piece(2, 0, 6, state);
    add_piece(2, 0, 7, state);
    add_piece(2, 1, 5, state);

    add_piece(2, 10, 3, state);
    add_piece(2, 10, 4, state);
    add_piece(2, 10, 5, state);
    add_piece(2, 10, 6, state);
    add_piece(2, 10, 7, state);
    add_piece(2, 9, 5, state);

    add_piece(1, 5, 3, state);
    add_piece(1, 4, 4, state);
    add_piece(1, 5, 4, state);
    add_piece(1, 6, 4, state);
    add_piece(1, 3, 5, state);
    add_piece(1, 4, 5, state);
    add_piece(1, 6, 5, state);
    add_piece(1, 7, 5, state);
    add_piece(1, 4, 6, state);
    add_piece(1, 5, 6, state);
    add_piece(1, 6, 6, state);
    add_piece(1, 5, 7, state);

    add_piece(3, 5, 5, state);
}

char list_legal_moves_for_piece(char p_x, char p_y, struct State *state, char moves[120][2]) {
    char moves_found = 0;

    for(char x=p_x; x>=0; x--) {
        if(x != p_x && state->board[p_y][x] != 0) {
            break;
        }
        if(x != p_x && state->board[p_y][x] == 0) {
            moves[moves_found][0] = x;
            moves[moves_found][1] = p_y;
            moves_found++;
        }
    }

    for(char x=p_x; x<11; x++) {
        if(x != p_x && state->board[p_y][x] != 0) {
            break;
        }
        if(x != p_x && state->board[p_y][x] == 0) {
            moves[moves_found][0] = x;
            moves[moves_found][1] = p_y;
            moves_found++;
        }
    }

    for(char y=p_y; y>=0; y--) {
        if(y != p_y && state->board[y][p_x] != 0) {
            break;
        }
        if(y != p_y && state->board[y][p_x] == 0) {
            moves[moves_found][0] = p_x;
            moves[moves_found][1] = y;
            moves_found++;
        }
    }

    for(char y=p_y; y<11; y++) {
        if(y != p_y && state->board[y][p_x] != 0) {
            break;
        }
        if(y != p_y && state->board[y][p_x] == 0) {
            moves[moves_found][0] = p_x;
            moves[moves_found][1] = y;
            moves_found++;
        }
    }

    return moves_found;
}

void list_legal_moves(char side, struct State *state, struct MoveSet *legal_moves) {
    char pieces[24][2];
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
        moves_found = list_legal_moves_for_piece(pieces[i][0], pieces[i][1], state, legal_moves->moves[i]);
        legal_moves->move_count += moves_found;
        legal_moves->moves_per_piece[i] = moves_found;
    }
}

int main(int argc, char **argv) {
    struct State gameState;
    initialise_state(&gameState);

    for(char y=0; y<11; y++) {
        for(char x=0; x<11; x++) {
            if (gameState.board[y][x] == 0) printf(".");
            else printf("%d", gameState.board[y][x]);
        }
        printf("\n");
    }

    struct MoveSet legal_moves;
    list_legal_moves(2, &gameState, &legal_moves);

    printf("%d\n", legal_moves.move_count);

    for(char i=0; i<24; i++)
        for(char j=0; j<legal_moves.moves_per_piece[i]; j++)
            printf("%d, %d/%d: %d, %d\n", i, j, legal_moves.moves_per_piece[i], legal_moves.moves[i][j][0], legal_moves.moves[i][j][1]);

    return 0;
}
