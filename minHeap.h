#ifndef MINHEAP_H
#define MINHEAP_H

class cell;

typedef struct heapNode
{
    cell  *data;
    int *value;
    //int x, y;
} heapNode_t;

class minHeap {
public:
    heapNode_t heap[1860];
    int currLen;
};

void mhSwap(minHeap *mh, int idx1, int idx2);
void heapifyUpCell(minHeap *mh, cell *cell);
void heapifyUp(minHeap *mh, int srcIdx);
void heapifyDown(minHeap *mh, int srcIdx);
void mhAdd(minHeap *mh, cell *cell);
cell mhExtract(minHeap *mh);
int mhFind(minHeap *mh, cell *cell);
void mhResetMap(cell map[21][80]);
void mhAddAll(minHeap *mh, cell map[21][80]);
void mhDeleteElement(minHeap *mh, cell *theDoomed);
cell *peek(minHeap *mh);

#endif
