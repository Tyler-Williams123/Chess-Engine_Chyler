#include <stdio.h>
#include <stdlib.h>

enum {WPHash = 0, WNHash = (WN - 1) * 64, WBHash = (WB - 1) * 64, WRHash = (WR - 1) * 64, WQHash = (WQ - 1) * 64, WKHash = (WK - 1) * 64,
      BPHash = (BP - 1) * 64, BNHash = (BN - 1) * 64, BBHash = (BB - 1) * 64, BRHash = (BR - 1) * 64, BQHash = (BQ - 1) * 64, BKHash = (BK - 1) * 64,
      BlackMoveHash = 768,
      WKCastleHash, WQCastleHash, BKCastleHash, BQCastleHash,
      AHash, HHash = AHash + 7};


typedef struct{
    u64 randoms[HHash + 1];
    Dictionary table;
}ZobristHash;

void initZobrist(ZobristHash* zobrist){
    for(int i = 0; i <= HHash; i++){
        zobrist->randoms[rand64()];
    }
    dictInit(&zobrist->table, 20);
}

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

u64 zobrist(Board* b, ZobristHash* z){ // need to fix for phantom en Pessant;
    u64 hash = 0;
    for(int sq = A1; sq <= H8; sq++){
        smol piece = b->pieceArr[sq];
        switch(piece){
            case(WP): hash ^= z->randoms[WPHash + sq]; break;
            case(WN): hash ^= z->randoms[WNHash + sq]; break;
            case(WB): hash ^= z->randoms[WBHash + sq]; break;
            case(WR): hash ^= z->randoms[WRHash + sq]; break;
            case(WQ): hash ^= z->randoms[WQHash + sq]; break;
            case(WK): hash ^= z->randoms[WKHash + sq]; break;
            case(BP): hash ^= z->randoms[BPHash + sq]; break;
            case(BN): hash ^= z->randoms[BNHash + sq]; break;
            case(BB): hash ^= z->randoms[BBHash + sq]; break;
            case(BR): hash ^= z->randoms[BRHash + sq]; break;
            case(BQ): hash ^= z->randoms[BQHash + sq]; break;
            case(BK): hash ^= z->randoms[BKHash + sq]; break;
        }
    }

    hash ^= b->turn ? z->randoms[BlackMoveHash] : 0;
    
    hash ^= b->castleRights & 1 ? z->randoms[WKCastleHash] : 0;
    hash ^= (b->castleRights >> 1) & 1 ? z->randoms[WQCastleHash] : 0;
    hash ^= (b->castleRights >> 2) & 1 ? z->randoms[BKCastleHash] : 0;
    hash ^= (b->castleRights >> 3) & 1 ? z->randoms[BQCastleHash] : 0;

    hash ^= b->enPessant != 0 ? z->randoms[__builtin_ctzll(b->enPessant) % 8 + AHash] : 0;

    return hash;
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

    if(b2->coloredPieces[White] != b1->coloredPieces[White]) return 0;
    if(b2->coloredPieces[Black] != b1->coloredPieces[Black]) return 0;

    if(b2->allPieces != b1->allPieces) return 0;

    if(b2->enPessant != b1->enPessant) return 0;
    if(b2->castleRights != b1->castleRights) return 0;
    if(b2->turn != b1->turn) return 0;
    
    for(int i = A1; i <= H8; i++){
        if(b2->pieceArr[i] != b1->pieceArr[i]) return 0;
    }

    if(b2->ply != b1->ply) return 0;
    if(memcmp(b2, b1, sizeof(Undo) * (b1->ply + 1)) != 0) return 0;
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
        //debug
        Board before = *b;
        // debug
        b->ply++;
        makeMove(b, moves.moves[i]);
        
        u64 perf = perft(depth - 1, b);
        total += perf;
        
        char buffer[6];
        moveToString(moves.moves[i], buffer);
        printf("%s %llu/", buffer, perf);
        
        undoMove(b, moves.moves[i]);
        b->ply--;

        // debugging
        smol verify = compareBoard(&before, b);
        if(!verify){
            verifyBoard(b);
            char debugMove[6];
            moveToString(moves.moves[i], debugMove);
            printf("broke at ply: %d by move: %s\n", b->ply, debugMove, moves.moves[i]);
            printMove(moves.moves[i]);
        }
        assert(verify);
        //debugging
    }

    return total;
}