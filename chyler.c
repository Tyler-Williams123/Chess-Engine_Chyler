#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "defs.h"
#include "board.h"
#include "moveGeneration.h"
#include "search.h"
#include "Debugtools.h"

Board board;
typedef struct{
    u64 bitboards[12];
    smol sideToMove;
    smol castling;
    smol enPessant;

    char FEN[100];
} DataEntry;


int main(){
    FILE* file = fopen("dataset.bin", "wb");
    FENInit(&board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    // smol count = 0;
    // printBoard(&board);
    
    // while(count < 100){
    //     printf("Eval: %d\n", evaluate(&board));
    //     move best = search(&board, 5);
    //     makeMove(&board, best);

    //     printBoard(&board);
    //     count++;
    // }

    // fwrite();
    fclose("dataset.bin");

    return 0;
}