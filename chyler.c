#include <stdio.h>
#include <stdint.h>
#include <windows.h>
#include <time.h>
#include "board.h"
#include "moveGeneration.h"


Board board;
MoveList moveList;

int main(){
    initBoard(&board);
    
    // smol count = 0;
    // srand(time(NULL));
    // printBoard(&board);
    
    // while(count < 100){
    //     Sleep(500);
    //     printf("\n\n");
        
    //     generateMoves(&board, &moveList);
    //     smol random = rand() % moveList.count;
    //     makeMove(&board, moveList.moves[random]);

    //     printBoard(&board);
    //     count++;
    // }

    // makeMove(&board, Encode_Move(G1, F3, WN, Empty, Empty, 0));
    printf("%llu", perft(7 ,&board));
    // printf("knightTargets(BlackKing) & whiteKnights = %llx\n", knightTargets(__builtin_ctzll(board.pieces[BK])) & board.pieces[WN]);
    // printf("BN: %llx WN: %llx\n", board.pieces[BN], board.pieces[WN]);
    // printf("%d\n", squareAttacked(&board, __builtin_ctzll(board.pieces[BK]), Black));

    //generateMoves(&board, &moveList);
    return 0;
}