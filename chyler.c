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
MoveList moveList;
ZobristHash zobristHash;

int main(){
    FENInit(&board, "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
    // initZobrist(&zobristHash);
    // makeMove(&board, Encode_Move(E7, C5, BQ, Empty, None, None));
    // initBoard(&board);

    // generateFen(&board);
    // smol count = 0;
    // printBoard(&board);
    
    // while(count < 100){
    //     Sleep(500);
    //     printf("\n\n");
        
    //     generateMoves(&board, &moveList);
    //     makeMove(&board, moveList.moves[0]);

    //     printBoard(&board);
    //     count++;
    // }

    printf("PERFT: %llu\n", perft(5 ,&board));

    return 0;
}