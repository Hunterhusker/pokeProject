#ifndef MINHEAP_H
#define MINHEAP_H

#include "mapBuilder.h" // allows us to return cell_t and take it as inputs
#include <vector>

/// Heap structs so that they can be used in here
typedef struct heapNode
{
    cell_t  *data;
    int *value;
    int x, y;
} heapNode_t;

class minHeap {
private:
    //heapNode_t heap[1860];
    std::vector<heapNode> heap;
    int currLen;

public:
    minHeap();

    void mhSwap(int idx1, int idx2);
    void heapifyUpCell(cell_t *cell);
    void heapifyUp(int srcIdx);
    void heapifyDown(int srcIdx);
    void mhAdd(cell_t *cell);
    cell_t mhExtract();
    int mhFind(cell_t *cell);
    void mhResetMap(cell_t map[21][80]);
    void mhAddAll(cell_t map[21][80]);
    void mhDeleteElement(cell_t *theDoomed);
    cell_t *peek();
};



#endif
