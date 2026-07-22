all: Chyler

Chyler:
	cc chyler.c defs.c board.c moveGeneration.c search.c debugtools.c -O3 -march=native -funroll-loops -o Chyler