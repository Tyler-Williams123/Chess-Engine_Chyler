void generateFen(Board* b){
    char FEN[80];
    smol count = 0;
    for(int rank = 7; rank >= 0; rank--)
    {
        int blanks = 0;
        for(int file = 0; file < 8; file++)
        {
            int sq = rank * 8 + file;
            switch(b->pieceArr[sq]){
                case(Empty): blanks++; if(file == 7){FEN[count++] = '0' + blanks; blanks = 0;} break;
                case(WP): if(blanks > 0){FEN[count++] = '0' + blanks; blanks = 0;} FEN[count++] = 'P'; break;
                case(WN): if(blanks > 0){FEN[count++] = '0' + blanks; blanks = 0;} FEN[count++] = 'N'; break;
                case(WB): if(blanks > 0){FEN[count++] = '0' + blanks; blanks = 0;} FEN[count++] = 'B'; break;
                case(WR): if(blanks > 0){FEN[count++] = '0' + blanks; blanks = 0;} FEN[count++] = 'R'; break;
                case(WQ): if(blanks > 0){FEN[count++] = '0' + blanks; blanks = 0;} FEN[count++] = 'Q'; break;
                case(WK): if(blanks > 0){FEN[count++] = '0' + blanks; blanks = 0;} FEN[count++] = 'K'; break;
                case(BP): if(blanks > 0){FEN[count++] = '0' + blanks; blanks = 0;} FEN[count++] = 'p'; break;
                case(BN): if(blanks > 0){FEN[count++] = '0' + blanks; blanks = 0;} FEN[count++] = 'n'; break;
                case(BB): if(blanks > 0){FEN[count++] = '0' + blanks; blanks = 0;} FEN[count++] = 'b'; break;
                case(BR): if(blanks > 0){FEN[count++] = '0' + blanks; blanks = 0;} FEN[count++] = 'r'; break;
                case(BQ): if(blanks > 0){FEN[count++] = '0' + blanks; blanks = 0;} FEN[count++] = 'q'; break;
                case(BK): if(blanks > 0){FEN[count++] = '0' + blanks; blanks = 0;} FEN[count++] = 'k'; break;
            }
        }
        if(rank != 0)
            FEN[count++] = '/';
    }
    FEN[count++] = ' ';
    FEN[count++] = b->turn ? 'b' : 'w';
    FEN[count++] = ' ';
    
    if(b->castleRights & 1) FEN[count++] = 'K';
    if((b->castleRights >> 1) & 1) FEN[count++] = 'Q';
    if((b->castleRights >> 2) & 1) FEN[count++] = 'k';
    if((b->castleRights >> 3) & 1) FEN[count++] = 'q';
    
    FEN[count++] = ' ';
    if(!b->enPessant){
        FEN[count++] = '-';
    }
    else{
        smol EPSQ = __builtin_ctzll(b->enPessant);
        smol file = EPSQ % 8;
        smol rank = (EPSQ / 8);

        FEN[count++] = 'a' + file;
        FEN[count++] = '1' + rank;
    }
    FEN[count++] = ' ';
    FEN[count++] = '0';
    FEN[count++] = ' ';
    FEN[count++] = '1';
    FEN[count++] = '\0';

    printf("%s\n", FEN);
}

void printMove(move m){
    smol piece = (m >> 12) & 15;
    
    smol from = m & 0x3f;
    smol to = (m >> 6) & 0x3f;

    smol cPiece = (m >> 16) & 15;
    smol pPiece = (m >> 20) & 15;
    
    smol enPessant = (m >> 24) & 1;
    smol castle = (m >> 25) & 1;
    smol DPP = (m >> 26) & 1;

    printf("piece: %d\nfrom: %d\nto: %d\ncaptured piece: %d\npromotion Piece: %d\nen pessant: %d\ncastle: %d\ndouble pawn push: %d\n", piece, from, to, cPiece, pPiece, enPessant, castle, DPP);
}

smol verifyBoard(Board* board){ 
    u64 pieces[13] = {0};
    u64 white = 0;
    u64 black = 0;
    u64 all = 0;

    smol correct = 1;

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
            correct = 0;
        }
    }
    if(white != board->coloredPieces[White]){
        printf("Mismatch in white pieces bitboard \n");
        correct = 0;
    }
    if(black != board->coloredPieces[Black]){
        printf("Mismatch in black pieces bitboard \n");
        correct = 0;
    }
    if(all != board->allPieces){
        printf("Mismatch in all pieces bitboard \n");
        correct = 0;
    }

    return correct;
}

smol compareBoard(Board* b1, Board* b2){ // b1 is true board b2 is old board
    for(int i  = WP; i <= BK; i++){
        if (b2->pieces[i] != b1->pieces[i]){
            return 0;
        }
    }

    if(b2->coloredPieces[White] != b1->coloredPieces[White]){
        printf("colored piece bitboards mismatch");
        return 0;
    }
    if(b2->coloredPieces[Black] != b1->coloredPieces[Black]) return 0;

    if(b2->allPieces != b1->allPieces) return 0;

    if(b2->enPessant != b1->enPessant) return 0;
    if(b2->castleRights != b1->castleRights) return 0;
    if(b2->turn != b1->turn) return 0;
    
    for(int i = A1; i <= H8; i++){
        if(b2->pieceArr[i] != b1->pieceArr[i]) return 0;
    }

    if(b2->ply != b1->ply) return 0;
    if(memcmp(b2->history, b1->history, sizeof(Undo) * (b1->ply + 1)) != 0) return 0;
    return 1;
}

u64 perft(smol depth, Board* b){
    MoveList moves;
    u64 total = 0;

    if(depth <= 0){
        return 1;
    }

    generateMoves(b, &moves);
    for(int i = 0; i < moves.count; i++){
        b->ply++;
        makeMove(b, moves.moves[i]);
        total += perft(depth - 1, b);
        undoMove(b, moves.moves[i]);
        b->ply--;
    }
    return total;
}

u64 perftDevide(smol depth, Board* b, ZobristHash* z){
    MoveList moves;
    u64 total = 0;

    if(depth <= 0){
        return 1;
    }

    generateMoves(b, &moves);
    for(int i = 0; i < moves.count; i++){
        b->ply++;
        makeMove(b, moves.moves[i]);
        
        u64 perf = perft(depth - 1, b);
        total += perf;
        
        char buffer[6];
        moveToString(moves.moves[i], buffer);
        printf("%s %llu\n", buffer, perf);
        
        undoMove(b, moves.moves[i]);
        b->ply--;
    }

    return total;
}