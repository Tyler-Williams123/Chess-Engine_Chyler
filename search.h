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
