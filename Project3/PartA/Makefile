all: test
	echo 'Made main'

test: mm.o main_mm.c
	gcc -o test main_mm.c mm.o

mm.o: mm.c mm.h
	gcc -o mm.o -c mm.c

clean:
	rm -rf test mm.o
