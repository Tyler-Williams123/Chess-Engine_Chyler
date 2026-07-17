all: Chyler

Chyler:
	cc chyler.c -O3 -march=native -funroll-loops -o Chyler