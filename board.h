#ifndef board_H
#define board_H

#include "defs.h"

#include <string.h>
#include <stdio.h>

#define Encode_Move(from, to, piece, capturedPiece, promotionPiece, flag) (from | (to << 6) | (piece << 12) | (capturedPiece << 16) | (promotionPiece << 20) | (flag << 24))

typedef struct{
    u64 EnPessant;
    smol castleRights; // bit 0-WSC 1-WLC 2-BSC 3-BLC
    u64 hash;
    smol halfMoveClock;
} Undo;

enum{WSC, WLC, BSC, BLC};

enum {None, EnPessant = 1, Castle = 2, DoublePP = 4};

enum {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8, NO_SQ = -15
};

enum {White, Black};

enum {Empty, WP, WN, WB, WR, WQ, WK, BP, BN, BB, BR, BQ, BK};

enum {WPHash = 0, WNHash = (WN - 1) * 64, WBHash = (WB - 1) * 64, WRHash = (WR - 1) * 64, WQHash = (WQ - 1) * 64, WKHash = (WK - 1) * 64,
      BPHash = (BP - 1) * 64, BNHash = (BN - 1) * 64, BBHash = (BB - 1) * 64, BRHash = (BR - 1) * 64, BQHash = (BQ - 1) * 64, BKHash = (BK - 1) * 64,
      BlackMoveHash = 768,
      WKCastleHash, WQCastleHash, BKCastleHash, BQCastleHash,
      AHash, HHash = AHash + 7};

typedef struct{
    u64 randoms[HHash + 1];
    Dictionary table;
}ZobristHash;

typedef struct{
    u64 pieces[13];
    u64 coloredPieces[2];
    u64 allPieces;

    u64 enPessant;
    smol castleRights; // bit 0-WSC 1-WLC 2-BSC 3-BLC

    smol pieceArr[64];

    ZobristHash zobristHash;
    u64 hash;

    Dictionary transpositionTable;
    move killerMoves[256][2];
    int historyHueristic[64][64];

    Undo history[256];
    smol ply;
    
    smol turn;
    smol halfMoveClock;
    int scores[2];
} Board;

move parseMove(char* move, Board* b);

void initZobrist(ZobristHash* zobrist);

u64 zobrist(Board* b);

int fullEvaluate(Board* b);

int evaluate(Board* b);

void FENInit(Board* board, char* FEN); // half move clock is just set to zero

void initBoard(Board* board);

void printBoard(Board* board);

void makeMove(Board* b, move m);

void undoMove(Board* b, move m);

#endif