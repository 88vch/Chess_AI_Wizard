#ifndef PIECE_H_
#define PIECE_H_
#include <stdio.h>

typedef enum Piece_type {
    king,
    queen,
    bishop,
    knight,
    rook,
    pawn,
    empty
} Piece_type;

typedef enum Color {
    black,
    white,
    no_color
} Color;

typedef enum Bool {
    False,
    True
} Bool;

typedef struct Piece {
    Piece_type type;
    Color color;
} Piece;

void PrintColorAndType(Piece piece);

Bool PieceCompare(Piece piece1, Piece piece2);

#endif