all: comp decomp

comp: comp.o minheap.o huffman.o
	cc -o bin/comp comp.o minheap.o huffman.o

decomp: src/decomp.o
	cc -o bin/decomp src/decomp.o

comp.o: src/comp.c
	cc -c src/comp.c

decomp.o: src/decomp.c
	cc -c src/decomp.c

minheap.o: src/minheap.c
	cc -c src/minheap.c

huffman.o: src/huffman.c
	cc -c src/huffman.c

clean:
	-rm *.o *.txt *.bin src/decomp.o kody

.PHONY: clean