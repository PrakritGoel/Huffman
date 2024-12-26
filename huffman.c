#include "huffman.h"
#include "code.h"
#include "pq.h"
#include "stack.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void print_tree(Node *node);
extern Code c;

// Constructs a Huffman tree given a computed histogram.
//
// Input parameters
// hist: uint64_t[]: Histogram of size ALPHABET to be used for the tree
// Returns: Node *: Pointer to the root node of the tree.
Node *build_tree(uint64_t hist[static ALPHABET]) {
    Node *node, *left, *right;
    PriorityQueue *pq;
    uint32_t capacity = 0;

    for (uint32_t i = 0; i < ALPHABET; i++) {
        if (hist[i] != 0) {
            capacity++;
        }
    }
    pq = pq_create(capacity);

    // First, create nodes for all entries in the histogram,
    // and add them to the priority queue.
    for (uint16_t i = 0; i < ALPHABET; i++) {
        if (hist[i] != 0) {
            node = node_create(i, hist[i]);
            enqueue(pq, node);
        }
    }

    // While there are >=2 nodes in the queue, dequeue two nodes,
    // join them, and add the resutant node to the queue.
    while (pq_size(pq) >= 2) {
        if ((dequeue(pq, &left) == true) && (dequeue(pq, &right) == true)) {
            node = node_join(left, right);
            enqueue(pq, node);
        }
    }
    dequeue(pq, &node);
    pq_delete(&pq);
    return node;
}

// Populates a code table, building the code for each symbol in the
// Huffman tree.
//
// Input parameters:
// root: Node *: Root node of the Huffman tree.
// table: Code []: Code table
// Returns: void
void build_codes(Node *root, Code table[static ALPHABET]) {
    uint8_t popped_bit;

    if (root != NULL) {
        // If leaf node, add symbol to the code table
        if (root->left == NULL && root->right == NULL) {
            table[root->symbol] = c;
        } else {
            code_push_bit(&c, 0);
            build_codes(root->left, table);
            code_pop_bit(&c, &popped_bit);

            code_push_bit(&c, 1);
            build_codes(root->right, table);
            code_pop_bit(&c, &popped_bit);
        }
    }
    return;
}

// Dumps the contents of the tree into a file. Leaf nodes are represented
// with the symbol L, followed by the symbol itself, and the interior
// nodes are represented with the symbol I.
//
// Input parameters:
// outfile: int: File descriptor of the output file
// root: Node *: Root node of the Huffman tree.
// Returns: void
void dump_tree(int outfile, Node *root) {
    if (root != NULL) {
        dump_tree(outfile, root->left);
        dump_tree(outfile, root->right);

        // If leaf node
        if (root->left == NULL && root->right == NULL) {
            write(outfile, "L", 1);
            write(outfile, &(root->symbol), 1);
        } else {
            write(outfile, "I", 1);
        }
    }
    return;
}

// Rebuilds the tree from the data read from the encoded file. Any time
// 'L' is encountered, the next byte is a leaf node symbol. 'I' represents
// an interior node, that joins two leaf nodes.
//
// Input parameters:
// nbytes: uint16_t: Buffer size
// tree: uint8_t []: Buffer to build the tree
// Returns: Node *: Pointer to the root of the node
Node *rebuild_tree(uint16_t nbytes, uint8_t tree[static nbytes]) {
    Node *n;
    Node *left_child, *right_child;
    Stack *s = stack_create(MAX_TREE_SIZE);

    for (uint16_t i = 0; i < nbytes; i++) {
        if (tree[i] == 'L') { // Leaf node
            i += 1;
            n = node_create(tree[i], 0);
            stack_push(s, n);
        } else if (tree[i] == 'I') { // Interior node
            stack_pop(s, &right_child);
            stack_pop(s, &left_child);
            n = node_join(left_child, right_child);
            stack_push(s, n);
        }
    }
    stack_pop(s, &n);
    stack_delete(&s);
    // print_tree(n);
    return n;
}

// Delete the tree to free memory using postorder traversal
//
// Input parameters:
// root: Node **: Root * for the tree to be deleted
// Returns: void
void delete_tree(Node **root) {
    if (*root == NULL) {
        return;
    }

    if ((*root)->left != NULL) {
        delete_tree(&((*root)->left));
    }
    if ((*root)->right != NULL) {
        delete_tree(&((*root)->right));
    }
    node_delete(root);

    return;
}

// Debug function to print the tree. I used it to debug some issues I
// was getting during decode.
//
// Input parameters:
// root: Node **: Root * for the tree to be deleted
// Returns: void
void print_tree(Node *node) {
    if (node == NULL) {
        return;
    }
    if (node->left != NULL) {
        print_tree(node->left);
    }
    if (node->right != NULL) {
        print_tree(node->right);
    }
    printf("%c\n", node->symbol);
    return;
}
