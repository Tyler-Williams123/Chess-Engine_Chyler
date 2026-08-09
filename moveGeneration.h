#ifndef Move_Generation_H
#define Move_Generation_H

#include "defs.h"
#include "board.h"

typedef struct{
    move moves[256];
    smol count;
}MoveList;

u64 knightTargets(smol sq);

void generateWhiteKnightMoves(Board* b, MoveList* m);

void generateBlackKnightMoves(Board* b, MoveList* m);
// kinghts

u64 kingTargets(smol sq);

void generateWhiteKingMoves(Board* b, MoveList* m);

void generateBlackKingMoves(Board* b, MoveList* m);
// kings

u64 pawnSingleMoves(smol sq, smol color);

u64 pawnDoubleMoves(smol sq, smol color);

u64 pawnAttacks(u64 sq, smol color);

void generateWhitePawnMoves(Board* b, MoveList* m);

void generateBlackPawnMoves(Board* b, MoveList* m);
// pawns

u64 rookTargets(Board* b, smol sq);

void generateWhiteRookMoves(Board* b, MoveList* m);

void generateBlackRookMoves(Board* b, MoveList* m);
// rooks

u64 bishopTargets(Board* b, smol sq);

void generateWhiteBishopMoves(Board* b, MoveList* m);

void generateBlackBishopMoves(Board* b, MoveList* m);
// bishops

u64 queenTargets(Board* b, smol sq);

void generateWhiteQueenMoves(Board* b, MoveList* m);

void generateBlackQueenMoves(Board* b, MoveList* m);
// queens

smol squareAttacked(Board* b, smol sq, smol color); // color is king color

void generateWhiteCastle(Board* b, MoveList* m);

void generateBlackCastle(Board* b, MoveList* m);
// castling

void generateMoves(Board* b, MoveList* m); // generates legal moves only

smol hasLegalMove(Board* b);

void generateCaptures(Board* b, MoveList* m);

#endif