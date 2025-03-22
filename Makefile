flags=-O2 -Wall -std=c23 -L/usr/local/lib -lelutils -I/usr/local/include
CC = /opt/homebrew/bin/gcc-14

.PHONY: all clean

all: clean Cebola

Cebola: Cebola.o
	$(CC) $(flags) $^ -o $@ $

Cebola.o: Cebola.c Cebola.h
	$(CC) $(flags) -c $<

clean:
	rm -f *.o Cebola