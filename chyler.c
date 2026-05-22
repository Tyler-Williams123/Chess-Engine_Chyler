#include <stdio.h>
#include <stdint.h>
#include "board.h"
Board board;



int main(){
    initBoard(&board);

    printBoard(&board);
    return 0;
}