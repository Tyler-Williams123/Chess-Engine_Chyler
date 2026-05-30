#ifndef Move_Generation_H
#define Move_Generation_H
#include "board.h"

typedef struct{
    move moves[256];
    smol count;
}MoveList;

u64 knightMoves(u64 bb){
    u64 notA = ~0x0101010101010101ULL;
    u64 notB = ~0x0202020202020202ULL;

    u64 notG = ~0x4040404040404040ULL;
    u64 notH = ~0x8080808080808080ULL;

    u64 moves = ((bb & notH) << 17) |
                ((bb & notA) << 15) |
                ((bb & notG & notH) << 10) |
                ((bb & notA & notB) << 6)  |
                
                ((bb & notA) >> 17) |
                ((bb & notH) >> 15) |
                ((bb & notA & notB) >> 10) |
                ((bb & notG & notH) >> 6);

    return moves;
}

u64 kingMoves(u64 bb){
    u64 notA = ~0x0101010101010101ULL;
    u64 notH = ~0x8080808080808080ULL;
    
    u64 moves = (bb << 8) |
                (bb >> 8) |
                ((bb & notH) << 1) |
                ((bb & notH) << 9) |
                ((bb & notA) << 7) |
                ((bb & notA) >> 1) |
                ((bb & notA) >> 9) |
                ((bb & notH) >> 7);
                
    return moves;
}

u64 pawnSingleMoves(u64 bb, smol color){
    u64 moves;

    if(color)
        moves = bb >> 8;
    else
        moves = bb << 8;

    return moves;
}

u64 pawnDoubleMoves(u64 bb, smol color){
    u64 row7 = 0x00ff000000000000ULL;
    u64 row2 = 0x000000000000ff00ULL;
    u64 moves;

    if(color)
        moves = (bb & row7) >> 16;
    else
        moves = (bb & row2) << 16;

    return moves;
}

u64 pawnAttacks(u64 bb, smol color){
    u64 notA = ~0x0101010101010101ULL;
    u64 notH = ~0x8080808080808080ULL;
    u64 moves;

    if(color){
        moves = ((bb & notA) >> 9) |
                ((bb & notH) >> 7);
    }
    else{
        moves = ((bb & notH) << 9) |
                ((bb & notA) << 7);
    }

    return moves;
}

u64 rookAttacks(Board b, smol color){
    return 0;
}

void generateMoves(Board* b, MoveList* m){
    u64 empty = ~b->allPieces;

    u64 wPawnSingle = pawnSingleMoves(b->pieces[WP], White) & empty;
    u64 bPawnSingle = pawnSingleMoves(b->pieces[BP], Black) & empty;
    
    u64 wPawnDouble = pawnDoubleMoves(b->pieces[WP], White) & empty & (empty << 8);
    u64 bPawnDouble = pawnDoubleMoves(b->pieces[BP], Black) & empty & (empty >> 8);

    u64 wPawnCaptures = pawnAttacks(b->pieces[WP], White) & b->coloredPieces[Black];
    u64 bPawnCaptures = pawnAttacks(b->pieces[BP], Black) & b->coloredPieces[White];

    u64 wKnightShift = knightMoves(b->pieces[WKN]) & ~b->coloredPieces[White];
    u64 bKnightShift = knightMoves(b->pieces[BKN]) & ~b->coloredPieces[Black];

    
}

#endif