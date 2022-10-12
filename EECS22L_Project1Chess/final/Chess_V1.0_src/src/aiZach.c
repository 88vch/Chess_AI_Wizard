/* ai.c file */
#include "aiZach.h"
#include "stdlib.h"
#include "stdio.h"

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
    listOfBoards.list = malloc(iMoves.size* sizeof(Board));
    for (int i = 0; i < listOfBoards.size; i++) {
        listOfBoards.list[i] = CloneBoardV2(board);
        SmartMovePiece(iMoves.list[i].iPos, iMoves.list[i].fPos, listOfBoards.list+i, queen);
    }
    free(iMoves.list);
    return listOfBoards;
}

/* NOTE: CHECK FOR MEMORY LEAKAGE */
int minimax(Board* board, int depth, Color teamColor) {
    int i = 0;
    int minEval, maxEval;
    static int eval;

    //printf("calling createBoardList\n");
    BoardList listOfBoards = createBoardList(board, teamColor);
    //printf("finished calling createBoardlist\n");
    /* if depth == 0 or game over in board 
 *           NOTE: game over in board implies a boardEvaluation favoring opposite teamColor
 *                */
    if (depth == 0 || CheckForCheckMate(LocateKing(teamColor, board), board)) {
        return boardEvaluation(board); //used to say return static boardEvaluation(board);
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
    MoveList iMoves;
    MinimaxBoardList listOfMinimaxBoards;
    //printf("\nusing GetAllMoves()...\n");
    iMoves = GetAllMoves(teamColor, board);
    //printf("done using GetAllMoves()\n");
    listOfMinimaxBoards.size = iMoves.size;
    //printf("iMoves size: %d\n", iMoves.size);
    listOfMinimaxBoards.list = malloc(iMoves.size * sizeof(MinimaxBoard));
    for (int i = 0; i < listOfMinimaxBoards.size; i++) {
        //printf("index of for-loop of createMinimaxBoardList: %d\n", i);
        listOfMinimaxBoards.list[i].board = CloneBoard(board);
        //printf("Step 1\n");
        listOfMinimaxBoards.list[i].move = iMoves.list[i];
        //printf("Step 2\n");
        SmartMovePiece(iMoves.list[i].iPos, iMoves.list[i].fPos, listOfMinimaxBoards.list[i].board, queen);
        //printf("calling minimax()\n");
        listOfMinimaxBoards.list[i].minimaxScore = minimax(board, depth - 1, teamColor);
        //printf("exited minimax()\n");
    }
    free(iMoves.list);
    //printf("exiting createMinimaxBoard()...\n");
    return listOfMinimaxBoards;
}

/* updated function for creatingBestAIMove */
Move createBestMove(Board *board, int depth, Color teamColor) {
    int i, maxMinimaxScore;

    MinimaxBoardList minimaxBoardList;
    Move finalMove;     /* what will be returned */
    Move bestMove;
    //printf("\ncreating minimaxboardlist ...\n");
    minimaxBoardList = createMinimaxBoardList(board, depth, teamColor);
    //printf("done making minimaxboardlist....\n");
    /* find max/min for white/black */
    if (teamColor == white) {
        maxMinimaxScore = -1000000;
        for (i = 0; i < minimaxBoardList.size; i++) {
            if (maxMinimaxScore < minimaxBoardList.list[i].minimaxScore) {
                maxMinimaxScore = minimaxBoardList.list[i].minimaxScore;
                bestMove = minimaxBoardList.list[i].move;
            }
        }
    }
    else if (teamColor == black) {
        maxMinimaxScore = 1000000;
        for (i = 0; i < minimaxBoardList.size; i++) {
            if (maxMinimaxScore > minimaxBoardList.list[i].minimaxScore) {
                maxMinimaxScore = minimaxBoardList.list[i].minimaxScore;
                bestMove = minimaxBoardList.list[i].move;
            }
        }
    }

    finalMove = bestMove;
    for (i = 0; i < minimaxBoardList.size; i++) {
        free(minimaxBoardList.list[i].board);
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
*/


