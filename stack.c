#include "stack.h"

#include <stdio.h>
#include <stdlib.h>

struct Stack {
    uint32_t top;
    uint32_t capacity;
    Node **items;
};

// Stack constructor. The capacity is the maximum number of nodes
// that the stack can hold.
//
// Input parameters:
// capacity: uint32_t: Max number of nodes in the stack
// Returns: Stack *: Pointer to the stack created
Stack *stack_create(uint32_t capacity) {
    Stack *s = (Stack *) calloc(1, sizeof(Stack));
    s->items = (Node **) calloc(capacity, sizeof(Node *));
    s->top = 0;
    return s;
}

// Stack destructor.
//
// Input parameters:
// s: Stack **: Stack to be deleted
// Returns: void
void stack_delete(Stack **s) {
    for (uint32_t i = 0; i < (*s)->top; i++) {
        free((*s)->items[i]);
    }
    free((*s)->items);
    free(*s);
    *s = NULL;
    return;
}

// Checks if the stack is empty.
//
// Input parameters:
// s: Stack *: Stack to be checked
// Returns: bool: true if the stack is empty. False otherwise
bool stack_empty(Stack *s) {
    if (s->top == 0) {
        return true;
    }
    return false;
}

// Checks if the stack is full.
//
// Input parameters:
// s: Stack *: Stack to be checked
// Returns: bool: true if the stack is full. False otherwise
bool stack_full(Stack *s) {
    if (s->top == s->capacity - 1) {
        return true;
    }
    return false;
}

// Return the size of the stack
//
// Input parameters:
// s: Stack *: Stack whose size is to be returned
// Returns: uint32_t: Size of the stack
uint32_t stack_size(Stack *s) {
    return (s->top);
}

// Push a node onto a stack.
//
// Input parameters:
// s: Stack *: Stack to which the node must be pushed.
// n: Node *: Node to be pushed
// Returns: bool: false if the stack is full before push, true otherwise
bool stack_push(Stack *s, Node *n) {
    if (stack_full(s)) {
        return false;
    }

    s->items[s->top] = n;
    s->top += 1;
    return true;
}

// Pop a node from the stack.
//
// Input parameters:
// s: Stack *: Stack from which the node must be popped..
// n: Node **: Node that's popped
// Returns: bool: false if the stack is empty before push, true otherwise
bool stack_pop(Stack *s, Node **n) {
    if (stack_empty(s)) {
        return false;
    }
    s->top -= 1;
    *n = s->items[s->top];
    return true;
}

// Debug function to print the contents of a stack.
//
// Input parameters:
// s: Stack *: Stack to be printed.
// Returns: void
void stack_print(Stack *s) {
    if (s == NULL) {
        return;
    }
    for (uint32_t i = 0; i < s->top; i++) {
        printf("%u\n", (s->items[i])->symbol);
    }
    return;
}
