#pragma once

#include <stdlib.h>
#include <stdint.h>

typedef unsigned long long u64;
typedef unsigned char smol;

#define NO_MOVE 0

extern int pieceValue[6];

typedef uint32_t move; // bits 0-5 are from, 6-11 are to, 12-15 are for piece and color and type
                       // 16-19 are captured piece and color, 20-23 for promotion piece and color
                       //24-31 for flags : 24-en pessant, 25-castle 26-Double pawn push

extern int pieceSQTable[8][64];

extern u64 State;

void seedRandom(u64 seed);

u64 rand64();

enum {UPPER, LOWER, EXACT};

typedef struct{
    int score;
    move bestMove;
    signed char depth;
    smol flag;
} TranspositionTableItem;

typedef struct{
    u64 key;
    TranspositionTableItem tableItem;
}node;

typedef struct{
    node* data;
    u64 size;
}Dictionary;

void dictInit(Dictionary* d, smol size); //size is exponent 2^n size is n

u64 dictHash(Dictionary* d, u64 key);

TranspositionTableItem get(Dictionary* d, u64 key);

void put(Dictionary* d, u64 key, TranspositionTableItem item);