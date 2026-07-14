#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <windows.h>

#include "defs.h"
#include "board.h"
#include "moveGeneration.h"
#include "search.h"
#include "Debugtools.h"

Board board;

int main(){
    FENInit(&board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    // int total = 0;
    // for(int sq = A1; sq <= H8; sq++){
    //     if(board.pieceArr[sq] == Empty)
    //         continue;
            
    //     smol color = board.pieceArr[sq] > 6 ? Black : White;
    //     total = 0;
    //     if(color){
    //         total += pieceValue[(board.pieceArr[sq] - 1) % 6];
    //         total += pieceSQTable[board.pieceArr[sq] - 6][sq ^ 56];
    //         printf("Piece contribution of square %d: %d", sq, total);
    //     }
    //     else{
    //         total += pieceValue[(board.pieceArr[sq] - 1)];
    //         total += pieceSQTable[board.pieceArr[sq]][sq];
    //         printf("Piece contribution of square %d: %d", sq, total);
    //     }
    // }

    smol count = 0;
    printBoard(&board);
    
    while(count < 100){
        printf("Eval: %d\n", evaluate(&board));
        move best = search(&board, 5);
        makeMove(&board, best);

        printBoard(&board);
        count++;
    }

    return 0;
}