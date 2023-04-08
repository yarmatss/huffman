.PHONY: clean valgrind

comp: comp.o minheap.o huffman.o
	cc -o comp comp.o minheap.o huffman.o

comp.o: comp.c
	cc -c comp.c

minheap.o: minheap.c
	cc -c minheap.c

huffman.o: huffman.c
	cc -c huffman.c

clean:
	-rm *.o comp *.bin a.out

valgrind:
	valgrind --show-leak-kinds=all --leak-check=full -s ./comp test.txt out.bin -o1


