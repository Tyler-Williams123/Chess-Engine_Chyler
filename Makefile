all: Chyler

Chyler: $(wildcard *.c *.h)
	gcc *.c -O3 -march=native -funroll-loops -o Chyler
