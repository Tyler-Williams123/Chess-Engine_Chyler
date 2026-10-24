#include <stdlib.h>
#include <stdint.h>

#include "defs.h"
#include "board.h"
#include "moveGeneration.h"
#include "search.h"
#include "Timer.h"

smol stopSearch = 0;

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

int quiescence(Board* b, int alpha, int beta){ // add back ttTables
    if(timeUp()){
        stopSearch = 1;
        return 0;
    }
    if(b->halfMoveClock >= 100)
        return 0;
    if(positionRepeats(b))
        return 0;
    
    int standPat = evaluate(b);
        if(b->turn == White){
            if(standPat >= beta) return standPat; 
            if(standPat > alpha) alpha = standPat;
        }
        else{ 
            if(standPat <= alpha) return standPat;
            if(standPat < beta) beta = standPat;
        }

    int best = standPat;

    MoveList m;
    if(!hasLegalMove(b)){
        if(squareAttacked(b, __builtin_ctzll(b->pieces[WK + 6 * b->turn]), b->turn))
            return b->turn ? -100000 + b->ply : 100000 - b->ply;
        else
            return 0;
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
            if(stopSearch){
                undoMove(b, bestMove.move);
                b->ply--;
                return 0;
            }
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
            if(stopSearch){
                undoMove(b, bestMove.move);
                b->ply--;
                return 0;
            }
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

int searchHelp(Board* b, int alpha, int beta, smol depth){
    if(timeUp()){
        stopSearch = 1;
        return 0;
    }

    if(b->halfMoveClock >= 100)
        return 0;
    if(positionRepeats(b))
        return 0;
    
    int best = INT32_MIN;
    int chosenMove = NO_MOVE;

    if(!hasLegalMove(b)){
        if(squareAttacked(b, __builtin_ctzll(b->pieces[WK + 6 * b->turn]), b->turn))
            return b->turn ? -MATE_SCORE + b->ply : MATE_SCORE - b->ply;
        else
            return 0;
    }
    
    if(depth == 0)
        return quiescence(b, alpha, beta);
    
    TranspositionTableItem ttState = get(&b->transpositionTable, b->hash);
    if(ttState.depth >= depth){
        if(ttState.flag == EXACT)
            return ttState.score; 
        if(ttState.flag == LOWER)
            alpha = ttState.score > alpha ? ttState.score : alpha;
        else
            beta = ttState.score < beta ? ttState.score : beta;
        if(alpha >= beta){
            return ttState.score;
        }
    }

    int alphaUsed = alpha;
    int betaUsed = beta;

    MoveList m;
    generateMoves(b, &m);
    
    scoredMove scoredMoves[m.count];
    
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

    for(int i = 0; i < m.count; i++){
        int max = scoredMoves[i].score;
        int bestIndex = i;
        for(int j = i; j < m.count; j++){
            if(max < scoredMoves[j].score){
                max = scoredMoves[j].score;
                bestIndex = j;
            }
        }
        scoredMove temp = scoredMoves[i];
        scoredMoves[i] = scoredMoves[bestIndex];
        scoredMoves[bestIndex] = temp;
    }
    
    if(b->turn == White){
        for(int i = 0; i < m.count; i++){
            scoredMove bestMove = scoredMoves[i];
            
            b->ply++;
            makeMove(b, bestMove.move);
    
            int result = searchHelp(b, alpha, beta, depth - 1);
            if(stopSearch){
                undoMove(b, bestMove.move);
                b->ply--;
                return 0;
            }

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
                break;
            }
        }
    }
    else{
        best = INT32_MAX;
        for(int i = 0; i < m.count; i++){
            scoredMove bestMove = scoredMoves[i];
   
            b->ply++;
            makeMove(b, bestMove.move);
    
            int result = searchHelp(b, alpha, beta, depth - 1);
            if(stopSearch){
                undoMove(b, bestMove.move);
                b->ply--;
                return 0;
            }
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
                break;
            }
        }
    }
        smol flag = best <= alphaUsed ? UPPER
          : best >= betaUsed  ? LOWER
          :                     EXACT;
        TranspositionTableItem ttItem = {.score = best, .bestMove = chosenMove, .flag = flag, .depth = depth};
        put(&b->transpositionTable, b->hash, ttItem);

    return best;
}

move iterativeDeepen(Board* b, smol depth, move previousMove){ // add alpha beta pruning
    
    int best = INT32_MIN;
    move bestMove = NO_MOVE;

    if(b->halfMoveClock >= 100)
        return NO_MOVE;

    MoveList m;
    generateMoves(b, &m);

    if(m.count == 0)
        return NO_MOVE;

    if(timeUp()){
        if(previousMove == NO_MOVE)
            return m.moves[0];
        else
            return previousMove;
    }

    scoredMove scoredMoves[m.count];
    TranspositionTableItem ttState = get(&b->transpositionTable, b->hash);

    for(int i = 0; i < m.count; i++){
        scoredMove scored;
        scored.move = m.moves[i];
        
        smol piece = (m.moves[i] >> 12) & 15;
        smol capturedPiece = (m.moves[i] >> 16) & 15;
        
        scored.score = 0;
        if(scored.move == previousMove){
            scored.score = 100000;
        }
        else if(ttState.bestMove == scored.move){
            scored.score = 99999;
        }
        else if(capturedPiece){
            scored.score = 5000 + 10 * pieceValue[(capturedPiece - 1) % 6] - pieceValue[(piece - 1) % 6];
        }
        else{
            smol from = scored.move & 0x3f;
            smol to = (scored.move >> 6) & 0x3f;
            scored.score = b->historyHueristic[from][to];
        }

        scoredMoves[i] = scored;
    }

    for(int i = 0; i < m.count; i++){
        int max = scoredMoves[i].score;
        int bestIndex = i;
        for(int j = i; j < m.count; j++){
            if(max < scoredMoves[j].score){
                max = scoredMoves[j].score;
                bestIndex = j;
            }
        }
        scoredMove temp = scoredMoves[i];
        scoredMoves[i] = scoredMoves[bestIndex];
        scoredMoves[bestIndex] = temp;
    }

    int alpha = INT32_MIN;
    int beta = INT32_MAX;

    if(b->turn == White){
        for(int i = 0; i < m.count; i++){
            b->ply++;
            makeMove(b, scoredMoves[i].move);
            
            int result = searchHelp(b, alpha, beta, depth - 1);
            if(stopSearch){
                undoMove(b, scoredMoves[i].move);
                b->ply--;
                return previousMove;
            }
            if(result > best){
                best = result;
                bestMove = scoredMoves[i].move;
                if(result > alpha)
                    alpha = result;
            }
    
            undoMove(b, scoredMoves[i].move);
            b->ply--;
        }
    }
    else{
        best = INT32_MAX;
        for(int i = 0; i < m.count; i++){
            b->ply++;
            makeMove(b, scoredMoves[i].move);
    
            int result = searchHelp(b, alpha, beta, depth - 1);
            if(stopSearch){
                undoMove(b, scoredMoves[i].move);
                b->ply--;
                return 0;
            }
            if(result < best){
                best = result;
                bestMove = scoredMoves[i].move;
                if(result < beta)
                    beta = result;
            }
    
            undoMove(b, scoredMoves[i].move);
            b->ply--;
        }
    }

    return bestMove;
}

move search(Board* b, smol depth){
    move bestMove = NO_MOVE;
    stopSearch = 0;

    if(b->halfMoveClock >= 100)
        return NO_MOVE;
    
    MoveList m;
    generateMoves(b, &m);

    if(m.count == 0)
        return NO_MOVE;

    for(int i = 1; i <= depth; i++){
        bestMove = iterativeDeepen(b, i, bestMove);
    }
            
    return bestMove;
}
