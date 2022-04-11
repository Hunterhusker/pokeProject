//
// Created by hunter on 4/10/22.
//
#include "minHeap.h"
#include <iostream>

typedef struct thing {
    int val;
} thing_t;

int main() {
    minHeap<int> mh;
    int a = 5, b = 10, c = 15, d = 20, e = 25, f = 1, g = -10;

    mh.add(&d, &d);
    mh.add(&a, &a);
    mh.add(&e, &e);
    mh.add(&b, &b);
    mh.add(&c, &c);
    mh.add(&g, &g);
    mh.add(&f, &f);

    std::cout << mh << std::endl;

    while (!mh.isEmpty()) {
        std::cout << *mh.extract() << std::endl;
    }
}
