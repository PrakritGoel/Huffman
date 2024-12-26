#include "header.h"
#include "huffman.h"
#include "io.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

Code c;

// Usage Function
// Input parameters:
// exec_name: char *: Name of the program
// Returns: void
void usage(char *exec_name) {
    printf("USAGE: %s [-i <infile>][-o <outfile>][-vh]\n", exec_name);
    printf("-i <infile>: Input file to encode. Default is stdin\n");
    printf("-o <outfile>: File to write the compressed output to. Default is "
           "stdout\n");
    printf("-v: Print compression statistics to stderr\n");
    printf("-h: Print this message\n");
    return;
}

// Create the frequency table for the input file. The file is read as
// bytes, and each byte is used as an index into the histogram. Each time
// the byte is encountered in the file, the frequency is incremented by 1.
// To ensure that there are at least 2 nodes in the tree that's created
// from this histogram, the first and the last frequency is incremented by 1.
//
// Input parameters:
// infile: char *: Input file
// h: uint64_t *: Pointer to the histogram
// Returns: void
void create_histogram(int ifd, uint64_t *h) {
    uint8_t buf[BLOCK];
    int num_bytes_read;

    h[0] += 1;
    h[ALPHABET - 1] += 1;

    // Read the infile using read_bytes(). Use the byte value as an index
    // to the histogram and increment the frequency by 1.
    while ((num_bytes_read = read_bytes(ifd, buf, BLOCK)) != 0) {
        for (int i = 0; i < num_bytes_read; i++) {
            h[buf[i]] += 1;
        }
    }
    return;
}

// The main function
//
// Input parameters:
// argc: int: Number of input arguments
// argv: char **: The input arguments
// Returns: int: 0 in case of success, non-zero for failure
int main(int argc, char **argv) {
    int opt;
    char *infile = NULL;
    char *outfile = NULL;
    bool verbose = false;
    uint64_t histogram[ALPHABET] = { 0 };
    Node *root;
    Code table[ALPHABET];
    Header header;
    struct stat statbuf;
    int ifd = 0;
    int ofd = 1;
    uint32_t hist_size = 0;
    uint8_t buf[BLOCK];
    int num_bytes_read;

    // Parse the input options.
    while ((opt = getopt(argc, argv, "i:o:vh")) != -1) {
        switch (opt) {
        case ('i'): infile = optarg; break;
        case ('o'): outfile = optarg; break;
        case ('v'): verbose = true; break;
        case ('h'): usage(argv[0]); return 0;
        default: usage(argv[0]); exit(EXIT_FAILURE);
        }
    }

    if (infile != NULL) {
        if ((ifd = open(infile, O_RDONLY)) == -1) {
            printf("Unable to open input file for reading\n");
            return 1;
        }
    }

    // Obtain permissions for the input file using fstat
    fstat(ifd, &statbuf);

    // Create a frequency table (histogram) for each symbol
    // in the input file.
    create_histogram(ifd, histogram);

    // Build a Huffman tree and code table from the histogram
    root = build_tree(histogram);
    c = code_init();
    build_codes(root, table);
    // print_codes(root, table);

    header.magic = MAGIC;
    if (outfile != NULL) {
        if ((ofd = open(outfile, O_CREAT | O_WRONLY | O_TRUNC)) == -1) {
            printf("Error opening output file\n");
            return 1;
        }
        fchmod(ofd, statbuf.st_mode);
    }
    header.permissions = statbuf.st_mode & 0777;

    // Determine the number of unique symbols in the tree. This
    // should be equal to the number of leaf nodes, which is also
    // equal to the elements in histogram with non-zero frequencies.

    hist_size = 0;
    for (uint32_t i = 0; i < ALPHABET; i++) {
        if (histogram[i] != 0) {
            hist_size += 1;
        }
    }
    header.tree_size = (3 * hist_size) - 1;
    header.file_size = statbuf.st_size;

    write(ofd, &header, sizeof(header));
    dump_tree(ofd, root);

    lseek(ifd, 0, SEEK_SET);
    while ((num_bytes_read = read_bytes(ifd, buf, BLOCK)) != 0) {
        for (int i = 0; i < num_bytes_read; i++) {
            write_code(ofd, &table[buf[i]]);
        }
    }
    flush_codes(ofd);

    if (verbose == true) {
        // Obtain size of the output file
        struct stat ofd_buffer;
        double i_size, o_size;

        fstat(ofd, &ofd_buffer);
        i_size = (double) statbuf.st_size;
        o_size = (double) ofd_buffer.st_size;
        printf("Uncompressed file size = %ld bytes\n", (long) i_size);
        printf("Compressed file size = %ld bytes\n", (long) o_size);
        printf("Compression gain = %0.2f%%\n", (1 - (o_size / i_size)) * 100);
    }

    if (ifd != 0) {
        close(ifd);
    }

    if (ofd != 1) {
        close(ofd);
    }

    delete_tree(&root);
    return 0;
}
