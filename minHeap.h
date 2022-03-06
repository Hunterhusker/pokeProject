#ifndef MINHEAP_H
#define MINHEAP_H

#include "mapBuilder.h" // allows us to return cell_t and take it as inputs

void mhSwap(minHeap_t *mh, int idx1, int idx2);
void heapifyUpCell(minHeap_t *mh, cell_t *cell);
void heapifyUp(minHeap_t *mh, int srcIdx);
void heapifyDown(minHeap_t *mh, int srcIdx);
void mhAdd(minHeap_t *mh, cell_t *cell);
cell_t mhExtract(minHeap_t *mh);
int mhFind(minHeap_t *mh, cell_t *cell);
void mhResetMap(cell_t map[21][80]);
void mhAddAll(minHeap_t *mh, cell_t map[21][80]);
void mhDeleteElement(minHeap_t *mh, cell_t *theDoomed);

#endif
