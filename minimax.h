#ifndef MINIMAX_H
#define MINIMAX_H

#include "board.h"
#include "global.h"

#define MAX_MOVES (ARRAY_BOARD_SIZE * ARRAY_BOARD_SIZE)
#define TIME_LIMIT 15.0  // Maximum time per move in seconds

// Structure for moves with evaluation scores
typedef struct {
    Move move;
    int evaluation;
} ScoredMove;


int dynamicDepth(Position gamePosition, char playerColor);
Move alphaBetaMove(Position gamePosition, char playerColor);
int compareMovesDesc(const void *a, const void *b);
int compareMovesAsc(const void *a, const void *b);
int maxValue(Position gamePosition, int depth, int depthMax, char playerColor, Move *bestMove, int alpha, int beta, clock_t startTime);
int minValue(Position gamePosition, int depth, int depthMax, char playerColor, Move *bestMove, int alpha, int beta, clock_t startTime);
int evaluate(Position gamePosition, char playerColor);
void *searchThread(void *args);

#endif /* MINIMAX_H */