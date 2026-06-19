all: Chyler

Chyler:
	gcc chyler.c -O3 -march=native -funroll-loops -o Chyler