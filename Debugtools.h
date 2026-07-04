#include <stdio.h>
#include <stdlib.h>
#include "moveGeneration.h"
#include "defs.h"
#include "board.h"

enum {WPHash = 0, WNHash = (WN - 1) * 64, WBHash = (WB - 1) * 64, WRHash = (WR - 1) * 64, WQHash = (WQ - 1) * 64, WKHash = (WK - 1) * 64,
      BPHash = (BP - 1) * 64, BNHash = (BN - 1) * 64, BBHash = (BB - 1) * 64, BRHash = (BR - 1) * 64, BQHash = (BQ - 1) * 64, BKHash = (BK - 1) * 64,
      BlackMoveHash = 768,
      WQCastleHash, WKCastleHash, BQCastleHash, BKCastleHash,
      AHash, BHash, CHash, DHash, EHash, FHash, GHash, HHash};


typedef struct{
    u64 randoms[HHash + 1];
    
}ZobristHash;

void initZobristArray(ZobristHash* zobrist){ // figure out how to do randoms
    for(int i = 0; i <= HHash; i++){
    }
}

smol verifyBoard(Board* board){ 
    u64 pieces[13] = {0};
    u64 white = 0;
    u64 black = 0;
    u64 all = 0;

    smol correct = 1;

    for(int j = 0; j < 64; j++){
        if(board->pieceArr[j] == Empty) continue;
        pieces[board->pieceArr[j]] = pieces[board->pieceArr[j]] | (1ULL << j);
    }
    for(int j = WP; j <= WK; j++){
        white |= pieces[j];
    }
    for(int j = BP; j <= BK; j++){
        black |= pieces[j];
    }
    all = white | black;

    for(int i = WP; i <= BK; i++){
        if(board->pieces[i] != pieces[i]){
            printf("Mismatch in piece bitboard: %d \n", i);
            correct = 0;
        }
    }
    if(white != board->coloredPieces[White]){
        printf("Mismatch in white pieces bitboard \n");
        correct = 0;
    }
    if(black != board->coloredPieces[Black]){
        printf("Mismatch in black pieces bitboard \n");
        correct = 0;
    }
    if(all != board->allPieces){
        printf("Mismatch in all pieces bitboard \n");
        correct = 0;
    }

    return correct;
}

u64 perft(smol depth, Board* b){
    MoveList moves;
    u64 total = 0;

    if(depth <= 0){
        return 1;
    }

    generateMoves(b, &moves);
    for(int i = 0; i < moves.count; i++){
        b->ply++;
        makeMove(b, moves.moves[i]);
        total += perft(depth - 1, b);
        undoMove(b, moves.moves[i]);
        b->ply--;
    }
    return total;
}

u64 perftDevide(smol depth, Board* b){
    MoveList moves;
    u64 total = 0;

    if(depth <= 0){
        return 1;
    }

    generateMoves(b, &moves);
    for(int i = 0; i < moves.count; i++){
        b->ply++;
        makeMove(b, moves.moves[i]);

        // debugging
        smol verify = verifyBoard(b);
        if(!verify){
            char debugMove[6];
            moveToString(moves.moves[i], debugMove);
            printf("broke at ply: %d by move: %s", b->ply, debugMove, moves.moves[i]);
            printMove(moves.moves[i]);
        }
        assert(verify);
        //debugging
        
        u64 perf = perft(depth - 1, b);
        total += perf;
        
        char buffer[6];
        moveToString(moves.moves[i], buffer);
        printf("%s %llu/", buffer, perf);
        
        undoMove(b, moves.moves[i]);
        b->ply--;

        // debugging
        verify = verifyBoard(b);
        if(!verify){
            char debugMove[6];
            moveToString(moves.moves[i], debugMove);
            printf("broke at ply: %d by move: %s move binary is: %b \n", b->ply, debugMove, moves.moves[i]);
        }
        assert(verify);
        //debugging
    }

    return total;
}