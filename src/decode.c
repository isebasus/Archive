// Sebastian Carbonero
// © 2021

#include "util/code.h"
#include "util/defines.h"
#include "util/header.h"
#include "util/huffman.h"
#include "io/io.h"
#include "util/pq.h"
#include "util/variables.h"

#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define OPTIONS "hvi:o:"

// Reads header of encoded file
//
// infile: file to read from
// *h: header struct
//
static inline void read_header(int infile, Header *h) {
    read_bytes(infile, (uint8_t *) h, sizeof(Header));
    return;
}

// Set permissions of outfile
//
// outfile: file to set permissions to
// *h: header struct
//
static inline void set_permissions(int outfile, Header *h) {
    fchmod(outfile, h->permissions);
    return;
}

// Read tree from infile
//
// *root: root of Huffman tree
// infile: infile to read from
// outfile: file to write decoded messages to
// *h: header containing decompressed file size
//
static inline void read_tree(Node *root, int infile, int outfile, Header *h) {
    Node *n = root;
    uint64_t i = 0;
    uint8_t buf[BLOCK];
    while (i < h->file_size) {
        uint8_t bit;
        // If leaf node, set symbol to index in buffer
        if (n->left == NULL && n->right == NULL) {
            buf[i % BLOCK] = n->symbol; // Set symbol
            n = root; // Reset to Root
            i++;
            if (i % BLOCK == 0 && i != 0) { // Write bytes if buffer is full
                write_bytes(outfile, buf, sizeof(buf));
            }
        }
        read_bit(infile, &bit); // Read bit
        if (bit == 0 && n->left != NULL) {
            n = n->left; // Traverse left if bit is 0
        }
        if (bit == 1 && n->right != NULL) {
            n = n->right; // Traverse right if bit is 1
        }
    }
    if (i % BLOCK != 0 && i != 0) { // Write out left over bytes
        write_bytes(outfile, buf, (i % BLOCK));
    }
    delete_tree(&root);
    return;
}

// Reads and decodes file
//
// infile: file to decode
// outfile: file to write to
//
uint8_t read_file(int infile, int outfile, bool verbose) {
    Header h;
    read_header(infile, &h);
    if (h.magic != MAGIC) { // Check for 0xDEADBEEF
        fprintf(stderr, "Error: invalid MAGIC number.\n");
        return 1;
    }
    uint8_t tree[h.tree_size];
    set_permissions(outfile, &h);
    read_bytes(infile, tree, h.tree_size); // Read in dumped tree

    Node *root = rebuild_tree(h.tree_size, tree); // Reconstruct Huffman Tree
    read_tree(root, infile, outfile, &h); // Decode the tree
    if (verbose) {
        double stat = 100 * (1 - ((double) total_bytes_read / (double) h.file_size));
        fprintf(stderr, "Compressed file size: %" PRId64 " bytes\n", total_bytes_read);
        fprintf(stderr, "Decompressed file size: %" PRId64 " bytes\n", h.file_size);
        fprintf(stderr, "Space saving: %0.2lf%c\n", stat, '%');
    }
    close(infile);
    close(outfile);
    return 0;
}

// Prints out help guide
//
// This function is used when there are
// no user inputs
//
// Also used when there is an unknown argument
//
static inline void no_argument() { // Used to print out options
    fprintf(stderr, "SYNOPSIS\n   A Huffman decoder.\n"
                    "   Decompresses a file using the Huffman coding algorithm."
                    "\n\nUSAGE\n     ./decode [-h] [-i infile] [-o outfile]"
                    "\n\nOPTIONS\n  -h             Program usage and help.\n"
                    "  -v             Print compression statistics.\n"
                    "  -i infile      Input data to decompress.\n"
                    "  -o outfile     Output of decompressed data.\n");
    return;
}

int main(int argc, char *argv[]) {
    int infile = STDIN_FILENO, outfile = STDOUT_FILENO; // Files to read and write to
    bool verbose = false;
    int opt = 0;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': no_argument(); return 1; // print out help options
        case 'v': verbose = true; break;
        case 'i':
            // Open file to read from
            if ((infile = open(optarg, O_RDONLY | O_CREAT)) == -1) { // if open fail print error
                fprintf(stderr, "Error: failed to open infile.\n");
                return 1;
            }
            break;
        case 'o':
            // Open file to write to
            if ((outfile = open(optarg, O_WRONLY | O_CREAT)) == -1) { // if write fail print error
                fprintf(stderr, "Error: Unable to write file\n");
                return 1;
            }
            break;
        case '?': no_argument(); return 1; // Unknown argument
        default: abort(); return 1;
        }
    }
    return read_file(infile, outfile, verbose);
}
