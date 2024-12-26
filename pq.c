#include "pq.h"
#include "huffman.h"
#include "node.h"

#include <stdio.h>
#include <stdlib.h>

struct PriorityQueue {
    Node **nodes;
    uint32_t size;
    uint32_t capacity;
};

// Constructor function for the priority queue
//
// Input parameters:
// capacity: uint32_t: Maximum capacity of the queue.
// Returns: PriorityQueue *: Pointer to the pq created
PriorityQueue *pq_create(uint32_t capacity) {
    PriorityQueue *q = (PriorityQueue *) calloc(1, sizeof(PriorityQueue));
    q->nodes = (Node **) calloc(capacity, sizeof(Node *));
    q->size = 0;
    q->capacity = capacity;

    return q;
}

// Destructor function for the priority queue
//
// Input parameters:
// q: PriorityQueue **: Pointer to the pq to be deleted
// Returns: void
void pq_delete(PriorityQueue **q) {
    free((*q)->nodes);
    free(*q);
    *q = NULL;
    return;
}

// Checks if the queue is empty or not
//
// Input parameters:
// q: PriorityQueue *: Ptr to the queue to check
// Returns: bool: true if the queue is empty. False otherwise
bool pq_empty(PriorityQueue *q) {
    if (q->size == 0) {
        return true;
    }
    return false;
}

// Checks if the queue is full or not
//
// Input parameters:
// q: PriorityQueue *: Ptr to the queue to check
// Returns: bool: true if the queue is full. False otherwise
bool pq_full(PriorityQueue *q) {
    if (q->size == q->capacity) {
        return true;
    }
    return false;
}

// Returns the current size of the queue
//
// Input parameters:
// q: PriorityQueue *: Ptr to the queue to check
// Returns: uint32_t: Current size of the queue
uint32_t pq_size(PriorityQueue *q) {
    return q->size;
}

// Enqueues a node in the pq
//
// Input parameters:
// q: PriorityQueue *: Ptr to the queue to be updated
// n: Node *: Ptr to the node to be added
// Returns: bool: false if the queue is full before addition. True otherwise
bool enqueue(PriorityQueue *q, Node *n) {
    // We use insertion sort to enqueue the new node.
    uint32_t i;

    if (pq_full(q)) {
        return false;
    }

    i = q->size;

    // Identify the position to insert the node, and create space for it.
    while (i > 0 && q->nodes[i - 1]->frequency < n->frequency) {
        q->nodes[i] = q->nodes[i - 1];
        i -= 1;
    }
    q->nodes[i] = n;
    q->size += 1;
    return true;
}

// Dequeues a node from the pq. The node dequeued has the highest priority
// of all the nodes in the pq.
//
// Input parameters:
// q: PriorityQueue *: Ptr to the queue to be updated
// n: Node **: Will contain the ptr to the node that's dequeued.
// Returns: bool: false if the queue is empty before dequeue. True otherwise
bool dequeue(PriorityQueue *q, Node **n) {
    if (pq_empty(q)) {
        return false;
    }

    // Dequeue is always done from the end, since enqueue ensures that
    // the node of highest priority will be at the end.
    uint32_t size = pq_size(q);
    *n = q->nodes[size - 1];
    q->nodes[size - 1] = NULL;
    q->size -= 1;
    return true;
}

// Debug function to print the pq.
//
// Input parameters:
// q: PriorityQueue *: Ptr to the queue to be printed
// Returns: void
void pq_print(PriorityQueue *q) {
    for (uint32_t i = 0; i < q->size; i++) {
        printf("%u\t%ld\n", q->nodes[i]->symbol, q->nodes[i]->frequency);
    }
    return;
}
