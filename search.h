#ifndef SEARCH_H
#define SEARCH_H

#include "defs.h"
#include "board.h"

typedef struct{
    move move;
    int score;
} scoredMove;

smol positionRepeats(Board* b);

int quiescence(Board* b, int alpha, int beta); // add back ttTables fix draw by no legal moves checking

int searchHelp(Board* b, int alpha, int beta, smol depth); // fix all leaf nodes generating moves at some point. fix move ordering loop

move search(Board* b, smol depth);

#endif