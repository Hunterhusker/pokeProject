#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct listNode {
    void* data;

    struct listNode *next;
    struct listNode *prev;
} listNode_t;

typedef struct LinkedList {
    listNode_t *head;
    listNode_t *end;

    int size;
} linkedList_t;

void* listGet(int idx, linkedList_t *ll);
int listAdd(void* data, linkedList_t *ll);

#endif