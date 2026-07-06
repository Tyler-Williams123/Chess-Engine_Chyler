#include <stdio.h>
#include <stdint.h>
#include "defs.h"
#include "board.h"
#include "moveGeneration.h"
#include "Debugtools.h"


Board board;
MoveList moveList;
ZobristHash zobristHash;

int main(){
    FENInit(&board, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    initZobrist(&zobristHash);
    // makeMove(&board, Encode_Move(E7, C5, BQ, Empty, None, None));
    // initBoard(&board);

    // generateFen(&board);
    // smol count = 0;
    // printBoard(&board);
    
    // while(count < 100){
    //     Sleep(500);
    //     printf("\n\n");
        
    //     generateMoves(&board, &moveList);
    //     makeMove(&board, moveList.moves[0]);

    //     printBoard(&board);
    //     count++;
    // }

    // printf("PERFT: %llu\n", perft(2 ,&board));
    printf("PERFT: %llu\n", perftDevide(4 ,&board, &zobristHash));
    
    // makeMove(&board, Encode_Move(A8, B8, BR, Empty, Empty, None));
    // generateMoves(&board, &moveList);
    // for(int i = 0; i < moveList.count; i++){
    //     if(moveList.moves[i] & (1ULL << 25)){
    //         printf("yeet");
    //     } 
    // }

    return 0;
}