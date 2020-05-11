#include <assert.h>
#include <stddef.h>

#include "utils.h"

struct AIMoveTree* lookup_list_move(uint8_t piece, uint8_t move, struct AIMoveList *list) {
    struct AIMoveList *current = list;

    while(current->next != NULL) {
        if(current->payload->piece == piece && current->payload->move == move) return current->payload;
        current = current->next;
    }

    struct AIMoveTree move_tree;
    move_tree.piece = piece;
    move_tree.move = move;
    move_tree.visits = 0;
    move_tree.score = 0;
    add_list_move(&move_tree, list);
}

void add_list_move(struct AIMoveTree *moves, struct AIMoveList *list) {
    struct AIMoveList *current = list;
    struct AIMoveList next;

    while(current->next != NULL) {
        current = current->next;
    }
    
    next.payload = moves;
    next.next = NULL;
    current->next = &next;
}
