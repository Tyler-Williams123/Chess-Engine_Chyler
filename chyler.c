#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "defs.h"
#include "board.h"
#include "moveGeneration.h"
#include "search.h"
#include "debugtools.h"

Board board;

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
    u64 size = perft(1, &board) + perft(2, &board) + perft(3, &board) + perft(4, &board) + 1;
    DataEntry* data = malloc(sizeof(DataEntry) * size);
    int count = 0;
    generateEvalNNBinaryData(4, &board, data, &count);


    fwrite(data, sizeof(DataEntry), size, file);
    fclose(file);

    free(data);
    return 0;
}