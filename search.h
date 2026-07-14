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
        zobrist->randoms[i] = rand64();
    }
    dictInit(&zobrist->table, 20);
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

int searchHelp(Board* b, int alpha, int beta, smol depth){
    if(depth == 0)
        return evaluate(b);

    int best = INT32_MIN;
    
    MoveList m;
    generateMoves(b, &m);
    
    if(b->turn == White){
        for(int i = 0; i < m.count; i++){
            b->ply++;
            makeMove(b, m.moves[i]);
    
            int result = searchHelp(b, alpha, beta, depth - 1);
            if(result > best)
                best = result;
            if(result > alpha)
                alpha = result;
            
            undoMove(b, m.moves[i]);
            b->ply--;

            if(alpha >= beta)
                break;
        }
    }
    else{
        best = INT32_MAX;
        for(int i = 0; i < m.count; i++){
            b->ply++;
            makeMove(b, m.moves[i]);
    
            int result = searchHelp(b, alpha, beta, depth - 1);
            if(result < best)
                best = result;
            if(result < beta)
                beta = result;
            
            undoMove(b, m.moves[i]);
            b->ply--;
    
            if(beta <= alpha)
                break;
        }
    }
    return best;
}

move search(Board* b, smol depth){
    int best = INT32_MIN;
    move bestMove = 0;

    MoveList m;
    generateMoves(b, &m);

    if(b->turn == White){
        for(int i = 0; i < m.count; i++){
            b->ply++;
            makeMove(b, m.moves[i]);
    
            int result = searchHelp(b, INT32_MIN, INT32_MAX, depth - 1);
            if(result > best){
                best = result;
                bestMove = m.moves[i];
            }
    
            undoMove(b, m.moves[i]);
            b->ply--;
        }
    }
    else{
        best = INT32_MAX;
        for(int i = 0; i < m.count; i++){
            b->ply++;
            makeMove(b, m.moves[i]);
    
            int result = searchHelp(b, INT32_MIN, INT32_MAX, depth - 1);
            if(result < best){
                best = result;
                bestMove = m.moves[i];
            }
    
            undoMove(b, m.moves[i]);
            b->ply--;
        }
    }

    return bestMove;
}
