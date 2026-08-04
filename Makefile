all: Chyler

Chyler:
	cc *.c -O3 -march=native -funroll-loops -o Chyler