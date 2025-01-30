flags=-O2 -Wall -std=c2x
ldflags=-lbu

.PHONY: all clean

all: clean Cebola

Cebola: Cebola.o
	cc $(flags) $^ -o $@ $(ldflags)

Cebola.o: Cebola.c Cebola.h
	cc $(flags) -c $<

clean:
	rm -f *.o Cebola