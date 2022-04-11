#ifndef MINHEAP_H
#define MINHEAP_H

#include <vector>
#include <iostream>

template <class T>
class minHeap {
private:
    class heapNode {
    public:
        T  *data;
        int *value;
        heapNode(T *t, int *val) : data(t), value(val) {}
        heapNode() {}
    };

    std::vector<heapNode> heap;
    int currLen;
    void swap(int i, int j);
    //int find(T *t);

public:
    minHeap();
    ~minHeap();
    void add(T *t, int *val);
    void heapifyDown(int idx);
    void heapifyUp(int idx);

    bool isEmpty() const;
    int len() const;

    T *peek();
    T *extract();

    T *operator[](const int idx);
};

template <class T>
std::ostream &operator<< (std::ostream &o, minHeap<T> mh);

//void mhSwap(minHeap *mh, int idx1, int idx2);
//void heapifyUpCell(minHeap *mh, cell *cell);
//void heapifyUp(minHeap *mh, int srcIdx);
//void heapifyDown(minHeap *mh, int srcIdx);
//void mhAdd(minHeap *mh, cell *cell);
//cell mhExtract(minHeap *mh);
//int mhFind(minHeap *mh, cell *cell);
//void mhResetMap(cell map[21][80]);
//void mhAddAll(minHeap *mh, cell map[21][80]);
//void mhDeleteElement(minHeap *mh, cell *theDoomed);
//cell *peek(minHeap *mh);

#endif
