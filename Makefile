.PHONY: clean valgrind

comp: comp.o
	cc -o comp comp.o

comp.o: comp.c
	cc -c comp.c

clean:
	-rm *.o comp *.bin

valgrind:
	valgrind --show-leak-kinds=all --leak-check=full -s ./comp test.txt out.bin -o1


