// Sebastian Carbonero
// © 2021

#include "node.h"

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Constructor function for Node
//
// symbol: symbol of a character
// frequency: frequency of a character
//
Node *node_create(uint8_t symbol, uint64_t frequency) {
    Node *n = (Node *) malloc(sizeof(Node));
    assert(n); // Check if malloc was successful
    n->left = NULL;
    n->right = NULL;
    n->symbol = symbol;
    n->frequency = frequency;
    return n;
}

// Destructor function for Node
//
// **n: node to delete
//
void node_delete(Node **n) {
    if (*n) {
        // Free Memory
        free(*n);
        *n = NULL;
    }
    return;
}

// Return a new Node after joining two nodes together
//
// *left: left child for new parent Node
// *right: right child for new parent Node
//
Node *node_join(Node *left, Node *right) {
    // Add left and right frequencies together
    Node *n = node_create('$', left->frequency + right->frequency);
    n->left = left;
    n->right = right;
    return n;
}

// Used to print Node
//
// *n: Node to print
//
void node_print(Node *n) {
    printf("Parent Node Frequency: %" PRId64 " Parent node symbol: %d\n", n->frequency, n->symbol);
    if (n->left && n->right) {
        printf("Left child frequency '%c': %" PRId64 " Right child frequency '%c': %" PRId64 "\n",
            n->left->symbol, n->left->frequency, n->right->symbol, n->right->frequency);
    }
    return;
}
