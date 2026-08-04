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
    FENInit(&board, "1r1k4/p1p2P2/B1p2bp1/3n4/1q2P3/8/PpP1KPQR/3R4 w - - 0 1");
    printBoard(&board);
    printf("\n");
    FENInit(&board, "1r1k4/p1p2P2/B1p2bQ1/3n4/1q2P3/8/PpP1KP1R/3R4 b - - 0 1");
    printBoard(&board);

    return 0;
}