typedef struct{
    move move;
    int score;
} scoredMove;

smol positionRepeats(Board* b){
    smol stop = b->ply - b->halfMoveClock;
    stop = stop >= 0 ? stop : 0;
    for(int i = b->ply - 2; i >= stop; i -= 2){
        if(b->hash == b->history[i].hash){
            return 1;
        }
    }
    return 0;
}

int searchHelp(Board* b, int alpha, int beta, smol depth){ // fix all leaf nodes generating moves at some point. Also need to add upper and lower bound to tt tab
    if(b->halfMoveClock >= 100)
        return 0;
    if(positionRepeats(b))
        return 0;
    
    TranspositionTableItem ttState = get(&b->transpositionTable, b->hash);
    if(ttState.depth >= depth){
        return ttState.score; 
    }
    int best = INT32_MIN;
    
    MoveList m;
    generateMoves(b, &m);
    smol pruned = 0;

    scoredMove scoredMoves[m.count];
    
    
    if(m.count == 0){
        if(squareAttacked(b, __builtin_ctzll(b->pieces[WK + 6 * b->turn]), b->turn))
        return b->turn ? -100000 : 100000;
        else
        return 0;
    }
    
    if(depth == 0)
        return evaluate(b);
    
    for(int i = 0; i < m.count; i++){
        scoredMove scored;
        scored.move = m.moves[i];
        
        smol piece = (m.moves[i] >> 12) & 15;
        smol capturedPiece = (m.moves[i] >> 16) & 15;
        
        scored.score = 0;
        if(capturedPiece){
            scored.score = 10 * pieceValue[(capturedPiece - 1) % 6] - pieceValue[(piece - 1) % 6];
        }
        scoredMoves[i] = scored;
    }
    
    if(b->turn == White){
        for(int i = 0; i < m.count; i++){
            smol bestMoveIndex = 0;
            for(int j = 1; j < m.count; j++){
                if(scoredMoves[j].score > scoredMoves[bestMoveIndex].score){
                    bestMoveIndex = j;
                }
            }
            scoredMoves[bestMoveIndex].score = INT32_MIN;
            scoredMove bestMove = scoredMoves[bestMoveIndex];

            b->ply++;
            makeMove(b, bestMove.move);
    
            int result = searchHelp(b, alpha, beta, depth - 1);
            if(result > best)
                best = result;
            if(result > alpha)
                alpha = result;
            
            undoMove(b, bestMove.move);
            b->ply--;

            if(alpha >= beta){
                pruned = 1;
                break;
            }
        }
    }
    else{
        best = INT32_MAX;
        for(int i = 0; i < m.count; i++){ // fix this to use best index
            smol bestMoveIndex = 0;
            for(int j = 1; j < m.count; j++){
                if(scoredMoves[j].score > scoredMoves[bestMoveIndex].score){
                    bestMoveIndex = j;
                }
            }
            scoredMoves[bestMoveIndex].score = INT32_MIN;
            scoredMove bestMove = scoredMoves[bestMoveIndex];
   
            b->ply++;
            makeMove(b, bestMove.move);
    
            int result = searchHelp(b, alpha, beta, depth - 1);
            if(result < best)
                best = result;
            if(result < beta)
                beta = result;
            
            undoMove(b, bestMove.move);
            b->ply--;
    
            if(beta <= alpha){
                pruned = 1;
                break;
            }
        }
    }

    if(pruned == 0){
        TranspositionTableItem ttItem = {.score = best, .depth = depth};
        put(&b->transpositionTable, b->hash, ttItem);
    }

    return best;
}

move search(Board* b, smol depth){
    int best = INT32_MIN;
    move bestMove = 0;

    if(b->halfMoveClock >= 100)
        return NO_MOVE;
    
    MoveList m;
    generateMoves(b, &m);

    if(m.count == 0){
        if(squareAttacked(b, __builtin_ctzll(b->pieces[WK + 6 * b->turn]), b->turn))
            return b->turn ? NO_MOVE : NO_MOVE;
        else
            return NO_MOVE;
    }

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
