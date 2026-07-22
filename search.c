#include <stdlib.h>

#include "defs.h"
#include "board.h"
#include "moveGeneration.h"
#include "search.h"

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

int quiescence(Board* b, int alpha, int beta){ // add back ttTables fix draw by no legal moves checking
    if(b->halfMoveClock >= 100)
        return 0;
    if(positionRepeats(b))
        return 0;
    
    int standPat = evaluate(b);
    if(standPat > alpha)
        alpha = standPat;
    if(standPat < beta)
        beta = standPat;
    if(alpha >= beta)
        return standPat;

    int best = INT32_MIN;
    
    MoveList m;
    if(squareAttacked(b, __builtin_ctzll(b->pieces[WK + 6 * b->turn]), b->turn)){
        generateMoves(b, &m);
        if(m.count == 0){
            return b->turn ? -100000 + b->ply : 100000 - b->ply;
        }
    }
    else{
        generateCaptures(b, &m);
        if(m.count == 0){
            return standPat;
        }
    }
    
    scoredMove scoredMoves[m.count];
    
    for(int i = 0; i < m.count; i++){
        scoredMove scored;
        scored.move = m.moves[i];
        
        smol piece = (m.moves[i] >> 12) & 15;
        smol capturedPiece = (m.moves[i] >> 16) & 15;
        
        scored.score = 5000 + 10 * pieceValue[(capturedPiece - 1) % 6] - pieceValue[(piece - 1) % 6];
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
    
            int result = quiescence(b, alpha, beta);
            if(result > best){
                best = result;
            }
            if(result > alpha)
                alpha = result;
            
            undoMove(b, bestMove.move);
            b->ply--;

            if(alpha >= beta){
                break;
            }
        }
    }
    else{
        best = INT32_MAX;
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
    
            int result = quiescence(b, alpha, beta);
            if(result < best){
                best = result;
            }
            if(result < beta)
                beta = result;
            
            undoMove(b, bestMove.move);
            b->ply--;
    
            if(beta <= alpha){
                break;
            }
        }
    }

    return best;
}

int searchHelp(Board* b, int alpha, int beta, smol depth){ // fix all leaf nodes generating moves at some point. fix move ordering loop
    if(b->halfMoveClock >= 100)
        return 0;
    if(positionRepeats(b))
        return 0;
    
    TranspositionTableItem ttState = get(&b->transpositionTable, b->hash);
    if(ttState.depth >= depth){
        if(ttState.flag == EXACT)
            return ttState.score; 
        if(ttState.flag == LOWER)
            alpha = ttState.score > alpha ? ttState.score : alpha;
        else
            beta = ttState.score < beta ? ttState.score : beta;
    }
    if(alpha >= beta){
        return ttState.score;
    }
    int best = INT32_MIN;
    int chosenMove = NO_MOVE;
    
    MoveList m;
    generateMoves(b, &m);
    smol flag = EXACT;
    
    scoredMove scoredMoves[m.count];
    
    if(m.count == 0){
        if(squareAttacked(b, __builtin_ctzll(b->pieces[WK + 6 * b->turn]), b->turn))
            return b->turn ? -100000 + b->ply : 100000 - b->ply;
        else
            return 0;
    }

    if(depth == 0)
        return quiescence(b, alpha, beta);
    
    for(int i = 0; i < m.count; i++){
        scoredMove scored;
        scored.move = m.moves[i];
        
        smol piece = (m.moves[i] >> 12) & 15;
        smol capturedPiece = (m.moves[i] >> 16) & 15;
        
        scored.score = 0;
        if(ttState.bestMove == scored.move){
            scored.score = 100000;
        }
        else if(capturedPiece){
            scored.score = 5000 + 10 * pieceValue[(capturedPiece - 1) % 6] - pieceValue[(piece - 1) % 6];
        }
        else if(scored.move == b->killerMoves[depth][0]){
            scored.score= 4000;
        }
        else if(scored.move == b->killerMoves[depth][1]){
            scored.score= 3900;
        }
        else{
            smol from = scored.move & 0x3f;
            smol to = (scored.move >> 6) & 0x3f;
            scored.score = b->historyHueristic[from][to];
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
            if(result > best){
                chosenMove = bestMove.move;
                best = result;
            }
            if(result > alpha)
                alpha = result;
            
            undoMove(b, bestMove.move);
            b->ply--;

            if(alpha >= beta){
                smol capturedPiece = (bestMove.move >> 16) & 15;
                if(capturedPiece == Empty){
                    if(bestMove.move != b->killerMoves[depth][0]){
                        b->killerMoves[depth][1] = b->killerMoves[depth][0];
                        b->killerMoves[depth][0] = bestMove.move;
                    }
    
                    smol from = bestMove.move & 0x3f;
                    smol to = (bestMove.move >> 6) & 0x3f;
                    b->historyHueristic[from][to] += depth * depth;
                }

                flag = LOWER;
                break;
            }
        }
    }
    else{
        best = INT32_MAX;
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
            if(result < best){
                chosenMove = bestMove.move;
                best = result;
            }
            if(result < beta)
                beta = result;
            
            undoMove(b, bestMove.move);
            b->ply--;
    
            if(beta <= alpha){
                smol capturedPiece = (bestMove.move >> 16) & 15;
                if(capturedPiece == Empty){
                    if(bestMove.move != b->killerMoves[depth][0]){
                        b->killerMoves[depth][1] = b->killerMoves[depth][0];
                        b->killerMoves[depth][0] = bestMove.move;
                    }
    
                    smol from = bestMove.move & 0x3f;
                    smol to = (bestMove.move >> 6) & 0x3f;
                    b->historyHueristic[from][to] += depth * depth;
                }

                flag = UPPER;
                break;
            }
        }
    }

        TranspositionTableItem ttItem = {.score = best, .bestMove = chosenMove, .flag = flag, .depth = depth};
        put(&b->transpositionTable, b->hash, ttItem);

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