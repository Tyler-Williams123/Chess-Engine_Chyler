#include <stdio.h>
#include <stdint.h>
#include <windows.h>
#include "board.h"

#define Encode_Move(from, to, piece, capturedPiece) (from | (to << 6) | (piece << 12) | (capturedPiece << 16))

Board board;

int main(){
    initBoard(&board);

    printBoard(&board);
    
    Sleep(2000);
    
    printf("\n\n");
    move knightD4 = Encode_Move(B1, C3, WKN, Empty);
    
    makeMove(&board, knightD4);

    
    printBoard(&board);

    return 0;
}