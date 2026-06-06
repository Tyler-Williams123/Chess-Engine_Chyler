#include <stdio.h>
#include <stdint.h>
#include <windows.h>
#include "board.h"
#include "moveGeneration.h"


Board board;
MoveList moveList;

int main(){
    initBoard(&board);
    
    smol count = 0;
    // printBoard(&board);
    
    // while(count < 100){
    //    Sleep(500);
    //    printf("\n\n");
        
    //    generateMoves(&board, &moveList);
    //    makeMove(&board, moveList.moves[0]);

    //    printBoard(&board);
    //    count++;
    // }

    // printf("%d", perftDevide(2 ,&board));

    generateMoves(&board, &moveList);
    return 0;
}