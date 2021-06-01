// Sebastian Carbonero
// © 2021

#include "util/code.h"
#include "util/defines.h"
#include "util/header.h"
#include "util/huffman.h"
#include "io/io.h"
#include "util/node.h"
#include "util/pq.h"
#include "util/variables.h"

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define OPTIONS "hvi:o:"

struct stat statbuf;

static inline void write_header(int outfile, Header *h) {
    write_bytes(outfile, (uint8_t *) h, sizeof(Header));
}

// Helper function used to write statistics
//
// infile: get file statistics
// header: header for statistics
//
static inline void write_stats(int infile, Header header) {
    fstat(infile, &statbuf);
    double stat
        = 100 * (1 - (((double) bytes_written - header.file_size) / (double) header.file_size));
    fprintf(stderr, "Uncompressed file size: %" PRId64 " bytes\n", header.file_size);
    fprintf(stderr, "Compressed file size: %" PRId64 " bytes\n", bytes_written - header.file_size);
    fprintf(stderr, "Space saving: %0.2lf%c\n", stat, '%');
}

// Seek either tmp file or infile if not STDIN
//
// infile: get file statistics
// tmp: temp file
//
static inline int seek(int tmp, int infile) {
    int file;
    if (infile == STDIN_FILENO && tmp != 0) {
        lseek(tmp, 0, SEEK_SET); // Set fp to 0
        file = tmp;
    } else {
        lseek(infile, 0, SEEK_SET);
        file = infile;
    }
    return file;
}

// Construct Header
//
// infile: get file statistics
// outfile: file to set permissions to
//
static inline Header create_header(int infile, int outfile) {
    Header header;
    fstat(infile, &statbuf);
    fchmod(outfile, statbuf.st_mode);
    header.magic = MAGIC;
    header.permissions = statbuf.st_mode;
    header.tree_size = (3 * symbols) - 1; // symbols is from variables.h
    header.file_size = statbuf.st_size;
    return header;
}

// Traverses through Huffman Tree to print out codes
//
// *root: root of Huffman Tree
// outfile: file to write to
//
static inline void traverse(Node *root, int outfile) {
    if (root) {
        uint8_t l = 'L';
        uint8_t i = 'I';
        // Check for leaf Nodes
        if (root->left == NULL && root->right == NULL) {
            write_bytes(outfile, &l, 1); // Write out leaf node symbol 'L'
            write_bytes(outfile, &root->symbol, 1);
        } else { // Traverse through left and right nodes
            traverse(root->left, outfile);
            traverse(root->right, outfile);
            write_bytes(outfile, &i, 1); // Write out parent node symbol 'I'
        }
    }
}

// Read infile
//
// infile: infile to read from
// *hist: histogram to build
// fno: if there is a tempfile, write to it
//
static inline void read_file(int infile, uint64_t *hist, int fno) {
    uint8_t buf[BLOCK];
    int status = 0;
    do {
        status = read_bytes(infile, buf, sizeof(buf));
        for (uint64_t i = 0; i < bytes_read; i++) {
            hist[buf[i]] += 1; // Add to histogram
        }
        if (infile == STDIN_FILENO && fno != 0) {
            write_bytes(fno, buf, bytes_read);
        }
    } while (status != 0);
    return;
}

// Reads and decodes file
//
// infile: file to decode
// outfile: file to write to
//
uint8_t huffman(int infile, int outfile, bool verbose) {
    uint64_t hist[ALPHABET] = { 0 };
    hist[0]++;
    hist[255]++;

    int fno = 0;
    off_t offset;
    char file_name[255];
    if (offset = lseek(infile, 0, SEEK_SET), offset == -1) { // Create temp file
        time_t t;
        time(&t);
        sprintf(file_name, "/tmp/huffman.%s", ctime(&t)); // set file name with time
        fno = open(file_name, O_RDWR | O_CREAT | O_EXCL | O_TRUNC, 0600);
    }
    read_file(infile, hist, fno);

    // Build tree
    Node *root = build_tree(hist);
    Code table[ALPHABET] = { 0 };
    build_codes(root, table);

    int file = seek(fno, infile);

    // Create header and write header, then dump Huffman Tree
    Header header = create_header(file, outfile);
    write_header(outfile, &header);
    traverse(root, outfile); // Dump tree

    uint8_t buf[BLOCK];
    int status = 0;
    do { // Read from file again
        status = read_bytes(file, buf, sizeof(buf));
        for (uint64_t i = 0; i < bytes_read; i++) {
            Code code = table[buf[i]];
            write_code(outfile, &code);
        }
        memset(buf, 0, sizeof(buf)); // Clear buffer
    } while (status != 0);

    flush_codes(outfile);

    if (verbose) { // Print out statistics if verbose
        write_stats(infile, header);
    }

    if (infile == STDIN_FILENO && fno != 0) {
        unlink(file_name);
        close(fno);
    }
    // Free allocated memory
    delete_tree(&root);
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
    fprintf(stderr, "SYNOPSIS\n   A Huffman encoder.\n"
                    "   Compresses a file using the Huffman coding algorithm."
                    "\n\nUSAGE\n     ./encode [-h] [-i infile] [-o outfile]"
                    "\n\nOPTIONS\n  -h             Program usage and help.\n"
                    "  -v             Print compression statistics.\n"
                    "  -i infile      Input data to encode.\n"
                    "  -o outfile     Output of encoded data.\n");
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
    return huffman(infile, outfile, verbose);
}
