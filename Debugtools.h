#ifndef DEBUGTOOLS_H
#define DEBUGTOOLS_H

#include "defs.h"
#include "board.h"
#include "moveGeneration.h"
#include "search.h"

void generateFen(Board* b);

void printMove(move m);

smol verifyBoard(Board* board);

smol compareBoard(Board* b1, Board* b2);

u64 perft(smol depth, Board* b);

typedef struct{
    u64 bitboards[12];
    smol sideToMove;
    smol castling;
    smol enPessant;

    char FEN[100];
} DataEntry;

void generateEvalNNBinaryData(smol depth, Board* b, DataEntry* data);

u64 perftDevide(smol depth, Board* b, ZobristHash* z);

#endif