#include "defines.h"
#include "header.h"
#include "huffman.h"
#include "io.h"
#include "node.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
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
    printf("-i <infile>: Input file to decode. Default is stdin\n");
    printf("-o <outfile>: File to write the decompressed output to. Default is "
           "stdout\n");
    printf("-v: Print compression statistics to stderr\n");
    printf("-h: Print this message\n");
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
    int ifd = 0;
    int ofd = 1;
    bool verbose = false;
    uint8_t buf[BLOCK];
    Header header;
    Node *root, *n;
    uint8_t bit;
    uint16_t tree_size;
    uint64_t file_size;

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

    read(ifd, &header, sizeof(header));

    if (header.magic != 0xBEEFBBAD) {
        printf("The magic number is not 0xBEEFBBAD.\n");
        printf("The input file is not correctly encoded\n");
        return 1;
    }

    if (outfile != NULL) {
        if ((ofd = open(outfile, O_CREAT | O_WRONLY | O_TRUNC)) == -1) {
            printf("Unable to open output file for writing\n");
            return 1;
        }
        fchmod(ofd, header.permissions);
    }

    tree_size = header.tree_size;
    file_size = header.file_size;
    read_bytes(ifd, buf, tree_size);
    root = rebuild_tree(tree_size, buf);

    n = root;
    uint64_t decoded_symbols_count = 0;
    while (decoded_symbols_count < file_size) {
        if (n == NULL) {
            continue;
        }
        if (n->left == NULL && n->right == NULL) { // Leaf node
            write_bytes(ofd, &(n->symbol), 1);
            n = root;
            decoded_symbols_count += 1;
        } else {
            read_bit(ifd, &bit);
            if (bit == 0) {
                n = n->left;
            } else {
                n = n->right;
            }
        }
    }

    if (verbose == true) {
        // Obtain size of the output file
        struct stat ifd_buffer, ofd_buffer;
        double i_size, o_size;

        fstat(ifd, &ifd_buffer);
        fstat(ofd, &ofd_buffer);
        i_size = (double) ifd_buffer.st_size;
        o_size = (double) ofd_buffer.st_size;
        printf("Compressed file size = %ld bytes\n", (long) i_size);
        printf("Decompressed file size = %ld bytes\n", (long) o_size);
        printf("Decompression size change = %0.2f%%\n", (1 - (i_size / o_size)) * 100);
    }

    if (ifd != 0) {
        close(ifd);
    }
    if (ofd != 1) {
        close(ofd);
    }
    delete_tree(&root);
}
