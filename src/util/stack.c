// Sebastian Carbonero
// © 2021

#include "stack.h"

#include "huffman.h"
#include "node.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Stack struct
//
// top: top of stack
// capacity: stack capacity
// items: Nodes in the stack
//
struct Stack {
    uint32_t top;
    uint32_t capacity;
    Node **items;
};

// Creates stack
//
// capacity: size of array for capacity
//
Stack *stack_create(uint32_t capacity) {
    Stack *s = (Stack *) malloc(sizeof(Stack));
    assert(s); // Checks if malloc was successful
    s->capacity = capacity;
    s->top = 0;
    s->items = (Node **) calloc(capacity, sizeof(Node *));
    assert(s->items); // Checks if calloc was successful
    return s;
}

// Deletes Stack
//
// Used code from Asgn 3 Doc PDF
// for stack_delete
//
// **s: pointer for stack
//
void stack_delete(Stack **s) {
    if (*s) {
        // Free memory from any mem leaks
        for (uint32_t i = 0; i < (*s)->top; i++) {
            delete_tree(&(*s)->items[i]); // Delete each node left in stack
        }
        free((*s)->items);
        free(*s);
        *s = NULL;
    }
    return;
}

// Returns true if stack is empty
//
// *s: reference for stack struct
//
bool stack_empty(Stack *s) {
    return s->top == 0;
}

// Returns true if stack full
//
// *s: reference for stack struct
//
bool stack_full(Stack *s) {
    return s->top == s->capacity;
}

// Returns stack size
//
// *s: reference for stack struct
//
uint32_t stack_size(Stack *s) {
    return s->top;
}

// Returns true if stack push was successful
//
// *s: reference for stack struct
//  *n: pointer to Node
//
bool stack_push(Stack *s, Node *n) {
    if (!stack_full(s)) {
        if (s->items == NULL) { // Checks if stack was initialized
            return false;
        }
        s->items[s->top] = n; // Set n to top of stack
        s->top++;
        return true;
    }
    return false;
}

// Returns true if stack pop was successful
//
// *s: reference for stack struct
// **n: pointer of element x
//
bool stack_pop(Stack *s, Node **n) {
    if (!stack_empty(s)) {
        s->top--;
        *n = s->items[s->top]; // Set pointer of element *n
        return true;
    }
    return false;
}

// Used to print stack
//
// *s: reference for stack
//
void stack_print(Stack *s) {
    for (uint32_t i = 0; i < s->top; i++) {
        node_print(s->items[i]); // Print node
    }
}
