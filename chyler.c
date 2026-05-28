#include <stdio.h>
#include <stdint.h>
#include <windows.h>
#include "board.h"
Board board;



int main(){
    initBoard(&board);

    printBoard(&board);
    
    Sleep(2000);
    
    printf("\n\n");
    move knightD4 = 0b00000000000000000011010010000001;
    
    makeMove(&board, knightD4);
    
    printBoard(&board);

    return 0;
}