#ifndef board_H
#define board_H

typedef unsigned char smol;
typedef unsigned long long u64;

typedef uint32_t move; // bits 0-5 are from, 6-11 are to, 12-15 are for piece and color and type
                       // 16-19 are captured piece and color, 20-23 for promotion piece and color, 24-31 for flags

enum {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
};

enum {
    White, Black
};

enum {
    Pawn, Rook, Knight, Bishop, King, Queen
};

enum {
    Empty, WP, WR, WN, WB, WK, WQ, BP, BR, BN, BB, BK, BQ
};

typedef struct{
    u64 pieces[13];
    u64 coloredPieces[2];
    u64 allPieces;

    smol pieceArr[64];
    smol turn;
} Board;

void initBoard(Board* board){
    board->turn = 0;

    board->pieces[WP] = 0x000000000000ff00;
    board->pieces[WR] = 0x0000000000000081;
    board->pieces[WN] = 0x0000000000000042;
    board->pieces[WB] = 0x0000000000000024;
    board->pieces[WK] = 0x0000000000000008;
    board->pieces[WQ] = 0x0000000000000010;

    board->pieces[BP] = 0x00ff000000000000;
    board->pieces[BR] = 0x8100000000000000;
    board->pieces[BN] = 0x4200000000000000;
    board->pieces[BB] = 0x2400000000000000;
    board->pieces[BK] = 0x1000000000000000;
    board->pieces[BQ] = 0x0800000000000000;

    board->coloredPieces[White] = 0xffff;
    board->coloredPieces[Black] = 0xffff000000000000;
    board->allPieces = 0xffff00000000ffff;

    board->pieceArr[A8] = BR;
    board->pieceArr[B8] = BN;
    board->pieceArr[C8] = BB;
    board->pieceArr[D8] = BQ;
    board->pieceArr[E8] = BK;
    board->pieceArr[F8] = BB;
    board->pieceArr[G8] = BN;
    board->pieceArr[H8] = BR;
    for (int i = A7; i <= H7; i++){
        board->pieceArr[i] = BP;
    }
    for (int i = A3; i <= H6; i++){
        board->pieceArr[i] = Empty;
    }
    for (int i = A2; i <= H2; i++){
        board->pieceArr[i] = WP;
    }
        board->pieceArr[A1] = WR;
        board->pieceArr[B1] = WN;
        board->pieceArr[C1] = WB;
        board->pieceArr[D1] = WQ;
        board->pieceArr[E1] = WK;
        board->pieceArr[F1] = WB;
        board->pieceArr[G1] = WN;
        board->pieceArr[H1] = WR;
}

void printBoard(Board* board){
    for(int rank = 7; rank >= 0; rank--)
    {
        for(int file = 0; file < 8; file++)
        {
            int sq = rank * 8 + file;
            if(board->pieceArr[sq] == Empty)
            printf("-- ");
            else if(board->pieceArr[sq] == BP)
            printf("BP ");
            else if(board->pieceArr[sq] == BR)
            printf("BR ");
            else if(board->pieceArr[sq] == BN)
            printf("BN ");
            else if(board->pieceArr[sq] == BB)
            printf("BB ");
            else if(board->pieceArr[sq] == BK)
            printf("BK ");
            else if(board->pieceArr[sq] == BQ)
            printf("BQ ");
            else if(board->pieceArr[sq] == WP)
            printf("WP ");
            else if(board->pieceArr[sq] == WR)
            printf("WR ");
            else if(board->pieceArr[sq] == WN)
            printf("WN ");
            else if(board->pieceArr[sq] == WB)
            printf("WB ");
            else if(board->pieceArr[sq] == WK)
            printf("WK ");
            else if(board->pieceArr[sq] == WQ)
            printf("WQ ");
        }
        printf("\n");
    }
}

void makeMove(Board* b, move m){
    smol piece = (m >> 12) & 15;
    smol color = piece > 6 ? Black : White;
    
    smol from = m & 0x3f;
    smol to = (m >> 6) & 0x3f;

    smol cPiece = b->pieceArr[to];
    
    b->pieceArr[to] = b->pieceArr[from];
    b->pieceArr[from] = Empty;

    if(cPiece != 0){
        smol cColor = cPiece > 6 ? Black : White;
        b->coloredPieces[cColor] &= ~(1ULL << to);
        b->pieces[cPiece] &= ~(1ULL << to);
        b->allPieces &= ~(1ULL << to);
    }

    b->coloredPieces[color] &= ~(1ULL << from); // old location
    b->pieces[piece] &= ~(1ULL << from);
    b->allPieces &= ~(1ULL << from);
    
    b->coloredPieces[color] |= (1ULL << to); // new location
    b->pieces[piece] |= (1ULL << to);
    b->allPieces |= 1ULL << to;

    b->turn++;
}

void undoMove(Board* b, move m){
    smol piece = (m >> 12) & 15;
    smol color = piece > 6 ? Black : White;
    
    smol from = m & 0x3f;
    smol to = (m >> 6) & 0x3f;

    smol cPiece = (m >> 16) & 15;
    smol cColor = cPiece > 6 ? Black : White;

    b->pieceArr[from] = b->pieceArr[to];
    b->pieceArr[to] = cPiece;

    b->allPieces |= 1ULL << from;
    b->pieces[piece] |= 1ULL << from;
    b->coloredPieces[color] |= 1ULL << from;

    b->allPieces &= ~(1ULL << to);
    b->pieces[piece] &= ~(1ULL << to);
    b->coloredPieces[color] &= ~(1ULL << to);
    
    if(cPiece != 0){
        b->allPieces |= 1ULL << to;
        b->pieces[cPiece] |= 1ULL << to;
        b->coloredPieces[cColor] |= 1ULL << to;
    }

    b->turn--;
}

#endif