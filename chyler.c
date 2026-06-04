#include <stdio.h>
#include <stdint.h>
#include <windows.h>
#include "board.h"
#include "moveGeneration.h"

Board board;
MoveList moveList;

int main(){
    initBoard(&board);
    printBoard(&board);
    
    Sleep(2000);
    
    printf("\n\n");
    
    generateMoves(&board, &moveList);
    makeMove(&board, moveList.moves[1]);
    makeMove(&board, moveList.moves[2]);
    makeMove(&board, moveList.moves[4]);
    makeMove(&board, moveList.moves[6]);

    
    printBoard(&board);

    return 0;
}