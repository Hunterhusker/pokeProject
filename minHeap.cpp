#include "minHeap.h"
#include <math.h>
#include <limits.h>
#include "mapBuilder.h"

void mhSwap(minHeap *mh, int idx1, int idx2)
{
    heapNode_t tmp = mh->heap[idx1];

    mh->heap[idx1] = mh->heap[idx2];
    mh->heap[idx2] = tmp;
}

void heapifyUp(minHeap *mh, int srcIdx)
{
    if (srcIdx <= 0)
    {
        return;
    }

    // If we try to heapify Up outside the arr, then no don't do that
    if (srcIdx > mh->currLen)
    {
        srcIdx = mh->currLen;
    }

    if (*mh->heap[srcIdx].value < *mh->heap[(int)floor((srcIdx - 1) / 2)].value)
    {
        mhSwap(mh, srcIdx, (int)floor((srcIdx - 1) / 2));

        heapifyUp(mh, (int)floor((srcIdx - 1) / 2));
    }
}

void heapifyUpCell(minHeap *mh, cell *cell)
{
    int srcIdx = mhFind(mh, cell);

    if (srcIdx <= 0)
    {
        return;
    }

    // If we try to heapify Up outside the arr, then no don't do that
    if (srcIdx > mh->currLen)
    {
        srcIdx = mh->currLen;
    }

    if (*mh->heap[srcIdx].value < *mh->heap[(int)floor((srcIdx - 1) / 2)].value)
    {
        mhSwap(mh, srcIdx, (int)floor((srcIdx - 1) / 2));

        heapifyUp(mh, (int)floor((srcIdx - 1) / 2));
    }
}

void heapifyDown(minHeap *mh, int srcIdx)
{
    if ((srcIdx * 2) + 1 < mh->currLen && *mh->heap[(2 * srcIdx) + 1].value < *mh->heap[srcIdx].value)
    {
        mhSwap(mh, (2 * srcIdx) + 1, srcIdx);
        heapifyDown(mh, (2 * srcIdx) + 1);
    }

    if ((srcIdx * 2) + 2 < mh->currLen && *mh->heap[(2 * srcIdx) + 2].value < *mh->heap[srcIdx].value)
    {
        mhSwap(mh, (2 * srcIdx) + 2, srcIdx);
        heapifyDown(mh, (2 * srcIdx) + 2);
    }
}

void mhAdd(minHeap *mh, cell *cell)
{
    heapNode_t hN;

    hN.value = &cell->dist;
    hN.data = cell;
    hN.data->inHeap = true;

    mh->heap[mh->currLen] = hN;

    heapifyUp(mh, mh->currLen);

    mh->currLen++;
}

cell mhExtract(minHeap *mh)
{
    heapNode_t tmp = mh->heap[0];

    mh->heap[0] = mh->heap[mh->currLen - 1];

    mh->currLen--;

    heapifyDown(mh, 0); // cry

    return *tmp.data;
}

int mhFind(minHeap *mh, cell *cell)
{
    for (int i = 0; i < mh->currLen; i++)
    {
        if (mh->heap[i].data->x == cell->x && mh->heap[i].data->y == cell->y)
        {
            return i;
        }
    }

    return -1;
}

void mhResetMap(cell map[21][80])
{
    for (int i = 0; i < 21; i++)
    {
        for (int j = 0; j < 80; j++)
        {
            map[i][j].dist = INT_MAX;

            map[i][j].parentLoc[0] = -1;
            map[i][j].parentLoc[1] = -1;
        }
    }
}

void mhAddAll(minHeap *mh, cell map[21][80])
{
    for (int i = 0; i < 21; i++)
    {
        for (int j = 0; j < 80; j++)
        {
            mhAdd(mh, &map[i][j]);
        }
    }
}

void mhDeleteElement(minHeap *mh, cell *theDoomed)
{
    int i = mhFind(mh, theDoomed); // Find our cell in the list

    mhSwap(mh, i, mh->currLen - 1); // Swap our cell to the end

    mh->currLen--; // decrement the size

    heapifyDown(mh, i); // restore the heap property
}

// Just peek at the cell in the front of the list w/o removing it
cell *peek(minHeap *mh) {
    return mh->heap[0].data;
}