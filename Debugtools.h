#include <stdio.h>
#include <stdlib.h>
#include "moveGeneration.h"
#include "defs.h"
#include "board.h"

enum {WPHash = 0, WNHash = (WN - 1) * 64, WBHash = (WB - 1) * 64, WRHash = (WR - 1) * 64, WQHash = (WQ - 1) * 64, WKHash = (WK - 1) * 64,
      BPHash = (BP - 1) * 64, BNHash = (BN - 1) * 64, BBHash = (BB - 1) * 64, BRHash = (BR - 1) * 64, BQHash = (BQ - 1) * 64, BKHash = (BK - 1) * 64,
      BlackMoveHash = 768,
      WKCastleHash, WQCastleHash, BKCastleHash, BQCastleHash,
      AHash, HHash = AHash + 7};


typedef struct{
    u64 randoms[HHash + 1];
    Dictionary table;
}ZobristHash;

void initZobristArray(ZobristHash* zobrist){ // figure out how to do randoms
    for(int i = 0; i <= HHash; i++){
        zobrist->randoms[rand64()];
    }
    dictInit(&zobrist->table, 20);
}

u64 zobrist(Board* b, ZobristHash* z){ // need to fix for phantom en Pessant;
    u64 hash = 0;
    for(int sq = A1; sq <= H8; sq++){
        smol piece = b->pieceArr[sq];
        switch(piece){
            case(WP): hash ^= z->randoms[WPHash + sq]; break;
            case(WN): hash ^= z->randoms[WNHash + sq]; break;
            case(WB): hash ^= z->randoms[WBHash + sq]; break;
            case(WR): hash ^= z->randoms[WRHash + sq]; break;
            case(WQ): hash ^= z->randoms[WQHash + sq]; break;
            case(WK): hash ^= z->randoms[WKHash + sq]; break;
            case(BP): hash ^= z->randoms[BPHash + sq]; break;
            case(BN): hash ^= z->randoms[BNHash + sq]; break;
            case(BB): hash ^= z->randoms[BBHash + sq]; break;
            case(BR): hash ^= z->randoms[BRHash + sq]; break;
            case(BQ): hash ^= z->randoms[BQHash + sq]; break;
            case(BK): hash ^= z->randoms[BKHash + sq]; break;
        }
    }

    hash ^= b->turn ? z->randoms[BlackMoveHash] : 0;
    
    hash ^= b->castleRights & 1 ? z->randoms[WKCastleHash] : 0;
    hash ^= (b->castleRights >> 1) & 1 ? z->randoms[WQCastleHash] : 0;
    hash ^= (b->castleRights >> 2) & 1 ? z->randoms[BKCastleHash] : 0;
    hash ^= (b->castleRights >> 3) & 1 ? z->randoms[BQCastleHash] : 0;

    hash ^= b->enPessant != NO_SQ ? z->randoms[b->enPessant % 8 + AHash] : 0;

    return hash;
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