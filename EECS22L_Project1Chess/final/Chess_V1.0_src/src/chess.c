#include "Board.h"
#include <stdlib.h>
#include <stdio.h>
#include "Piece.h"
#include "string.h"
#include "gui.h"



Board *MovePieces(int oldmove[2], int newmove[2], Board *subBoard)
{
    int errorcode;
    //int endgame;


    Position iPos, fPos;
    iPos.file = oldmove[0];
    iPos.rank = oldmove[1];


    fPos.file = newmove[0];
    fPos.rank = newmove[1];


    Piece_type promotedType;

    promotedType = queen;


    if ( (Rules(iPos, fPos, subBoard) == 0) && (CastleOrPromotionOrEnPessant(iPos, fPos, subBoard) == 1))
    {       
        /* pawn promotion is occuring */


           errorcode = SmartMovePiece(iPos, fPos, subBoard, promotedType);
           printf("promotedType is %d  \n", promotedType);
           printf("errorcode is %d", errorcode);
           if (errorcode != 0) 
        { 
            printf("move did not go through!\nerrorcode: %d\n", errorcode);
        }
    }else {
            errorcode = SmartMovePiece(iPos, fPos, subBoard, empty);
            if (errorcode != 0) 
        { 
            printf("move did not go through!\nerrorcode: %d\n", errorcode);
        }
    }








    return errorcode;
}