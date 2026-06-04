#ifndef Move_Generation_H
#define Move_Generation_H
#include "board.h"

#define Encode_Move(from, to, piece, capturedPiece) (from | (to << 6) | (piece << 12) | (capturedPiece << 16))

typedef struct{
    move moves[256];
    smol count;
}MoveList;

u64 knightTargets(smol sq){
    u64 notA = ~0x0101010101010101ULL;
    u64 notB = ~0x0202020202020202ULL;

    u64 notG = ~0x4040404040404040ULL;
    u64 notH = ~0x8080808080808080ULL;

    u64 bb = 1ULL << sq;

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

void generateWhiteKnightMoves(Board* b, MoveList* m){
    u64 wKnights = b->pieces[WN];
    u64 bKnights = b->pieces[BN];

    while(wKnights){
        u64 knight = wKnights & -wKnights;
        smol from = __builtin_ctzll(knight);

        u64 moves = knightTargets(from) & ~b->coloredPieces[White];

        while(moves){
            smol to = __builtin_ctzll(moves & -moves);
            smol captured = b->pieceArr[to];
            m->moves[m->count++] = Encode_Move(from, to, WN, captured);

            moves &= moves - 1;
        }
        wKnights &= wKnights - 1;
    }
}

void generateBlackKnightMoves(Board* b, MoveList* m){
    u64 wKnights = b->pieces[WN];
    u64 bKnights = b->pieces[BN];

    while(bKnights){
        u64 knight = bKnights & -bKnights;
        smol from = __builtin_ctzll(knight);

        u64 moves = knightTargets(from) & ~b->coloredPieces[Black];

        while(moves){
            smol to = __builtin_ctzll(moves & -moves);
            smol captured = b->pieceArr[to];
            m->moves[m->count++] = Encode_Move(from, to, BN, captured);

            moves &= moves - 1;
        }
        bKnights &= bKnights - 1;
    }
}

u64 kingTargets(smol sq){
    u64 notA = ~0x0101010101010101ULL;
    u64 notH = ~0x8080808080808080ULL;
    
    u64 bb = 1ULL << sq;
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

void generateWhiteKingMoves(Board* b, MoveList* m){
   u64 bb = b->pieces[WK];
   smol from = __builtin_ctzll(bb);

   u64 moves = kingTargets(from) & ~b->coloredPieces[White];

    while(moves){
        smol to = moves & -moves;
        smol captured = b->pieceArr[to];

        m->moves[m->count++] = Encode_Move(from, to, WK, captured);

        moves &= moves - 1;
    }
}

void generateBlackKingMoves(Board* b, MoveList* m){
   u64 bb = b->pieces[BK];
   smol from = __builtin_ctzll(bb);

   u64 moves = kingTargets(from) & ~b->coloredPieces[Black];

    while(moves){
        smol to = __builtin_ctzll(moves & -moves);
        smol captured = b->pieceArr[to];

        m->moves[m->count++] = Encode_Move(from, to, BK, captured);

        moves &= moves - 1;
    }
}

u64 pawnSingleMoves(smol sq, smol color){
    u64 move;

    if(color)
        move = (1ULL << sq) >> 8;
    else
        move = (1ULL << sq) << 8;

    return move;
}

u64 pawnDoubleMoves(smol sq, smol color){
    u64 row7 = 0x00ff000000000000ULL;
    u64 row2 = 0x000000000000ff00ULL;
    u64 move;

    if(color)
        move = ((1ULL << sq) & row7) >> 16;
    else
        move = ((1ULL << sq) & row2) << 16;

    return move;
}

u64 pawnAttacks(u64 sq, smol color){
    u64 notA = ~0x0101010101010101ULL;
    u64 notH = ~0x8080808080808080ULL;
    u64 moves;


    u64 bb = 1ULL << sq;
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

void generateWhitePawnMoves(Board* b, MoveList* m){
    u64 empty = ~b->allPieces;
    u64 pawns = b->pieces[WP];

    while(pawns){
        smol from = __builtin_ctzll(pawns & -pawns);
        u64 moves = 0;
        u64 caps = 0;

        moves |= pawnSingleMoves(from, White) & empty;
        moves |= pawnDoubleMoves(from, White) & empty & (empty << 8);
        caps |= pawnAttacks(from, White) & b->coloredPieces[Black];

        while(moves){
            smol to = __builtin_ctzll(moves & -moves);
            
            m->moves[m->count++] = Encode_Move(from, to, WP, Empty);

            moves &= moves - 1;
        }

        while(caps){
            smol to = __builtin_ctzll(caps & -caps);
            smol capture = b->pieceArr[to];

            m->moves[m->count++] = Encode_Move(from, to, WP, capture);

            caps &= caps - 1;
        }

        pawns &= pawns - 1;
    }

}

void generateBlackPawnMoves(Board* b, MoveList* m){
    u64 empty = ~b->allPieces;
    u64 pawns = b->pieces[BP];

    while(pawns){
        smol from = __builtin_ctzll(pawns & -pawns);
        u64 moves = 0;
        u64 caps = 0;

        moves |= pawnSingleMoves(from, Black) & empty;
        moves |= pawnDoubleMoves(from, Black) & empty & (empty >> 8);
        caps |= pawnAttacks(from, Black) & b->coloredPieces[White];

        while(moves){
            smol to = __builtin_ctzll(moves & -moves);
            
            m->moves[m->count++] = Encode_Move(from, to, BP, Empty);

            moves &= moves - 1;
        }

        while(caps){
            smol to = __builtin_ctzll(caps & -caps);
            smol capture = b->pieceArr[to];

            m->moves[m->count++] = Encode_Move(from, to, BP, capture);

            caps &= caps - 1;
        }

        pawns &= pawns - 1;
    }
}

u64 rookMoves(Board* b, smol sq){
    u64 moves = 0;
    smol color = (1ULL << sq) & b->coloredPieces[Black] ? 1 : 0;
    
    u64 sameColorBB = b->coloredPieces[color];
    u64 otherColorBB = b->coloredPieces[!color];
    
    for(int i = 1; i <= 7 - (sq % 8); i++){ //East
        u64 target = 1ULL << (sq + i);
        if(target & sameColorBB){
            break;
        }
        moves |= target;
        
        if(target & otherColorBB){
            break;
        }
    }
    
    for(int i = 1; i <= (sq % 8); i++){ //West
        u64 target = 1ULL << (sq - i);
        if(target & sameColorBB){
            break;
        }
        moves |= target;
        
        if(target & otherColorBB){
            break;
        }
    }

    for(int i = 1; i <= 7 - (sq / 8); i++){ //North
        u64 target = 1ULL << (sq + 8*i);
        if(target & sameColorBB){
            break;
        }
        moves |= target;
        
        if(target & otherColorBB){
            break;
        }
    }

    for(int i = 1; i <= (sq / 8); i++){ //South
        u64 target = 1ULL << (sq - 8*i);
        if(target & sameColorBB){
            break;
        }
        moves |= target;
        
        if(target & otherColorBB){
            break;
        }
    }

    return moves;
}

u64 bishopMoves(Board* b, smol sq){
    u64 moves = 0;

    smol color = b->coloredPieces[Black] & (1ULL << sq) ? Black : White;

    u64 SameColorBB = b->coloredPieces[color];
    u64 OtherColorBB = b->coloredPieces[!color];

    smol rank = sq/8;
    smol file = sq%8;

    smol NE = (7-rank) > (7-file) ? 7-file : 7-rank;
    smol NW = (7-rank) > (file) ? file : 7-rank;
    smol SW = (rank) > (file) ? file : rank;
    smol SE = (rank) > (7-file) ? 7-file : rank;

    for(int i = 1; i <= NE; i++){ // NorthEast
        smol target = sq + i*9;
        
        if(((1ULL << target) & SameColorBB)){
            break;
        }
        
        moves |= 1ULL << target;

        if((1ULL << target) & OtherColorBB){
            break;
        }
    }

    for(int i = 1; i <= NW; i++){ // NorthWest
        smol target = sq + i*7;
        
        if(((1ULL << target) & SameColorBB)){
            break;
        }
        
        moves |= 1ULL << target;

        if((1ULL << target) & OtherColorBB){
            break;
        }
    }

    for(int i = 1; i <= SW; i++){ // SouthWest
        smol target = sq - i*9;
        
        if(((1ULL << target) & SameColorBB)){
            break;
        }
        
        moves |= 1ULL << target;

        if((1ULL << target) & OtherColorBB){
            break;
        }
    }

    for(int i = 1; i <= SE; i++){ // SouthEast
        smol target = sq - i*7;
        
        if(((1ULL << target) & SameColorBB)){
            break;
        }
        
        moves |= 1ULL << target;

        if((1ULL << target) & OtherColorBB){
            break;
        }
    }

    return moves;
}

u64 queenMoves(Board* b, smol sq){
    return bishopMoves(b, sq) | rookMoves(b, sq);
}

void generateMoves(Board* b, MoveList* m){
    generateWhiteKnightMoves(b, m);
    generateWhiteKingMoves(b, m);
    
    generateBlackKnightMoves(b, m);
    generateBlackKingMoves(b, m);
}

#endif