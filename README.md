# Assignment 6 - Huffman Coding

The program must perform compression using the Huffman algorithm. The guiding principle is that the symbols that appear with higher frequencies are encoded using a small number of bits, while symbols that appear with lower frequencies can use a higher number of bits. This allows the overall content to be compressed to a smaller size. 

Originally proposed by Huffman, the program first creates a histogram, noting the frequency of each symbol in the text. This histogram is used to create a tree that we call the Huffman tree. As described above, the logic used in the construction of the tree is that the symbols that appear with higher frequencies are leaf nodes that have the shortest path to the root nodes. As the frequency reduces, the distance to the root node increases. This allows us to generate codes for each symbol, which are then used to encode the text in a compressed format.

The decode process reverses this logic. It recreates the Huffman tree from the information stored in the file. Each code that follows can then be replaced by the symbol to get the original file back.

The following are the user command-line options for running `encode` or `decode`:

-i <input_file>: Description below (default is stdin)
-o <output_file>: Description below (default is stdout)
-v: Print compression statistics to stderr
-h: Print the usage message

In `encode`, the command-line option "i" denotes the input file to encode, and the option "o" denotes the file to write the compressed output to. Meanwhile, in `decode`, the option "i" denotes the the input file to decode, and option "o" denotes the file to write the decompressed output to.


## Building

Run the following to build the `encode` and `decode` programs:

```
$ make all
```


## Running

```
$ ./encode [-i <infile>][-o <outfile>][-vh]
```

```
$ ./decode [-i <infile>][-o <outfile>][-vh]
```


## Testing

I have added two targets in the Makefile to test both executables and two others to check for memory leaks in either file.

```
$ make tst
$ make tst2
$ make tst_valgrind
$ make tst_valgrind2
```

I detected no memory leaks when the third and fourth targets were last invoked. Lastly, scan-build reported no false positives, nor any other bugs.


## Known Bugs

I have observed some issues while testing. In some cases, the last line was not reproduced exactly. For example, I have included a test file input_text. When I encoded and then decoded this file, the last line of the output is different from the original file. 

The issue seems to be in `encode`. When I use the given `encode` program to encode this file and my generated `decode`, it works fine. The reverse case is not true, suggesting that the issue resides there.

