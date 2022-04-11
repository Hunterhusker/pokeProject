#include <cmath>
#include "minHeap.h"

/// The constructor
template <class T>
minHeap<T>::minHeap() {
    this->currLen = 0;
}

/// Destructor: deletes each heapNode, since they're dynamically allocated w/ new
template <class T>
minHeap<T>::~minHeap() {
//    for (int i = 0; i < this->currLen; i++) {
//        delete this->heap[i];
//    }
}

/// Adds a generic item to the heap, and maintains the heap property
template <class T>
void minHeap<T>::add(T *t, int *val) {
    heapNode temp;

    temp.value = val;
    temp.data = t;

    this->heap.push_back(temp); // add it to the heap on the end
    this->currLen++; // make sure we know we have more elements

    heapifyUp(this->currLen - 1); // restore the heap property (-1 since arrays start at 0)
}

/// Returns a bool that is true only when the heap has a length of 0
template <class T>
bool minHeap<T>::isEmpty() const {
    return currLen == 0;
}

/// Returns the current amount of items in the heap
template <class T>
int minHeap<T>::len() const {
    return currLen;
}

/// Returns a pointer to the thing on the head of the heap
template <class T>
T *minHeap<T>::peek() {
    return &this->heap[0];
}

/// Returns a pointer to the element that was at the head of the heap, and removes it from the head
template <class T>
T *minHeap<T>::extract() {
    T *tmp = this->heap[0].data; // grab the value to return
    this->currLen--; // decrement the size, since we have one less element

    this->heap[0] = this->heap[currLen]; // move the tail to the head
    this->heap.pop_back();

    heapifyDown(0); // heapify this down

    return tmp; // actually return the value we saved earlier
}

/// Method to swap two nodes in the heap
template <class T>
void minHeap<T>::swap(int i, int j) {
    heapNode tmp = this->heap[i];

    this->heap[i] = this->heap[j];
    this->heap[j] = tmp;
}

/// A function to maintain the heap param from bottom up
template <class T>
void minHeap<T>::heapifyUp(int idx) {
    if (idx < 0 || idx > this->currLen) {
        std::cerr << "Heap out of bounds: \"" << idx << "\" is outside of the given heap!" << std::endl;
    }

    if (*this->heap[idx].value < *this->heap[(int) floor((idx - 1) / 2)].value) {
        swap(idx, (int) floor((idx - 1) / 2)); // swap the two

        heapifyUp((int) floor((idx - 1) / 2)); // heapify the node further up, just in case
    }
}

/// A heapify down function to maintain the heap param from top down
template <class T>
void minHeap<T>::heapifyDown(int idx) {
    if ((idx * 2) + 1 < this->currLen && *this->heap[idx].value > *this->heap[(idx * 2) + 1].value) {
        swap((idx * 2) + 1, idx);

        heapifyDown((idx * 2) + 1);
    }

    if ((idx * 2) + 2 < this->currLen && *this->heap[idx].value > *this->heap[(idx * 2) + 2].value) {
        swap((idx * 2) + 2, idx);

        heapifyDown((idx * 2) + 2);
    }
}

template <class T>
T *minHeap<T>::operator[](const int idx) {
    return this->heap[idx].value;
}

/// Prints out the entire minHeap w/ std::cout
template <class T>
std::ostream &operator<< (std::ostream &o, minHeap<T> mh) {
    for (int i = 0; i < mh.len(); i++) {
        o << *mh[i] << ", ";
    }

    return o;
}