CC=clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic

all: encode decode

encode: encode.o io.o pq.o node.o huffman.o code.o stack.o
	$(CC) $(CFLAGS) -o encode encode.o io.o pq.o node.o huffman.o code.o stack.o

decode: decode.o io.o node.o huffman.o code.o stack.o pq.o
	$(CC) $(CFLAGS) -o decode decode.o io.o node.o huffman.o code.o stack.o pq.o

encode.o: encode.c
	$(CC) $(CFLAGS) -c encode.c

decode.o: decode.c
	$(CC) $(CFLAGS) -c decode.c

node.o: node.c
	$(CC) $(CFLAGS) -c node.c

io.o: io.c
	$(CC) $(CFLAGS) -c io.c

pq.o: pq.c
	$(CC) $(CFLAGS) -c pq.c

huffman.o: huffman.c
	$(CC) $(CFLAGS) -c huffman.c

code.o: code.c
	$(CC) $(CFLAGS) -c code.c

stack.o: stack.c
	$(CC) $(CFLAGS) -c stack.c

clean:
	rm -f *.o encode decode

format:
	clang-format -i -style=file *.[c,h]

tst:
	echo "banana" > banana
	./encode -i banana -o banana.enc
	./decode -i banana.enc -o banana.dec
	diff banana banana.dec
	rm banana banana.enc banana.dec

tst_valgrind:
	echo "banana" > banana
	valgrind ./encode -i banana -o banana.enc
	valgrind ./decode -i banana.enc -o banana.dec
	rm banana banana.enc banana.dec


tst2:
	echo "This is a test file." > abc
	echo "It spans two lines." >> abc
	./encode -i abc -o abc.enc
	./decode -i abc.enc -o abc.dec
	diff abc abc.dec
	rm abc abc.enc abc.dec

tst_valgrind2:
	echo "This is a test file." > abc
	echo "It spans two lines." >> abc
	valgrind ./encode -i abc -o abc.enc
	valgrind ./decode -i abc.enc -o abc.dec
	rm abc abc.enc abc.dec

