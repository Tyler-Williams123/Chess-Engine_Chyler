#include <stdio.h>
#include <stdint.h>
#include "board.h"
#include "moveGeneration.h"
#include "defs.h"
#include "Debugtools.h"


Board board;
MoveList moveList;

int main(){
    FENInit(&board, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    // initBoard(&board);
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

    // makeMove(&board, Encode_Move(G1, F3, WN, Empty, Empty, 0));
    printf("PERFT: %llu\n", perftDevide(4 ,&board));
    // printf("PERFT: %llu\n", perft(3 ,&board));
    
    // makeMove(&board, Encode_Move(A8, B8, BR, Empty, Empty, None));
    // generateMoves(&board, &moveList);
    // for(int i = 0; i < moveList.count; i++){
    //     if(moveList.moves[i] & (1ULL << 25)){
    //         printf("yeet");
    //     } 
    // }

    return 0;
}