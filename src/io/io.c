// Sebastian Carbonero
// © 2021

#include "io.h"

#include "../util/variables.h"

#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

uint8_t buffer[BLOCK];
uint64_t bits_read = 0, bits_added = 0;
uint64_t bytes_read = 0, bytes_written = 0;
uint64_t total_bytes_read = 0;

// Reads bytes of infile
//
// infile: file to read
// *buf: array to read characters into
// nbytes: number of bytes to read
//
int read_bytes(int infile, uint8_t *buf, int nbytes) {
    int rb = 0;
    bytes_read = 0;
    for (int i = 0; i < nbytes; i++) { // Read until nbytes
        // Check read for EOF
        if (rb = read(infile, buf + bytes_read, nbytes - bytes_read), rb > 0) {
            bytes_read += rb;
            total_bytes_read += rb;
            if ((int)bytes_read == nbytes) { // Max bytes to read
                break;
            }
        } else {
            break;
        }
    }
    return rb;
}

// Writes bytes to outfile
//
// outfile: file to write to
// *buf: array of characters to write
// nbytes: number of bytes to write
//
int write_bytes(int outfile, uint8_t *buf, int nbytes) {
    int wb = 0;
    int bytes_w = 0;
    for (int i = 0; i < nbytes; i++) { // Read until nbytes
        if (wb = write(outfile, buf + bytes_w, nbytes - bytes_w), wb > 0) {
            bytes_w += wb;
            bytes_written += wb;
            if (bytes_w == nbytes) { // Max bytes to write
                break;
            }
        } else { // if write fails
            break;
        }
    }
    return wb;
}

// Reads individual bits from infile
//
// infile: file to read from
// *bit: bit to return
//
bool read_bit(int infile, uint8_t *bit) {
    int end = -1;
    if (bits_read == 0) { // if i == 0 or i == BLOCK
        read_bytes(infile, buffer, BLOCK);

        // Read end characters if less than block
        if (bytes_read < BLOCK) {
            end = 8 * bytes_read + 1;
        }
    }
    *bit = (buffer[bits_read / 8] >> (bits_read % 8) & 0x1); // Get bit
    bits_read = (bits_read + 1) % (BLOCK * 8); // Circular increment
    if ((int) bits_read == end) {
        return false;
    }
    return true;
}

// Writes code to outfile
//
// outfile: file to write to
// *c: code pointer
//
void write_code(int outfile, Code *c) {
    for (uint32_t i = 0; i < code_size(c); i++) { // Write of code size
        uint8_t bit = (c->bits[i / 8] >> (i % 8) & 0x1); // Get bit
        if (bit == 1) {
            buffer[bits_added / 8] |= (0x1 << (bits_added % 8));
        } else { // Clear bit
            buffer[bits_added / 8] &= ~(0x1 << (bits_added % 8));
        }
        bits_added = (bits_added + 1) % (BLOCK * 8); // Circular increment

        if (bits_added == 0) { // Write if i == 0 or i == BLOCK
            write_bytes(outfile, buffer, BLOCK);
            memset(buffer, 0, sizeof(buffer)); // Clear buffer
        }
    }
    return;
}

// Flushes left over codes in buffer
//
// outfile: file to read from
//
void flush_codes(int outfile) {
    if (bits_added > 0) { // Check if there are bits to write
        uint64_t bytes = bits_added % (BLOCK * 8) / 8;
        if (bits_added % 8 > 0) { // Add if not full byte
            bytes++;
        }
        write_bytes(outfile, buffer, bytes);
    }
    return;
}
