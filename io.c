#include "io.h"
#include "code.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Remove these includes later. Added for debugging an error
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

static uint8_t code_buf[BLOCK] = { 0 };
static int code_index = 0;

// Used to read the contents from infile. We create a wrapper around
// the read() system call, that loops till the desired number of
// bytes (nbytes) are read.
//
// Input parameters:
// infile: int: File descriptor of the file to be read
// buf: uint8_t *: Buffer containing the read bytes
// nbytes: int: Number of bytes to be read
// Returns: int: Number of bytes read
int read_bytes(int infile, uint8_t *buf, int nbytes) {
    ssize_t bytes_read = 0;

    // Initialize the buffer to 0 to ensure that if the amount of
    // data read is less than the buffer size, histogram is
    // not corrupted.
    memset(buf, 0, BLOCK);

    while (bytes_read < nbytes) {
        ssize_t num_bytes = read(infile, buf + bytes_read, nbytes - bytes_read);

        if (num_bytes == 0) {
            // End of file
            break;
        }

        bytes_read += num_bytes;
    }
    return bytes_read;
}

// Used to write the contents to outfile. We create a wrapper around
// the write() system call, that loops till the desired number of
// bytes (nbytes) are writeen.
//
// Input parameters:
// outfile: int: File descriptor of the file to be written
// buf: uint8_t *: Buffer containing the bytes to be written
// nbytes: int: Number of bytes to be written
// Returns: int: Number of bytes written
int write_bytes(int outfile, uint8_t *buf, int nbytes) {
    ssize_t bytes_written = 0;

    while (bytes_written < nbytes) {
        ssize_t num_bytes = write(outfile, buf + bytes_written, nbytes - bytes_written);

        if (num_bytes == 0) {
            // End of file
            break;
        }

        bytes_written += num_bytes;
    }
    return bytes_written;
}

// Read a block of bytes from infile into a buffer, and dole out one
// bit at a time, till all bits have been doled out.
//
// Input parameters:
// infile: int: File descriptor of the file to be read
// bit: uint8_t *: doled out bit
// Returns: bool: false if there are no more bits to be read, true otherwise.
bool read_bit(int infile, uint8_t *bit) {
    static uint8_t buf[BLOCK];
    static int num_bytes = 0;
    static int byte_index = 0;
    static int bit_index = 0;

    // We want to read BLOCK number of bytes, and dole out one bit at
    // a time. To ensure we don't read into buf each time we enter
    // the function, we add this check.
    if (num_bytes == 0 || byte_index == num_bytes) {
        num_bytes = read_bytes(infile, buf, BLOCK);
        byte_index = 0;
        bit_index = 0;
        if (num_bytes == 0) {
            return false;
        }
    }
    *bit = (buf[byte_index] & (0x1 << (bit_index & 0x7))) >> bit_index;
    bit_index += 1;

    // Reset at the end of each byte
    if (bit_index == 8) {
        bit_index = 0;
        byte_index += 1;
    }
    return true;
}

// Write bits from Code c into a buffer. Once the buffer is full, it
// will be writeen to the outfile.
//
// Input parameters:
// outfile: int: File descriptor of the file to be written
// c: Code *: Code to be written to the buffer
// Returns: void
void write_code(int outfile, Code *c) {
    // Read bits from the code c into the static buffer
    for (uint32_t i = 0; i < code_size(c); i++) {
        if (code_get_bit(c, i) == true) {
            code_buf[code_index >> 3] |= 0xff & (1 << (code_index & 0x7));
        } else {
            code_buf[code_index >> 3] |= 0xff & (0 << (code_index & 0x7));
        }
        code_index += 1;

        if (code_index == BLOCK) {
            write_bytes(outfile, code_buf, BLOCK);
            code_index = 0;
            memset(code_buf, 0, BLOCK);
        }
    }
    return;
}

// Write out any leftover, buffered bits. Since code_buf is initialized
// to 0, and is reset after being written, the extra bits should already
// be zeroed out.
//
// Input parameters:
// outfile: int
// Returns: void
void flush_codes(int outfile) {
    write_bytes(outfile, code_buf, code_index >> 3);
    return;
}
