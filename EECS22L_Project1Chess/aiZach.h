/* ai header file */
#ifndef AIZACH_H
#define AIZACH_H

#include "Board.h"

typedef struct BoardList {
    Board* list;
    int size;
} BoardList;

typedef struct MinimaxBoard {
    Board *board;
    Move move; //move that it took to get it to current board
    int minimaxScore;
} MinimaxBoard;

typedef struct MinimaxBoardList {
    MinimaxBoard *list;
    int size;
} MinimaxBoardList;


/* return an int calculating the evaluation of the board(whiteTotal - blackTotal) */
int boardEvaluation(Board* board);

/* dynamically create a board */
BoardList createBoardList(Board* board, Color teamColor);

/* returns an int of the best evaluation of current board */
int minimax(Board *board, int depth, Color teamColor);

/* dynamically create a MinimaxBoardList structure and defines every MinimaxBoard */
MinimaxBoardList createMinimaxBoardList(Board* board, int depth, Color teamColor);

/* updated function for creating best AI move */
Move createBestMove(Board *board, int depth, Color teamColor);

#endif
