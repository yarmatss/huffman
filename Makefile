comp: comp.o
	cc -o comp comp.o

comp.o: comp.c
	cc -c comp.c

clean:
	-rm *.o comp
