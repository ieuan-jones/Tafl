#ifndef UTILS_H
#define UTILS_H

#include "ai.h"

typedef struct LinkedListNode {
    struct void *payload;
    struct LinkedListNode *next;
} LinkedListNode;

struct AIMoveTree* lookup_list_move(uint8_t piece, uint8_t move, struct AIMoveList *list);
void add_list_move(struct AIMoveTree *moves, struct AIMoveList *list);

#endif
