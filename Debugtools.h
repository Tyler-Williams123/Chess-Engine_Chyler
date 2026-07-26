#ifndef DEBUGTOOLS_H
#define DEBUGTOOLS_H

void generateFen(Board* b, char* FEN);

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

void generateEvalNNBinaryData(smol depth, Board* b, DataEntry* data, int* count);

u64 perftDevide(smol depth, Board* b, ZobristHash* z);

#endif