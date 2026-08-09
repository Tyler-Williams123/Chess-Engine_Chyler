all: Chyler

Chyler:
	gcc *.c -O3 -march=native -funroll-loops -o Chyler