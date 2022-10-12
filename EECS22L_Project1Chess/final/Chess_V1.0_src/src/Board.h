#ifndef BOARD_H_
#define BOARD_H_
#include "Piece.h"

typedef enum Rank {
    one = 0,
    two = 1,
    three = 2,
    four = 3,   
    five = 4,
    six = 5,
    seven = 6,
    eight = 7,
    unknownRank = 8
} Rank;

typedef enum File {
    a, b, c, d, e, f, g, h, unknownFile
} File;

typedef struct Position {
    Rank rank;
    File file;
} Position;

typedef struct Move {
    Position iPos; //initial position
    Position fPos; //final position
} Move;

typedef struct PositionList {
    Position* list; 
    int size;
} PositionList;

typedef struct MoveList {
    Move* list;
    int size;
} MoveList;

typedef struct Board {
    //eventually have to initialize Board
    //[file][rank]
    Piece grid[8][8];

    /* Way to imagine iterating through board:
        -Start from bottom left of board (position a1)
        -move to the right (position a2)
        -finish iterating through Rank, then move on to next rank (position b1)
        -end in top right corner
    */

    //give us binary value on whether a pawn is up to be 'En Pessant'ed at a file
    //(as in a pawn has JUST moved 2 spaces)
    //[file]
    Bool blackUpForEnPassant[8];
    Bool whiteUpForEnPassant[8];
    
    //tells whether given piece has moved atleast once during game (useful for castling)
    Bool whiteKingMoved;
    Bool blackKingMoved;
    Bool whiteRookMoved[2]; // = {left, right}
    Bool blackRookMoved[2]; // = {left, right}
} Board;

/* Helper Functions */
//helper function for converting a rank and file into a single construct
Position IntToPosition(File file, Rank rank);
//locates king on Board
Position LocateKing(Color color, Board* board);
//checks if Boards are identical (useful for IsDraw() in Game Module)
Bool BoardCompare(Board* board1, Board* board2);
//create a copy of Board
Board* CloneBoard(Board* board1);

//create a copy of board (actual value)
Board CloneBoardV2(Board* board1);

/* Moving Pieces */

//does not check if move is legal, is only helper function for SmartMovePiece()
void DumbMovePiece(Position initialPosition, Position finalPosition, Board* board);

//dont use to test if castling will occur, only use to test for pawn promotion
//use before SmartMovePiece() to test if pawn promotion will occur. If so, set promotedType equal to whatever promoted type
int CastleOrPromotionOrEnPessant(Position initialPosition, Position finalPosition, Board *board);

//smart moving function that checks if move is legal, and also updates information pertaining to castling/enPassant in Board struct. 
//Also returns int for program to handle errors
//set promotedType = empty (if pawn will not be promoted). Must check if pawn will be moving to space, 
int SmartMovePiece(Position initialPosition, Position finalPosition, Board* board, Piece_type promotedType);

/* Defining Piece Rules */

/*
    THINGS TO NOTE:
    (1) When calling a rules() function, we assume that the current selected piece's side's king is not in check
    (2) My Definition of IDEAL move: a move that (assuming we dont care if king will be in check after) is legal
*/

//general function which tells us whether a certain move is allowed
int Rules(Position initialPosition, Position finalPosition, Board* board);

//IDEAL CASE helper functions: ignoring if the king will be in check after the move is made
//assumptions listed above do apply
Bool KingRules(Position initialPosition, Position finalPosition, Board* board);
Bool QueenRules(Position initialPosition, Position finalPosition, Board* board);
Bool BishopRules(Position initialPosition, Position finalPosition, Board* board);
Bool KnightRules(Position initialPosition, Position finalPosition, Board* board);
Bool RookRules(Position initialPosition, Position finalPosition, Board* board);
Bool PawnRules(Position initialPosition, Position finalPosition, Board* board);

/* Retrieving all possible moves */

//returns a Position array of size 64 (for now, could be variable in the future) that lists out all possible moves
//allocates memory that eventually needs to be freed
PositionList getMoves(Position position, Board* board);

//helper functions of IDEAL cases for each specific piece:
Position* getKingMoves(Position position, Board* board); 
Position* getQueenMoves(Position position, Board* board); //not defined
Position* getBishopMoves(Position position, Board* board); //not defined
Position* getKnightMoves(Position position, Board* board); //not defined
Position* getRookMoves(Position position, Board* board); //not defined
Position* getPawnMoves(Position position, Board* board); //not defined

/* Defining special move cases */
//Promotion: When a pawn has reached the enemy side, it gets promoted
void PawnPromotion(Position pawnPosition, Piece_type pieceSelected, Board *board); //not defined

/* checking if king is in check */
Bool CheckForCheck(Position kingPosition, Board *board);

/* checking if king is in checkmate */
Bool CheckForCheckMate(Position kingPosition, Board *board);

/* Creates Starting Board */
Board* StartingBoard();


void PrintFileAndRank(Position pos);

Position SetPosition(char file, char rank);

//returns a block of memory with possible moves of a given a team color
MoveList GetAllMoves(Color teamColor, Board *board);

#endif