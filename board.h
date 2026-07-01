#ifndef board_H
#define board_H

typedef unsigned char smol;
typedef unsigned long long u64;

typedef uint32_t move; // bits 0-5 are from, 6-11 are to, 12-15 are for piece and color and type
                       // 16-19 are captured piece and color, 20-23 for promotion piece and color
                       //24-31 for flags : 24-en pessant, 25-castle 26-Double pawn push

typedef struct{
    u64 EnPessant;
    smol castleRights; // bit 0-WSC 1-WLC 2-BSC 3-BLC
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
    A8, B8, C8, D8, E8, F8, G8, H8,
};

enum {White, Black};

enum {Empty, WP, WN, WB, WR, WQ, WK, BP, BN, BB, BR, BQ, BK};

typedef struct{
    u64 pieces[13];
    u64 coloredPieces[2];
    u64 allPieces;

    u64 enPessant;
    smol castleRights; // bit 0-WSC 1-WLC 2-BSC 3-BLC

    smol pieceArr[64];
    smol turn;

    Undo history[256];
    smol ply;
} Board;

void FENInit(Board* board, char* FEN){
    char pieces[128];
    char turn[2];
    char castling[8];
    char enPessant[4];
    smol count = 0;;

    smol field = 0;
    
    smol i = 0; // split string
    while(FEN[i] != '\0'){
        if(FEN[i] == ' '){
            i++;
            switch(field){
                case(0): pieces[count] = '\0'; break;
                case(2): castling[count] = '\0'; break;
            }
            
            field++;
            count = 0;
            continue;
        }
        switch(field){
            case(0): pieces[count++] = FEN[i]; break;
            case(1): turn[count++] = FEN[i]; break;
            case(2): castling[count++] = FEN[i]; break;
            case(3): enPessant[count++] = FEN[i]; break;
        }
        i++;
    }
    enPessant[count] = '\0';
    turn[1] = '\0';
    
    board->turn = turn[0] == 'w' ? White : Black; // turn
    
    i = 0;
    while(castling[i] != '\0'){ // castling
        switch(castling[i]){
            case('K'): board->castleRights |= 1ULL; break;
            case('Q'): board->castleRights |= (1ULL << 1); break;
            case('k'): board->castleRights |= (1ULL << 2); break;
            case('q'): board->castleRights |= (1ULL << 3); break;
        }
        i++;
    }
    
    i = 0;
    smol rank = 0; // En Pessant
    smol file = 0;
    board->enPessant = 0;
    if(enPessant[0] != '-'){
        rank = enPessant[0] - 'a';
        file = (enPessant[1] - '1') * 8;
        board->enPessant = 1ULL << (rank + file);
    }

    i = 0; // pieces
    count = A8;
    while(pieces[i] != '\0'){
        char c = pieces[i];
        switch(c){
            case('/'): count -= 16; break;
            case('p'): board->pieceArr[count++] = BP; break;
            case('n'): board->pieceArr[count++] = BN; break;
            case('b'): board->pieceArr[count++] = BB; break;
            case('r'): board->pieceArr[count++] = BR; break;
            case('q'): board->pieceArr[count++] = BQ; break;
            case('k'): board->pieceArr[count++] = BK; break;

            case('P'): board->pieceArr[count++] = WP; break;
            case('N'): board->pieceArr[count++] = WN; break;
            case('B'): board->pieceArr[count++] = WB; break;
            case('R'): board->pieceArr[count++] = WR; break;
            case('Q'): board->pieceArr[count++] = WQ; break;
            case('K'): board->pieceArr[count++] = WK; break;

            default:
                smol space = c - '0';
                for(int j = 0; j < space; j++){
                    board->pieceArr[count++] = Empty;
                } break;
        }
        i++;
    }
    for(int j = A1; j <= H8; j++){
        board->pieces[board->pieceArr[j]] = board->pieces[board->pieceArr[j]] | (1ULL << j);
    }
    for(int j = WP; j <= WK; j++){
        board->coloredPieces[White] |= board->pieces[j];
    }
    for(int j = BP; j <= BK; j++){
        board->coloredPieces[Black] |= board->pieces[j];
    }
    board->allPieces = board->coloredPieces[White] | board->coloredPieces[Black];
}

void initBoard(Board* board){
    board->turn = 0;
    board->enPessant = 0;
    board->castleRights = 0xf;

    board->pieces[WP] = 0x000000000000ff00;
    board->pieces[WR] = 0x0000000000000081;
    board->pieces[WN] = 0x0000000000000042;
    board->pieces[WB] = 0x0000000000000024;
    board->pieces[WK] = 0x0000000000000010;
    board->pieces[WQ] = 0x0000000000000008;

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

void verifyBoard(Board* board){
    u64 pieces[13] = {0};
    u64 white = 0;
    u64 black = 0;
    u64 all = 0;

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
        }
    }
    if(white != board->coloredPieces[White])
        printf("Mismatch in white pieces bitboard \n");
    if(black != board->coloredPieces[Black])
        printf("Mismatch in black pieces bitboard \n");
    if(all != board->allPieces)
        printf("Mismatch in all pieces bitboard \n");
}

void makeMove(Board* b, move m){
    Undo state;
    state.EnPessant = b->enPessant;
    state.castleRights = b->castleRights;
    b->history[b->ply] = state;

    smol piece = (m >> 12) & 15;
    smol color = piece > 6 ? Black : White;
    
    smol from = m & 0x3f;
    smol to = (m >> 6) & 0x3f;

    smol cPiece = (m >> 16) & 15;
    smol pPiece = (m >> 20) & 15;
    
    smol enPessant = (m >> 24) & 1;
    smol castle = (m >> 25) & 1;
    smol DPP = (m >> 26) & 1;

    b->pieceArr[to] = b->pieceArr[from];
    b->pieceArr[from] = Empty;

    b->enPessant = 0;
    if(piece == WR || piece == BR){
        switch(from){
            case(A1): b->castleRights &= 0b1101; break;
            case(H1): b->castleRights &= 0b1110; break;
            case(A8): b->castleRights &= 0b0111; break;
            case(H8): b->castleRights &= 0b1011; break;
        }
    }
    switch(piece){
        case(WK): b->castleRights &= 0b1100; break;
        case(BK): b->castleRights &= 0b0011; break;
    }
    
    if(DPP){
        smol EPSQ = color ? to + 8 : to - 8;
        b->enPessant = 1ULL << (EPSQ);
    }
    else if(castle){
        smol rook;
        smol rFrom;
        smol rTo;
        switch(to){
            case(C1): rook = WR; rFrom = A1; rTo = D1; break;
            case(G1): rook = WR; rFrom = H1; rTo = F1; break;
            case(C8): rook = BR; rFrom = A8; rTo = D8; break;
            case(G8): rook = BR; rFrom = H8; rTo = F8; break;
        }
        
        b->pieceArr[rTo] = b->pieceArr[rFrom];
        b->pieceArr[rFrom] = Empty;

        b->coloredPieces[color] &= ~(1ULL << rFrom); // old location
        b->pieces[rook] &= ~(1ULL << rFrom);
        b->allPieces &= ~(1ULL << rFrom);

        b->coloredPieces[color] |= (1ULL << rTo); // new location
        b->pieces[rook] |= (1ULL << rTo);
        b->allPieces |= 1ULL << rTo;
    }
    else if(enPessant){
        smol EPCaptureSQ = color ? to + 8 : to - 8;
        
        smol cColor = cPiece > 6 ? Black : White;
        b->coloredPieces[cColor] &= ~(1ULL << EPCaptureSQ);
        b->pieces[cPiece] &= ~(1ULL << EPCaptureSQ);
        b->allPieces &= ~(1ULL << EPCaptureSQ);
        
        b->pieceArr[EPCaptureSQ] = Empty;   
    }
    else if(cPiece != Empty){
        smol cColor = cPiece > 6 ? Black : White;
        b->coloredPieces[cColor] &= ~(1ULL << to);
        b->pieces[cPiece] &= ~(1ULL << to);
        b->allPieces &= ~(1ULL << to);
        
        switch(to){
            case(A1): b->castleRights &= 0b1101; break;
            case(H1): b->castleRights &= 0b1110; break;
            case(A8): b->castleRights &= 0b0111; break;
            case(H8): b->castleRights &= 0b1011; break;
        }
    }

    b->coloredPieces[color] &= ~(1ULL << from); // old location
    b->pieces[piece] &= ~(1ULL << from);
    b->allPieces &= ~(1ULL << from);
    
    if(pPiece){
        piece = pPiece;
        b->pieceArr[to] = pPiece;
    }

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
    
    smol enPessant = (m >> 24) & 1;
    smol castle = (m >> 25) & 1;
    smol DPP = (m >> 26) & 1;
    
    smol cPiece = (m >> 16) & 15;
    smol pPiece = (m >> 20) & 15;
    
    b->pieceArr[from] = b->pieceArr[to];
    b->pieceArr[to] = cPiece;
    
    b->allPieces |= 1ULL << from;
    b->pieces[piece] |= 1ULL << from;
    b->coloredPieces[color] |= 1ULL << from;
    
    if(pPiece){
        b->pieceArr[from] = piece;
        piece = pPiece;
    }
    
    b->allPieces &= ~(1ULL << to);
    b->pieces[piece] &= ~(1ULL << to);
    b->coloredPieces[color] &= ~(1ULL << to);
    if(castle){
        smol rook;
        smol rFrom;
        smol rTo;
        switch(to){
            case(C1): rook = WR; rFrom = A1; rTo = D1; break;
            case(G1): rook = WR; rFrom = H1; rTo = F1; break;
            case(C8): rook = BR; rFrom = A8; rTo = D8; break;
            case(G8): rook = BR; rFrom = H8; rTo = F8; break;
        }
        
        b->pieceArr[rFrom] = b->pieceArr[rTo];
        b->pieceArr[rTo] = Empty;

        b->coloredPieces[color] &= ~(1ULL << rTo); // new location
        b->pieces[rook] &= ~(1ULL << rTo);
        b->allPieces &= ~(1ULL << rTo);

        b->coloredPieces[color] |= (1ULL << rFrom); // old location
        b->pieces[rook] |= (1ULL << rFrom);
        b->allPieces |= 1ULL << rFrom;
    }
    else if(enPessant){
        smol EPCaptureSQ = color ? to + 8 : to - 8;
        
        b->pieceArr[to] = Empty;
        b->pieceArr[EPCaptureSQ] = cPiece;
        
        smol cColor = cPiece > 6 ? Black : White;
        b->allPieces |= 1ULL << EPCaptureSQ;
        b->pieces[cPiece] |= 1ULL << EPCaptureSQ;
        b->coloredPieces[cColor] |= 1ULL << EPCaptureSQ;
    }
    else if(cPiece != Empty){
        smol cColor = cPiece > 6 ? Black : White;
        b->allPieces |= 1ULL << to;
        b->pieces[cPiece] |= 1ULL << to;
        b->coloredPieces[cColor] |= 1ULL << to;
    }

    b->castleRights = b->history[b->ply].castleRights;
    b->enPessant = b->history[b->ply].EnPessant;
    
    b->turn--;
}

#endif