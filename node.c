#include "node.h"

#include <stdio.h>
#include <stdlib.h>

// Node constructor function. Allocates memory for a node, and sets the
// value of the symbol and frequency. The left and right children are
// set to NULL, to indicate a leaf node.
//
// Input parameters:
// symbol: uint8_t: Set the symbol field of the node to this value
// frequency: uint64_t: Set the frequency field of the node to this value
// Returns: Node *: Pointer to the node that's created
Node *node_create(uint8_t symbol, uint64_t frequency) {
    Node *node = (Node *) calloc(1, sizeof(Node));

    node->symbol = symbol;
    node->frequency = frequency;
    node->left = NULL;
    node->right = NULL;

    return node;
}

// Node destructor function. Frees memory, and sets the pointer to null.
//
// Input parameters:
// n: Node **: Ptr to pointer to the node to be destroyed
// Returns: void
void node_delete(Node **n) {
    // printf("Inside node delete and node symbol = %c\n", (*n)->symbol);
    free(*n);
    *n = NULL;
    return;
}

// Create a parent node, assigning the two input nodes as its children
//
// Input parameters:
// left: Node *: Node that will be the left child
// right: Node *: Node that will be the right child
// Returns: Node *: Pointer to the parent node
Node *node_join(Node *left, Node *right) {
    Node *parent = (Node *) calloc(1, sizeof(Node));

    parent->left = left;
    parent->right = right;
    parent->symbol = '$';
    parent->frequency = left->frequency + right->frequency;

    return parent;
}

// Debug function to verify that the nodes are created and joined correctly
//
// Input parameters:
// n: Node *: Pointer to the node to print/verify
// Returns: void
void node_print(Node *n) {
    printf("%ld\n", n->frequency);
    return;
}
