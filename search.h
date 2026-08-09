#ifndef SEARCH_H
#define SEARCH_H

#include "defs.h"
#include "board.h"

#define MATE_SCORE 100000

typedef struct{
    move move;
    int score;
} scoredMove;

smol positionRepeats(Board* b);

int quiescence(Board* b, int alpha, int beta);

int searchHelp(Board* b, int alpha, int beta, smol depth);

move iterativeDeepen(Board* b, smol depth, move previousMove);

move search(Board* b, smol depth);

#endif