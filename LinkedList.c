#include <stdlib.h>
#include <stdio.h>
#include "LinkedList.h"

void* listGet(int idx, linkedList_t *ll)
{
    if (idx > ll->size) {
        return -1;
    }

    listNode_t *curr = ll->head; // set to the head

    // Go to the index in the loop
    for (int i = 1; i < idx; i++) {
        curr = curr->next;
    }

    // return it as void data
    return curr->data;
}

int listAdd(void* data, linkedList_t *ll) {
    listNode_t ln;

    ln.data = data;

    if (ll->size == 0) {

    }
}