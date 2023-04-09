all: comp decomp

comp: comp.o minheap.o huffman.o
	cc -o comp comp.o minheap.o huffman.o

comp.o: comp.c
	cc -c comp.c

minheap.o: minheap.c
	cc -c minheap.c

huffman.o: huffman.c
	cc -c huffman.c

clean:
	-rm *.o comp *.bin decomp kody

decomp.o: decomp.c
	cc -c decomp.c

decomp: decomp.o
	cc -o decomp decomp.o

valgrind:
	valgrind --show-leak-kinds=all --leak-check=full -s ./comp test.txt out.bin -o1

.PHONY: clean valgrind