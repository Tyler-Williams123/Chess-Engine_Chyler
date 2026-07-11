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
    initBoard(&board);

    smol count = 0;
    printBoard(&board);
    
    while(count < 100){
        Sleep(500);
        printf("\n\n");
        
        move best = search(&board, 3);
        makeMove(&board, best);

        printBoard(&board);
        count++;
    }

    return 0;
}