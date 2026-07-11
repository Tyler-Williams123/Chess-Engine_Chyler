int searchHelp(Board* b, smol depth){
    if(depth == 0)
        return evaluate(b);

    int best = INT32_MIN;

    MoveList m;
    generateMoves(b, &m);

    for(int i = 0; i < m.count; i++){
        b->ply++;
        makeMove(b, m.moves[i]);

        int result = -searchHelp(b, depth - 1);
        if(result > best){
            best = result;
        }

        undoMove(b, m.moves[i]);
        b->ply--;
    }

    return best;
}

move search(Board* b, smol depth){
    int best = INT32_MIN;
    move bestMove = 0;

    MoveList m;
    generateMoves(b, &m);

    for(int i = 0; i < m.count; i++){
        b->ply++;
        makeMove(b, m.moves[i]);

        int result = -searchHelp(b, depth - 1);
        if(result > best){
            best = result;
            bestMove = m.moves[i];
        }

        undoMove(b, m.moves[i]);
        b->ply--;
    }

    return bestMove;
}
