// Sebastian Carbonero
// © 2021

#include "code.h"

#include "defines.h"

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Initializes a Code
Code code_init(void) {
    Code c;
    c.top = 0;
    // Sets each item in code to 0
    for (uint32_t i = 0; i < MAX_CODE_SIZE; i++) {
        c.bits[i] = 0;
    }
    return c;
}

// Returns code size
//
// *c: pointer to Code struct
//
uint32_t code_size(Code *c) {
    return c->top;
}

// Returns true if code empty
//
// *c: pointer to Code struct
//
bool code_empty(Code *c) {
    return c->top == 0;
}

// Returns true if code full
//
// *c: pointer to Code struct
//
bool code_full(Code *c) {
    return c->top == MAX_CODE_SIZE;
}

// Returns true if Code push was successful
//
// *c: pointer to Code struct
// bit: bit to push
//
bool code_push_bit(Code *c, uint8_t bit) {
    if (code_full(c)) {
        return false;
    }
    c->bits[c->top / 8] |= (bit << (c->top % 8)); // Set bit
    c->top++;
    return true;
}

// Returns true if Code pop was successful
//
// *c: pointer to Code struct
// *bit: bit to return popped value t
//
bool code_pop_bit(Code *c, uint8_t *bit) {
    if (code_empty(c)) {
        return false;
    }
    c->top--;
    *bit = (c->bits[c->top / 8] >> (c->top % 8) & 0x1); // Get bit
    c->bits[c->top / 8] &= ~(0x1 << (c->top % 8)); // Clear bit
    return true;
}

// Prints Code
//
// *c: pointer to Code
//
void code_print(Code *c) {
    for (uint32_t i = 0; i < c->top; i++) {
        printf("%d", (c->bits[i / 8] >> (i % 8) & 0x1));
    }
    printf("\n");
    return;
}
