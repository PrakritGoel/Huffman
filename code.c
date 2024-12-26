#include "code.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

// Constructor function. Creates a new code on the stack.
//
// Input parameters: None
// Returns: Code: Returns the initialized Code
Code code_init(void) {
    Code c;

    c.top = 0;
    memset(c.bits, 0, MAX_CODE_SIZE);
    return c;
}

// Return the size of the Code
//
// Input parameters:
// c: Code *: Code whose size needs to be returned
// Returns: uint32_t: Size of the code
uint32_t code_size(Code *c) {
    return c->top;
}

// Check if the code is empty
//
// Input parameters:
// c: Code *: Code to check
// Returns: bool: true if Code is empty, false otherwise
bool code_empty(Code *c) {
    if (c->top == 0) {
        return true;
    }
    return false;
}

// Check if the code is full
//
// Input parameters:
// c: Code *: Code to check
// Returns: bool: true if Code is full, false otherwise
bool code_full(Code *c) {
    if (c->top == ALPHABET) {
        return true;
    }
    return false;
}

// Set a specific bit in the Code to 1
//
// Input parameters
// c: Code *: Code that will be updated
// i: uint32_t: Index that will be updated
// Returns: bool: false if index is out of range, true otherwise
bool code_set_bit(Code *c, uint32_t i) {
    if (i >= ALPHABET) {
        return false;
    }
    // Right shift by 3 is the same as division by 8, and
    // an "AND" with mask 0x7 is the same as remainder with 8.
    // Since we have an array of 32 uint8_t elements, this
    // should set the desired bit to 1.
    c->bits[i >> 3] |= (1 << (i & 0x7));
    return true;
}

// Set a specific bit in the Code to 0
//
// Input parameters
// c: Code *: Code that will be updated
// i: uint32_t: Index that will be updated
// Returns: bool: false if index is out of range, true otherwise
bool code_clr_bit(Code *c, uint32_t i) {
    if (i >= ALPHABET) {
        return false;
    }

    // Similar logic as for code_set_bit. This time we need to use
    // bitwise AND though.
    c->bits[i >> 3] &= ~(1 << (i & 0x7));
    return true;
}

// Get the bit at the specified index in the given Code
//
// Input parameters
// c: Code *: Code that will be read
// i: uint32_t: Index to read
// Returns: bool: true if bit i is equal to 1, false otherwise
bool code_get_bit(Code *c, uint32_t i) {
    if (i >= ALPHABET) {
        return false;
    }
    if ((c->bits[i >> 3] & (0x1 << (i & 0x7))) == 0) {
        return false;
    }
    return true;
}

// Push a bit onto the Code
//
// Input parameters:
// c: Code *: Code to push the bit to
// bit: uint8_t *: bit to push
// Returns: bool: false if the Code is full prior to pushing, true otherwise
bool code_push_bit(Code *c, uint8_t bit) {
    if (code_full(c)) {
        return false;
    }

    if (bit == 0) {
        code_clr_bit(c, c->top);
    } else {
        code_set_bit(c, c->top);
    }
    c->top += 1;
    return true;
}

// Pop a bit off the Code
//
// Input parameters:
// c: Code *: Code to pop the bit from
// bit: uint8_t *: Popped bit
// Returns: bool: false if the Code is empty prior to popping, true otherwise
bool code_pop_bit(Code *c, uint8_t *bit) {
    if (code_empty(c)) {
        return false;
    }
    if (code_get_bit(c, c->top) == false) {
        *bit = 0;
    } else {
        *bit = 1;
    }
    c->top -= 1;
    return true;
}

// Debug function to verify if bits are pushed onto and popped
// off a Code correctly.
//
// Input parameters:
// c: Code *: Code to be printed.
// Returns: void
void code_print(Code *c) {
    for (uint32_t i = 0; i < c->top; i++) {
        printf("Bit at position %d = %u\n", i, c->bits[i]);
    }
    return;
}
