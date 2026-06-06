#ifndef Move_Generation_H
#define Move_Generation_H
#include "board.h"

#define Encode_Move(from, to, piece, capturedPiece) (from | (to << 6) | (piece << 12) | (capturedPiece << 16))

typedef struct{
    move moves[256];
    smol count;
}MoveList;

const char* moveToString(move m, char* b){
    smol from = m & 0x3f;
    smol to = (m >> 6) & 0x3f;

    char file = 'a' + (from % 8);
    char rank = '1' + (from / 8);
    b[0] = file;
    b[1] = rank;

    file = 'a' + (to % 8);
    rank = '1' + (to / 8);
    b[2] = file;
    b[3] = rank;

    b[4] = '\0';
}

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

    while(wKnights){
        u64 knight = wKnights & -wKnights;
        smol from = __builtin_ctzll(knight);
        u64 moves = knightTargets(from) & ~b->coloredPieces[White];

        while(moves){
            smol to = __builtin_ctzll(moves);
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

// kinghts

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

// kings

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

// pawns

u64 rookTargets(Board* b, smol sq){
    u64 moves = 0;
    smol color = (1ULL << sq) & b->coloredPieces[Black] ? Black : White;
    
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

void generateWhiteRookMoves(Board* b, MoveList* m){
    u64 rooks = b->pieces[WR];
    while(rooks){
        smol from = __builtin_ctzll(rooks & -rooks);

        u64 moves = rookTargets(b, from);
        while(moves){
            smol to = __builtin_ctzll(moves & -moves);
            smol capture = b->pieceArr[to];
            m->moves[m->count++] = Encode_Move(from, to, WR, capture);

            moves &= moves - 1;
        }

        rooks &= rooks - 1;
    }
}

void generateBlackRookMoves(Board* b, MoveList* m){
    u64 rooks = b->pieces[BR];
    while(rooks){
        smol from = __builtin_ctzll(rooks & -rooks);

        u64 moves = rookTargets(b, from);
        while(moves){
            smol to = __builtin_ctzll(moves & -moves);
            smol capture = b->pieceArr[to];
            m->moves[m->count++] = Encode_Move(from, to, BR, capture);

            moves &= moves - 1;
        }

        rooks &= rooks - 1;
    }
}

// rooks

u64 bishopTargets(Board* b, smol sq){
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

void generateWhiteBishopMoves(Board* b, MoveList* m){
    u64 bishops = b->pieces[WB];
    while(bishops){
        smol from = __builtin_ctzll(bishops & -bishops);

        u64 moves = bishopTargets(b, from);
        while(moves){
            smol to = __builtin_ctzll(moves & -moves);
            smol capture = b->pieceArr[to];
            m->moves[m->count++] = Encode_Move(from, to, WB, capture);

            moves &= moves - 1;
        }

        bishops &= bishops - 1;
    }
}

void generateBlackBishopMoves(Board* b, MoveList* m){
    u64 bishops = b->pieces[BB];
    while(bishops){
        smol from = __builtin_ctzll(bishops & -bishops);

        u64 moves = bishopTargets(b, from);
        while(moves){
            smol to = __builtin_ctzll(moves & -moves);
            smol capture = b->pieceArr[to];
            m->moves[m->count++] = Encode_Move(from, to, BB, capture);

            moves &= moves - 1;
        }

        bishops &= bishops - 1;
    }
}

// bishops

u64 queenTargets(Board* b, smol sq){
    return bishopTargets(b, sq) | rookTargets(b, sq);
}

void generateWhiteQueenMoves(Board* b, MoveList* m){
    u64 queens = b->pieces[WQ];
    while(queens){
        smol from = __builtin_ctzll(queens & -queens);

        u64 moves = queenTargets(b, from);
        while(moves){
            smol to = __builtin_ctzll(moves & -moves);
            smol capture = b->pieceArr[to];
            m->moves[m->count++] = Encode_Move(from, to, WQ, capture);

            moves &= moves - 1;
        }

        queens &= queens - 1;
    }
}

void generateBlackQueenMoves(Board* b, MoveList* m){
    u64 queens = b->pieces[BQ];
    while(queens){
        smol from = __builtin_ctzll(queens & -queens);

        u64 moves = queenTargets(b, from);
        while(moves){
            smol to = __builtin_ctzll(moves & -moves);
            smol capture = b->pieceArr[to];
            m->moves[m->count++] = Encode_Move(from, to, BQ, capture);

            moves &= moves - 1;
        }

        queens &= queens - 1;
    }
}

// queens

smol squareAttacked(Board* b, smol sq, smol color){ // color is king color
    if(color){
        if(pawnAttacks(sq, Black) & b->pieces[WP])
            return 1;
        if(knightTargets(sq) & b->pieces[WN])
            return 1;
        if(rookTargets(b, sq) & b->pieces[WR])
            return 1;
        if(bishopTargets(b, sq) & b->pieces[WB])
            return 1;
        if(queenTargets(b, sq) & b->pieces[WQ])
            return 1;
        if(kingTargets(sq) & b->pieces[WK])
            return 1;
    }
    else{
        if(pawnAttacks(sq, White) & b->pieces[BP])
            return 1;
        if(knightTargets(sq) & b->pieces[BN])
            return 1;
        if(rookTargets(b, sq) & b->pieces[BR])
            return 1;
        if(bishopTargets(b, sq) & b->pieces[BB])
            return 1;
        if(queenTargets(b, sq) & b->pieces[BQ])
            return 1;
        if(kingTargets(sq) & b->pieces[BK])
            return 1;
    }
    return 0;
}

void generateMoves(Board* b, MoveList* m){ // generates legal moves only
    MoveList pseudo;
    pseudo.count = 0;

    m->count = 0;
    if(b->turn % 2 == White){
        generateWhitePawnMoves(b, &pseudo);
        generateWhiteKingMoves(b, &pseudo);
        generateWhiteKnightMoves(b, &pseudo);
        generateWhiteRookMoves(b, &pseudo);
        generateWhiteBishopMoves(b, &pseudo);
        generateWhiteQueenMoves(b, &pseudo);

        for(int i = 0; i < pseudo.count; i++){
            makeMove(b, pseudo.moves[i]);
            
            if(!squareAttacked(b, __builtin_ctzll(b->pieces[WK]), White)){
                m->moves[m->count++] = pseudo.moves[i];
            }
            undoMove(b, pseudo.moves[i]);
        }
    }
    else{
        generateBlackPawnMoves(b, &pseudo);
        generateBlackKingMoves(b, &pseudo);
        generateBlackKnightMoves(b, &pseudo);
        generateBlackRookMoves(b, &pseudo);
        generateBlackBishopMoves(b, &pseudo);
        generateBlackQueenMoves(b, &pseudo);

        for(int i = 0; i < pseudo.count; i++){
            makeMove(b, pseudo.moves[i]);

            if(!squareAttacked(b, __builtin_ctzll(b->pieces[BK]), Black)){
                m->moves[m->count++] = pseudo.moves[i];
            }
            undoMove(b, pseudo.moves[i]);
        }
    }
}

u64 perft(smol depth, Board* b){
    MoveList moves;
    u64 total = 0;

    if(depth <= 0){
        return 1;
    }

    generateMoves(b, &moves);
    for(int i = 0; i < moves.count; i++){
        makeMove(b, moves.moves[i]);
        total += perft(depth - 1, b);
        undoMove(b, moves.moves[i]);
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
        makeMove(b, moves.moves[i]);

        u64 perf = perft(depth - 1, b);
        total += perf;
        
        char buffer[6];
        moveToString(moves.moves[i], buffer);
        printf("%s %llu\n", buffer, perf);
        
        undoMove(b, moves.moves[i]);
    }
    return total;
}
#endif