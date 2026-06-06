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

    printf("%d", perft(5 ,&board));

    // makeMove(&board, Encode_Move(G1, F3, WN, Empty));
    // printf("knightTargets(BlackKing) & whiteKnights = %llx\n", knightTargets(__builtin_ctzll(board.pieces[BK])) & board.pieces[WN]);
    // printf("BN: %llx WN: %llx\n", board.pieces[BN], board.pieces[WN]);
    // printf("%d\n", squareAttacked(&board, __builtin_ctzll(board.pieces[BK]), Black));

    //generateMoves(&board, &moveList);
    return 0;
}