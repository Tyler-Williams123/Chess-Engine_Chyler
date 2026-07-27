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
    FENInit(&board, "3r1k2/1pp2p2/p1np1p1p/3b4/3P1P2/P3Pq2/1PP4P/2K1QBR1 b - - 6 28");
    printBoard(&board);
    printf("\n");
    FENInit(&board, "3r1k2/1pp2p2/p2p1p1p/3b4/3n1P2/P3Pq2/1PP4P/2K1QBR1 w - - 0 29");
    printBoard(&board);
    
    MoveList m;
    generateMoves(&board, &m);
    move move1 = NO_MOVE;

    for(int i = 0; i < m.count; i++){
        if(m.moves[i] == move1){
            printf("found it");
        }
    }

    return 0;
}