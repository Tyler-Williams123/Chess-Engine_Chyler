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

int searchHelp(Board* b, int alpha, int beta, smol depth){ // fix all leaf nodes generating moves at some point
    if(b->halfMoveClock >= 100)
        return 0;
    if(positionRepeats(b))
        return 0;
        
    int best = INT32_MIN;
    
    MoveList m;
    generateMoves(b, &m);
    
    if(m.count == 0){
        if(squareAttacked(b, __builtin_ctzll(b->pieces[WK + 6 * b->turn]), b->turn))
        return b->turn ? -100000 : 100000;
        else
        return 0;
    }
    
    if(depth == 0)
        return evaluate(b);
    
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
