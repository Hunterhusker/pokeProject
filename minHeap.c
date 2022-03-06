#include "minHeap.h"
#include <math.h>
#include <limits.h>

void mhSwap(minHeap_t *mh, int idx1, int idx2)
{
    heapNode_t tmp = mh->heap[idx1];

    mh->heap[idx1] = mh->heap[idx2];
    mh->heap[idx2] = tmp;
}

void heapifyUp(minHeap_t *mh, int srcIdx)
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

void heapifyUpCell(minHeap_t *mh, cell_t *cell)
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

void heapifyDown(minHeap_t *mh, int srcIdx)
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

void mhAdd(minHeap_t *mh, cell_t *cell)
{
    heapNode_t hN;

    hN.value = &cell->dist;
    hN.data = cell;
    hN.data->inHeap = true;

    mh->heap[mh->currLen] = hN;

    heapifyUp(mh, mh->currLen);

    mh->currLen++;
}

cell_t mhExtract(minHeap_t *mh)
{
    heapNode_t tmp = mh->heap[0];

    mh->heap[0] = mh->heap[mh->currLen - 1];

    mh->currLen--;

    heapifyDown(mh, 0); // cry

    return *tmp.data;
}

int mhFind(minHeap_t *mh, cell_t *cell)
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

void mhResetMap(cell_t map[21][80])
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

void mhAddAll(minHeap_t *mh, cell_t map[21][80])
{
    for (int i = 0; i < 21; i++)
    {
        for (int j = 0; j < 80; j++)
        {
            mhAdd(mh, &map[i][j]);
        }
    }
}

void mhDeleteElement(minHeap_t *mh, cell_t *theDoomed)
{
    int i = mhFind(mh, theDoomed); // Find our cell in the list

    mhSwap(mh, i, mh->currLen - 1); // Swap our cell to the end

    mh->currLen--; // decrement the size

    heapifyDown(mh, i); // restore the heap property
}
