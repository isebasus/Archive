// Sebastian Carbonero
// © 2021

#include "huffman.h"

#include "node.h"
#include "pq.h"
#include "stack.h"
#include "variables.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

uint32_t symbols = 0;

// Traverses through Huffman Tree to build codes
//
// *root: root of Huffman Tree
// table: a table of codes corresponding to each symbol in file
// c: code to assign each symbol to
//
static inline void traverse(Node *root, Code table[static ALPHABET], Code c) {
    if (root) {
        uint8_t temp;
        if (root->left == NULL && root->right == NULL) { // If leaf
            table[root->symbol] = c; // Assign code to symbol
        } else {
            // Traverse through left branch
            code_push_bit(&c, 0);
            traverse(root->left, table, c);
            code_pop_bit(&c, &temp);

            // Traverse through right branch
            code_push_bit(&c, 1);
            traverse(root->right, table, c);
            code_pop_bit(&c, &temp);
        }
    }
    return;
}

// Builds huffman tree from infile symbols
//
// hist: Histogram of symbols with values of their frequencies
//
Node *build_tree(uint64_t hist[static ALPHABET]) {
    Node *p;
    Node *left, *right;
    PriorityQueue *pq = pq_create(ALPHABET); // Create pq
    for (uint32_t i = 0; i < ALPHABET; i++) {
        if (hist[i] > 0) { // if a character has a frequency
            Node *n = node_create(i, hist[i]);
            enqueue(pq, n); // Enqueue to pq
            symbols++;
        }
    }
    while (pq_size(pq) != 1) { // Create tree
        // Dequeue left and right Nodes
        dequeue(pq, &left);
        dequeue(pq, &right);
        Node *n = node_join(left, right); // Join nodes
        enqueue(pq, n);
    }
    dequeue(pq, &p); // Dequeue last node
    pq_delete(&pq);
    return p;
}

// Builds codes from Huffman tree
// traverse() is the helper function
//
// *root: root of Huffman Tree
// table: a table of codes corresponding to each symbol in file
//
void build_codes(Node *root, Code table[static ALPHABET]) {
    Code c = code_init();
    traverse(root, table, c);
    return;
}

// Builds huffman tree from infile symbols
//
// nbytes: Number of bytes in huffman tree dump
// tree: Array of characters containig tree dump
//
Node *rebuild_tree(uint16_t nbytes, uint8_t tree[static nbytes]) {
    Node *root;
    Stack *s = stack_create(nbytes);
    for (uint16_t i = 0; i < nbytes; i++) {
        if (tree[i] == 'L') { // 'L' represents a leaf
            // Create leaf nodes
            Node *n = node_create(tree[i + 1], 0);
            stack_push(s, n);
        } else if (tree[i] == 'I') { // 'I' represents a parent node
            // Create parent nodes
            Node *right, *left;
            stack_pop(s, &right);
            stack_pop(s, &left);
            Node *n = node_join(left, right);
            stack_push(s, n);
        }
        // Used to prevent 'I' and 'L' to be mistaken for a leaf or parent node
        if (i != nbytes - 1) {
            if ((tree[i + 1] == 'I' || tree[i + 1] == 'L') && tree[i] == 'L') {
                tree[i + 1] = '\0';
            }
        }
    }
    // Pop root node
    stack_pop(s, &root);
    stack_delete(&s);
    return root;
}

// Deletes Tree
//
// root: root node of tree
//
void delete_tree(Node **root) {
    if (*root) {
        if ((*root)->left) { // Traverse and delete left Nodes
            delete_tree(&(*root)->left);
        }
        if ((*root)->right) { // Traverse and delete right Nodes
            delete_tree(&(*root)->right);
        }
        node_delete(root); // Delete Node
    }
}
