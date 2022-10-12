#include "Piece.h"


void PrintColorAndType(Piece piece) {
    switch (piece.color)
    {
    case white:
        printf("white ");
        break;
    case black:
        printf("black ");
        break;
    case no_color:
        printf("no_color ");
        break;
    default:
        printf("unknown_color ");
        break;
    }

    switch (piece.type)
    {
    case king:
        printf("king");
        break;
    case queen:
        printf("queen");
        break;
    case bishop:
        printf("bishop");
        break;
    case knight:
        printf("knight");
        break;
    case rook:
        printf("rook");
        break;
    case pawn:
        printf("pawn");
        break;
    case empty:
        printf("empty");
        break;
    default:
        printf("unknown_type ");
        break;
    }
}

Bool PieceCompare(Piece piece1, Piece piece2) {
    if ((piece1.type == piece2.type) && (piece1.color == piece2.color)) {
        return True;
    }
    else {
        return False;
    }
}