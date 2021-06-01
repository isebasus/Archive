// Sebastian Carbonero
// © 2021

#include "pq.h"

#include "huffman.h"
#include "node.h"

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// PriorityQueue struct
//
// size: size of priority queue
// capacity: queue capacity
// **node: nodes in the queue
//
struct PriorityQueue {
    uint32_t size;
    uint32_t capacity;
    Node **node;
};

// Swaps two indexes of an array
//
// *q: priority queue pointer
//  i: index i to swap
//  j: index j to swap
//
static inline void swap(PriorityQueue *q, uint32_t i, uint32_t j) {
    Node *temp = q->node[i];
    q->node[i] = q->node[j];
    q->node[j] = temp;
    return;
}

// Gets the parent of the index
//
//  i: index to locate parent
//
static inline uint32_t parent(uint32_t i) {
    return ((i - 1) / 2);
}

// Gets the parent of the index
//
//  i: index to locate right child with
//
static inline uint32_t right(uint32_t i) {
    return ((2 * i) + 2);
}

// Gets the parent of the index
//
//  i: index to locate left child with
//
static inline uint32_t left(uint32_t i) {
    return ((2 * i) + 1);
}

// Sorts queue from smallest to biggest with heap sort
//
// Inspired code from 6.4 of Introduction to
// Algorithms: Heapsort pseudocode
//
// *p: pointer to priority queue
//  pos: dequeued position
//
static inline void heapify(PriorityQueue *q, uint32_t pos) {
    uint32_t l = left(pos); // get left child
    uint32_t r = right(pos); // get right child
    uint32_t smallest = pos;
    // Check if smallest is greater than left child and right child
    if (l < q->size && q->node[smallest]->frequency > q->node[l]->frequency) {
        smallest = l;
    }
    if (r < q->size && q->node[smallest]->frequency > q->node[r]->frequency) {
        smallest = r;
    }
    // heapify more if queue is not sorted
    if (pos != smallest) {
        swap(q, smallest, pos);
        heapify(q, smallest);
    }
    return;
}

// Creates PriorityQueue
//
// capacity: size of array for capacity
//
PriorityQueue *pq_create(uint32_t capacity) {
    PriorityQueue *q = (PriorityQueue *) malloc(sizeof(PriorityQueue));
    assert(q); // Checks if malloc was successful
    q->size = 0;
    q->capacity = capacity;
    q->node = (Node **) calloc(capacity, sizeof(Node *));
    assert(q->node); // Checks if calloc was successful
    return q;
}

// Destructs priority queue
//
// **q: pointer for pq
//
void pq_delete(PriorityQueue **q) {
    if (*q) {
        for (uint32_t i = 0; i < (*q)->size; i++) {
            // delete each node left over
            delete_tree(&(*q)->node[i]);
        }
        // free memory
        free((*q)->node);
        free(*q);
        *q = NULL;
    }
    return;
}

// Returns true if pq is empty
//
// *q: reference for pq struct
//
bool pq_empty(PriorityQueue *q) {
    return q->size <= 0;
}

// Returns true if pq full
//
// *q: pointer for pq struct
//
bool pq_full(PriorityQueue *q) {
    return q->size == q->capacity;
}

// Returns pq size
//
// *q: pointer for pq struct
//
uint32_t pq_size(PriorityQueue *q) {
    return q->size;
}

// Returns true if enqueue was successful
//
// Inspired code from 6.4 of Introduction to
// Algorithms: Heapsort enqueue
//
// *n: Node pointer
// *q: Queue pointer
//
bool enqueue(PriorityQueue *q, Node *n) {
    if (q) {
        if (pq_full(q)) {
            return false;
        }
        q->size++;
        uint32_t t = q->size - 1;
        q->node[t] = n;
        // Creates children and parents within the pq array
        while (t != 0 && q->node[parent(t)]->frequency > q->node[t]->frequency) {
            swap(q, t, parent(t));
            t = parent(t);
        }
        return true;
    }
    return false;
}

// Returns true if dequeue was successful
//
// Inspired code from 6.4 of Introduction to
// Algorithms: Heapsort dequeue
//
// **n: Node pointer
// *q: Queue pointer
//
bool dequeue(PriorityQueue *q, Node **n) {
    if (q) {
        if (pq_empty(q)) {
            return false;
        }
        if (q->size == 1) {
            // index cannot be negative
            q->size--;
            *n = q->node[0];
            return true;
        }
        // set *n to the item with the lowest frequency
        *n = q->node[0];
        q->node[0] = q->node[q->size - 1];
        q->size--;
        heapify(q, 0); // sort with heapify
        return true;
    }
    return false;
}

// Used to print PriorityQueue
//
// *q: pointer for pq
//
void pq_print(PriorityQueue *q) {
    for (uint32_t i = 0; i < q->size / 2; i++) {
        printf("Parent: %" PRId64 "", q->node[i]->frequency); // Print parent
        printf(" Left Child: %" PRId64 "", q->node[left(i)]->frequency); // Print left child
        printf(" Right Child: %" PRId64 "\n", q->node[right(i)]->frequency); // Print right child
    }
}
