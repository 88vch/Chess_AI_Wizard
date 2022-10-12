/* ai.c file */
#include <stdlib.h>
#include "aiZach.h"

int boardEvaluation(Board* board) {
    int kingValue         = 40;
    int queenValue        =  9;
    int rookValue         =  5;
    int bishopKnightValue =  3;
    int pawnValue         =  1;

    int whiteTotal = 0, blackTotal = 0;
    int file, rank;

    for (rank = one; rank <= eight; rank++) {
        for (file = a; file <= h; file++) {
            if (board->grid[file][rank].color == white) {
                if (board->grid[file][rank].type == king) {
                    whiteTotal += kingValue;
                }
                else if (board->grid[file][rank].type == queen) {
                    whiteTotal += queenValue;
                }
                else if (board->grid[file][rank].type == rook) {
                    whiteTotal += rookValue;
                }
                else if (board->grid[file][rank].type == bishop || board->grid[file][rank].type == knight) {
                    whiteTotal += bishopKnightValue;
                }
                else if (board->grid[file][rank].type == pawn) {
                    whiteTotal += pawnValue;
                }
            }
            else if (board->grid[file][rank].color == black) {
                if (board->grid[file][rank].type == king) {
                    blackTotal += kingValue;
                }
                else if (board->grid[file][rank].type == queen) {
                    blackTotal += queenValue;
                }
                else if (board->grid[file][rank].type == rook) {
                    blackTotal += rookValue;
                }
                else if (board->grid[file][rank].type == bishop || board->grid[file][rank].type == knight) {
                    blackTotal += bishopKnightValue;
                }
                else if (board->grid[file][rank].type == pawn) {
                    blackTotal += pawnValue;
                }
            }
        }
    }
    return (whiteTotal - blackTotal);
}

/* NOTE: CHECK FOR MEMORY LEAKAGE */
BoardList createBoardList(Board* board, Color teamColor) {
    BoardList listOfBoards;

    MoveList iMoves = GetAllMoves(teamColor, board);
    listOfBoards.size = iMoves.size;
    listOfBoards.list = malloc(iMoves.size* sizeof(board));
    for (int i = 0; i < listOfBoards.size; i++) {
        listOfBoards.list[i] = CloneBoardV2(board);
        SmartMovePiece(iMoves.list[i].iPos, iMoves.list[i].fPos, listOfBoards.list[i], queen);
    }
    free(iMoves.list);
    return listOfBoards;
}

/* NOTE: CHECK FOR MEMORY LEAKAGE */
int minimax(Board* board, int depth, Color teamColor) {
    int i = 0;
    double eval, minEval, maxEval;

    BoardList listOfBoards = createBoardList(board, teamColor);

    /* if depth == 0 or game over in board 
 *           NOTE: game over in board implies a boardEvaluation favoring opposite teamColor
 *                */
    if (depth == 0 || CheckForCheckMate(LocateKing(teamColor, board), board)) {
        return boardEvaluation(board);
    }

    if (teamColor == white) {
        maxEval = -1000000;
        for (i = 0; i < listOfBoards.size; i++) {
            eval = minimax(listOfBoards.list+i, depth - 1, black);
            maxEval = (maxEval > eval) ? maxEval : eval;
        }
        free(listOfBoards.list);
        return maxEval;
    }
    else {
        minEval = 1000000;
        for (i = 0; i < listOfBoards.size; i++) {
            eval = minimax(listOfBoards.list+i, depth - 1, white);
            minEval = (minEval < eval) ? minEval : eval;
        free(listOfBoards.list);
        return minEval;
        }
    }
}

MinimaxBoardList createMinimaxBoardList(Board* board, int depth, Color teamColor) {
    int i;

    MinimaxBoardList listOfMinimaxBoards;

    MoveList iMoves = GetAllMoves(teamColor, board);
    listOfMinimaxBoards.size = iMoves.size;
    listOfMinimaxBoards.list = malloc(iMoves.size* sizeof(MinimaxBoard));
    for (int i = 0; i < listOfMinimaxBoards.size; i++) {
        listOfMinimaxBoards.list[i].board = CloneBoard(board);
        listOfMinimaxBoards.list[i].move = iMoves.list[i];
        SmartMovePiece(iMoves.list[i].iPos, iMoves.list[i].fPos, listOfMinimaxBoards.list[i].board, queen);
        listOfMinimaxBoards.list[i].minimaxScore = minimax(board, depth - 1, teamColor);
    }
    free(iMoves.list);

    return listOfMinimaxBoards;
}

/* updated function for creatingBestAIMove */
Move createBestMove(Board *board, int depth, Color teamColor) {
    int i, maxMinimaxScore;

    MinimaxBoardList minimaxBoardList;
    Move finalMove;     /* what will be returned */
    Move bestMove;

    minimaxBoardList = createMinimaxBoardList(board, depth - 1, teamColor);

    /* find max/min for white/black */
    if (teamColor == white) {
        maxMinimaxScore = -1000000;
        for (i = 0; i < minimaxBoardList.size; i++) {
            if (maxMinimaxScore < (minimaxBoardList + i*sizeof(MinimaxBoardList)).score) {
                maxMinimaxScore = minimaxBoardList + i.score;
                bestMove = minimaxBoardList + i.move;
            }
        }
    }
    else if (teamColor == black) {
        maxMinimaxScore = 1000000;
        for (i = 0; i < minimaxBoardList.size; i++) {
            if (maxMinimaxScore > minimaxBoardList + i.score) {
                maxMinimaxScore = minimaxBoardList + i.score;
                bestMove = minimaxBoardList + i.move;
            }
        }
    }

    finalMove = bestMove;
    for (i = 0; i < minimaxBoardList.size; i++) {
        free(minimaxBoardList.list + i->board);
    }
    free(minimaxBoardList.list);
    return finalMove;
}

/* initial thought process for creatingBestAIMove

Move createBestMove(Board* board, int depth, Color teamColor) {
    int File, Rank;
    MinimaxMove bestMove; 

    bestMove.minimax = minimax(board, depth, teamColor);
        to get move;
           1. create BoardList for depth-1 moves (nextMoves)
           2. using a for loop which iterates through size of BoardList(w/depth-1), 
                evaluate whether or not minimax at depth-1 == bestMove.minimax
                2a. if so, using a for loop which iterates through every single index in a board (8x8),
                        if (piece in nextMoves) != (piece in board[curentBoard]) and (piece in nextMoves) != empty
                2b. set move finalPosition = piece board file & rank
                        and return the minimaxMove struct

    BoardList nextMoves = createBoardList(board);
    for (int i = 0; i < nextMoves.size; i++) {
        if (minimax(nextMoves.list[i], depth - 1, teamColor) == bestMove.minimax) {
            for (File = a; File <= h; File++) {
                for (Rank = one; Rank <= eight; Rank++) {
                    if ((nextMoves.list[i]->grid[File][Rank] != board->grid[File][Rank]) && nextMoves.) {
                    }
                }
            }
        }
    }
}
*/


