#include "Board.h"
#include <stdlib.h>
#include <assert.h>

/** Helper Functions **/

Position IntToPosition(File file, Rank rank) {
    Position pos;
    pos.file = file;
    pos.rank = rank;
    return pos;
}

Position LocateKing(Color color, Board* board) {
    int file;
    int rank;
    Piece searcher;
    Position kingPosition;

    for (rank = one; rank <= eight; rank++) {
        for (file = a; file <= h; file++) {
            searcher = board->grid[file][rank];
            if (searcher.color == color && searcher.type == king) {
                kingPosition.rank = rank;
                kingPosition.file = file;
                return kingPosition;
            }
        }
    }
    //else no king was found, which should not be possible
    //have to put a return statement to not get warnings
    kingPosition.file = a;
    kingPosition.rank = one;
    return kingPosition;
}

Bool CompareBoard(Board* board1, Board* board2) {
    int file;
    int rank;
    Piece searcher1;
    Piece searcher2;

    for (rank = one; rank <= eight; rank++) {
        for (file = a; file <= h; file++) {
            searcher1 = board1->grid[file][rank];
            searcher2 = board2->grid[file][rank];
            if (PieceCompare(searcher1, searcher2) == False) {
                return False;
            }
        }
    }

    //else
    return True;
}

Board* CloneBoard(Board* board1) {
    Board *clone;
    clone = malloc(sizeof(Board));
    assert(clone);
    /* cloning grid */
    int file;
    int rank;
    for (rank = one; rank <= eight; rank++) {
        for (file = a; file <= h; file++) {
            clone->grid[file][rank].type = board1->grid[file][rank].type;
            clone->grid[file][rank].color = board1->grid[file][rank].color;
        }
    }
    /* cloning blackEnPassant & whiteEnPassant */
    for (int pawnNum = 0; pawnNum < 8; pawnNum++) {
        clone->blackUpForEnPassant[pawnNum] = board1->blackUpForEnPassant[pawnNum];
    }
    for (int pawnNum = 0; pawnNum < 8; pawnNum++) {
        clone->whiteUpForEnPassant[pawnNum] = board1->whiteUpForEnPassant[pawnNum];
    }
    /* cloning KingMoved variables */
    clone->blackKingMoved = board1->blackKingMoved;
    clone->whiteKingMoved = board1->whiteKingMoved;
    /* cloning PawnsMoved arrays */
    for (int rookNum = 0; rookNum < 2; rookNum++) {
        clone->blackRookMoved[rookNum] = board1->blackRookMoved[rookNum];
    }
    for (int rookNum = 0; rookNum < 2; rookNum++) {
        clone->whiteRookMoved[rookNum] = board1->whiteRookMoved[rookNum];
    }

    return clone;
}

Board CloneBoardV2(Board* board1) {
    Board clone;
    /* cloning grid */
    int file;
    int rank;
    for (rank = one; rank <= eight; rank++) {
        for (file = a; file <= h; file++) {
            clone.grid[file][rank].type = board1->grid[file][rank].type;
            clone.grid[file][rank].color = board1->grid[file][rank].color;
        }
    }
    /* cloning blackEnPassant & whiteEnPassant */
    for (int pawnNum = 0; pawnNum < 8; pawnNum++) {
        clone.blackUpForEnPassant[pawnNum] = board1->blackUpForEnPassant[pawnNum];
    }
    for (int pawnNum = 0; pawnNum < 8; pawnNum++) {
        clone.whiteUpForEnPassant[pawnNum] = board1->whiteUpForEnPassant[pawnNum];
    }
    /* cloning KingMoved variables */
    clone.blackKingMoved = board1->blackKingMoved;
    clone.whiteKingMoved = board1->whiteKingMoved;
    /* cloning PawnsMoved arrays */
    for (int rookNum = 0; rookNum < 2; rookNum++) {
        clone.blackRookMoved[rookNum] = board1->blackRookMoved[rookNum];
    }
    for (int rookNum = 0; rookNum < 2; rookNum++) {
        clone.whiteRookMoved[rookNum] = board1->whiteRookMoved[rookNum];
    }

    return clone;
}

/** Moving Pieces **/

//dumb moving function, does not check if move is legal, nor does it update board information pertaining to castling/enPassant. Check assumptions in header file.
void DumbMovePiece(Position initialPosition, Position finalPosition, Board* board) {
    board->grid[finalPosition.file][finalPosition.rank] = board->grid[initialPosition.file][initialPosition.rank];
    board->grid[initialPosition.file][initialPosition.rank].type = empty;
    board->grid[initialPosition.file][initialPosition.rank].color = no_color;
}

//helper function for SmartMovePiece()
void BoardUpkeep(Position initialPosition, Position finalPosition, Board* board) {
    Piece_type pieceType;
    Color pieceColor;
    pieceType = board->grid[initialPosition.file][initialPosition.rank].type;
    pieceColor = board->grid[initialPosition.file][initialPosition.rank].color;
    //pawn information upkeep
    for (int i = 0; i < 8; i++) {
            board->blackUpForEnPassant[i] = 0;
            board->whiteUpForEnPassant[i] = 0;
        }
    if (pieceColor == white) {
        if ((pieceType == pawn) && (finalPosition.rank - initialPosition.rank == 2)) {
        board->whiteUpForEnPassant[initialPosition.file] = 1;
        }
    }
    else if (pieceColor == black) {
        if ((pieceType == pawn) && (finalPosition.rank - initialPosition.rank == -2)) {
        board->blackUpForEnPassant[initialPosition.file] = 1;
        }
    }
    //king information upkeep
    if ((pieceColor == white) && (pieceType == king) && (board->whiteKingMoved == False)) {
        board->whiteKingMoved = True;
    }
    else if ((pieceColor == black) && (pieceType == king) && (board->blackKingMoved == False)) {
        board->blackKingMoved = True;
    }
    //rook information upkeep
    if ((pieceType == rook) && (initialPosition.file == a) && (initialPosition.rank == one) && (board->whiteRookMoved[0] == False)) {
        board->whiteRookMoved[0] = True;
    }
    else if ((pieceType == rook) && (initialPosition.file == h) && (initialPosition.rank == one) && (board->whiteRookMoved[1] == False)) {
        board->whiteRookMoved[1] = True;
    }
    else if ((pieceType == rook) && (initialPosition.file == a) && (initialPosition.rank == eight) && (board->blackRookMoved[0] == False)) {
        board->blackRookMoved[0] = True;
    }
    else if ((pieceType == rook) && (initialPosition.file == h) && (initialPosition.rank == eight) && (board->blackRookMoved[1] == False)) {
        board->blackRookMoved[1] = True;
    }

    //accounting for if an piece captures certain a rook that has not moved (set that position to hasMoved to not allow castling)
    if ((finalPosition.file == a) && (finalPosition.rank == one)) {
        board->whiteRookMoved[0] = True;
    }
    else if ((finalPosition.file == h) && (finalPosition.rank == one)) {
        board->whiteRookMoved[1] = True;
    }
    else if ((finalPosition.file == a) && (finalPosition.rank == eight)) {
        board->blackRookMoved[0] = True;
    }
    else if ((finalPosition.file == a) && (finalPosition.rank == eight)) {
        board->blackRookMoved[1] = True;
    }
    //Note: work-around for this would be to modify KingRules() in Castling scenario to check if there exists a rook at a given location
}

//called by SmartMovePiece() to move the corresponding rook if castling is done
void CastlingRookMove(Position finalKingPosition, Board* board) {
    Position rookInitial;
    Position rookFinal;

    rookInitial.rank = finalKingPosition.rank;
    rookFinal.rank = finalKingPosition.rank;

    if ((finalKingPosition.file == c) && ((finalKingPosition.rank == eight) || (finalKingPosition.rank == one))) {
        rookInitial.file = a;
        rookFinal.file = d;
    }
    else if ((finalKingPosition.file == g) && ((finalKingPosition.rank == eight) || (finalKingPosition.rank == one))) {
        rookInitial.file = h;
        rookFinal.file = f;
    }

    //updating board info
    BoardUpkeep(rookInitial, rookFinal, board);
    //moving piece
    DumbMovePiece(rookInitial, rookFinal, board);
}

//assume that move has already been checked to be legal !!!
//helper function for SmartMovePiece() that tells function if move is a castling, pawn-promo, en-pessant, or something else
int CastleOrPromotionOrEnPessant(Position initialPosition, Position finalPosition, Board *board) {
    Piece_type pieceType;
    Color pieceColor;
    pieceType = board->grid[initialPosition.file][initialPosition.rank].type;
    pieceColor = board->grid[initialPosition.file][initialPosition.rank].color;

    //if move is a promotion
    if ((pieceType == pawn) && 
    (((pieceColor == white) && (finalPosition.rank == eight)) || ((pieceColor == black) && (finalPosition.rank == one))))
    {
        return 1;   
    }
    //if move is a castling
    if ((pieceType == king) && (abs(finalPosition.file - initialPosition.file) == 2))
    {
        return 2;
    }
    //if move is a en-pessant
    if ((pieceType == pawn) && (board->grid[finalPosition.file][finalPosition.rank].type == empty) && 
    (abs(finalPosition.file - initialPosition.file) == abs(finalPosition.rank - initialPosition.rank)))
    {
        return 3;
    }
    //else
    return 0;
}

//moves piece if legal, else will return an errorcode. If not intending to promote a pawn, can set promotedType to anything.
int SmartMovePiece(Position initialPosition, Position finalPosition, Board* board, Piece_type promotedType) {
    int errorcode;

    /* determine if move is legal */
    errorcode = Rules(initialPosition, finalPosition, board);
    if (errorcode != 0) {
        //error detected
        return errorcode;
    }

    //else no error and move is legal

    /* en pessanting scenario */
    if (CastleOrPromotionOrEnPessant(initialPosition, finalPosition, board) == 3)
    {
        Position enemyPos;
        /* must remove enemy piece */

        enemyPos.rank = initialPosition.rank;
        enemyPos.file = finalPosition.file;

        BoardUpkeep(initialPosition, enemyPos, board);
        DumbMovePiece(initialPosition, enemyPos, board);
        BoardUpkeep(initialPosition, finalPosition, board);
        DumbMovePiece(enemyPos, finalPosition, board);    
    }
    /* castling scenario */
    else if (CastleOrPromotionOrEnPessant(initialPosition, finalPosition, board) == 2) 
    {
        //board information upkeep
        BoardUpkeep(initialPosition, finalPosition, board);
        //moving piece
        DumbMovePiece(initialPosition, finalPosition, board);
        //moving rook and updating board info
        CastlingRookMove(finalPosition, board);
    }

    /* promotion scenario */
    else if (CastleOrPromotionOrEnPessant(initialPosition, finalPosition, board) == 1) 
    {
        //board information upkeep
        BoardUpkeep(initialPosition, finalPosition, board);
        //moving piece
        DumbMovePiece(initialPosition, finalPosition, board);
        //promoting pawn
        board->grid[finalPosition.file][finalPosition.rank].type = promotedType;
    }

    /* all other scenarios */
    else 
    {
        //board information upkeep
        BoardUpkeep(initialPosition, finalPosition, board);
        //moving piece
        DumbMovePiece(initialPosition, finalPosition, board);
    }

    //do anyting else??? 
    return 0;
}

//general function which tells us whether a certain move is allowed with error codes
int Rules(Position initialPosition, Position finalPosition, Board* board) {
    Bool movePossibleIdeally;
    Board *tempBoard;
    Bool kingInCheck;

    //error checking
    if ((initialPosition.rank == finalPosition.rank) && (initialPosition.file == finalPosition.file)) {
        return 5; //errorcode 5: attempting to move a piece to the same spot
    }
    if (board->grid[initialPosition.file][initialPosition.rank].color == board->grid[finalPosition.file][finalPosition.rank].color) {
        return 6; //errorcode 6: attempting to move a piece to a spot where a piece of the same color resides
    }
    if ((initialPosition.rank > 7) || (initialPosition.rank < 0) || (initialPosition.file > 7) || (initialPosition.file < 0)) {
        return 7; //errorcode 7: attempting to access a piece outside the bounds of the board
    }
    if ((finalPosition.rank > 7) || (finalPosition.rank < 0) || (finalPosition.file > 7) || (finalPosition.file < 0)) {
        return 8; //errorcode 8: attempting to move a piece outside the bounds of the board
    }

    //selecting piece type
    switch (board->grid[initialPosition.file][initialPosition.rank].type)
    {
    case empty:
        return 3; //errorcode 3: attempting to move an 'empty' piece
        break;
    case king:
        movePossibleIdeally = KingRules(initialPosition, finalPosition, board);
        break;
    case queen:
        movePossibleIdeally = QueenRules(initialPosition, finalPosition, board);
        break;
    case bishop:
        movePossibleIdeally = BishopRules(initialPosition, finalPosition, board);
        break;
    case knight:
        movePossibleIdeally = KnightRules(initialPosition, finalPosition, board);
        break;
    case rook:
        movePossibleIdeally = RookRules(initialPosition, finalPosition, board);
        break;
    case pawn:
        movePossibleIdeally = PawnRules(initialPosition, finalPosition, board);
        break;
    default:
        return 4; //errorcode 4: piece is of unknown type
        break;
    }

    if (movePossibleIdeally == False) {
        //move not possible
        return 2; //errorcode 2: attempting to move a piece in such a way that its not possible even in ideal case
    }

    //else check if move works in non-ideal scenario aswell
    tempBoard = CloneBoard(board);


    if (CastleOrPromotionOrEnPessant(initialPosition, finalPosition, tempBoard) == 0) {
        DumbMovePiece(initialPosition, finalPosition, tempBoard);
    }
    //if move is a en-pessant
    else if (CastleOrPromotionOrEnPessant(initialPosition, finalPosition, tempBoard) == 3) {
        Position enemyPos;
        /* must remove enemy piece */
        enemyPos.rank = initialPosition.rank;
        enemyPos.file = finalPosition.file;
        DumbMovePiece(initialPosition, enemyPos, tempBoard);
        DumbMovePiece(enemyPos, finalPosition, tempBoard);    
    }
    //if move is a castling
    else if (CastleOrPromotionOrEnPessant(initialPosition, finalPosition, tempBoard) == 2) {
        //moving piece
        DumbMovePiece(initialPosition, finalPosition, tempBoard);
        //moving rook
        CastlingRookMove(finalPosition, tempBoard);
    }
    //if move is a pawn promo
    else if (CastleOrPromotionOrEnPessant(initialPosition, finalPosition, tempBoard) == 1) {
        //moving piece
        DumbMovePiece(initialPosition, finalPosition, tempBoard);
        //promoting pawn
        tempBoard->grid[finalPosition.file][finalPosition.rank].type = queen;
    }

    kingInCheck = CheckForCheck(LocateKing(board->grid[initialPosition.file][initialPosition.rank].color, tempBoard), tempBoard);
    free(tempBoard);
    tempBoard = NULL;

    if (kingInCheck) {
        //move is illegal
        return 1; //errorcode 1: attempting to move a piece such that the player's king would now be in check
    }

    return 0; //move is legal
}

//modified rules() function for calling CheckForCheck(), only checks the ideal case
int ModifiedRules(Position initialPosition, Position finalPosition, Board* board) {
    Bool movePossibleIdeally;

    //error checking
    if ((initialPosition.rank == finalPosition.rank) && (initialPosition.file == finalPosition.file)) {
        return 5; //errorcode 5: attempting to move a piece to the same spot
    }
    if (board->grid[initialPosition.file][initialPosition.rank].color == board->grid[finalPosition.file][finalPosition.rank].color) {
        return 6; //errorcode 6: attempting to move a piece to a spot where a piece of the same color resides
    }
    if ((initialPosition.rank > 7) || (initialPosition.rank < 0) || (initialPosition.file > 7) || (initialPosition.file < 0)) {
        return 7; //errorcode 7: attempting to access a piece outside the bounds of the board
    }
    if ((finalPosition.rank > 7) || (finalPosition.rank < 0) || (finalPosition.file > 7) || (finalPosition.file < 0)) {
        return 8; //errorcode 8: attempting to move a piece outside the bounds of the board
    }

    //selecting piece type
    switch (board->grid[initialPosition.file][initialPosition.rank].type)
    {
    case empty:
        return 3; //errorcode 3: attempting to move an 'empty' piece
        break;
    case king:
        movePossibleIdeally = KingRules(initialPosition, finalPosition, board);
        break;
    case queen:
        movePossibleIdeally = QueenRules(initialPosition, finalPosition, board);
        break;
    case bishop:
        movePossibleIdeally = BishopRules(initialPosition, finalPosition, board);
        break;
    case knight:
        movePossibleIdeally = KnightRules(initialPosition, finalPosition, board);
        break;
    case rook:
        movePossibleIdeally = RookRules(initialPosition, finalPosition, board);
        break;
    case pawn:
        movePossibleIdeally = PawnRules(initialPosition, finalPosition, board);
        break;
    default:
        return 4; //errorcode 4: piece is of unknown type
        break;
    }

    if (movePossibleIdeally == False) {
        //move not possible
        return 2; //errorcode 2: attempting to move a piece in such a way that its not possible even in ideal case
    }

    return 0; //move is legal
}

/* Helper functions for moving ideally (remember to read assumptions in header) */

Bool KingRules(Position initialPosition, Position finalPosition, Board* board) {
    /* normal scenario */
    if  ((abs(finalPosition.file - initialPosition.file) <= 1) && 
        (abs(finalPosition.rank - initialPosition.rank) <= 1)) {   
        return True;
    }

    /* castling scenario */
    //checking if king is trying to move 2 spaces horizontally
    if  (!((finalPosition.rank - initialPosition.rank == 0) && 
        (abs(finalPosition.file - initialPosition.file) == 2))) {   
        return False;
    }
    //checking if the proper king has not moved
    Color kingColor;
    kingColor = board->grid[initialPosition.file][initialPosition.rank].color;
    if      ((kingColor == white) && (board->whiteKingMoved == False))   { }
    else if ((kingColor == black) && (board->blackKingMoved == False))   { }
    else    { return False; }
    //checking if the proper rook has not moved, and that the proper positions are empty
    if      ((finalPosition.file == c) && (finalPosition.rank == one) && (kingColor == white))
            {
                if  ((board->grid[b][one].type == empty) && (board->grid[c][one].type == empty) && 
                     (board->grid[d][one].type == empty) && (board->whiteRookMoved[0] == False))
                { return True; }
            }
    else if ((finalPosition.file == g) && (finalPosition.rank == one) && (kingColor == white))
            {
                if  ((board->grid[f][one].type == empty) && (board->grid[g][one].type == empty) && 
                     (board->whiteRookMoved[1] == False)) 
                { return True; }
            }
    else if ((finalPosition.file == c) && (finalPosition.rank == eight) && (kingColor == black))
            {
                if  ((board->grid[b][eight].type == empty) && (board->grid[c][eight].type == empty) && 
                     (board->grid[d][eight].type == empty) && (board->blackRookMoved[0] == False))
                { return True; }
            }
    else if ((finalPosition.file == g) && (finalPosition.rank == eight) && (kingColor == black))
            {
                if  ((board->grid[f][eight].type == empty) && (board->grid[g][eight].type == empty) && 
                     (board->blackRookMoved[1] == False)) 
                { return True; }
            }
    //else
    return False;
}

Bool PawnRules(Position initialPosition, Position finalPosition, Board* board) {
    Color pawnColor;
    pawnColor = board->grid[initialPosition.file][initialPosition.rank].color;

    /* pawn moves straight 1 space */
    //white case
    if ((finalPosition.file == initialPosition.file) && (finalPosition.rank == initialPosition.rank + 1) && (pawnColor == white)) {
        if (board->grid[finalPosition.file][finalPosition.rank].type == empty)  { return True; }
        else                                                                    { return False; }
    }
    //black case
    if ((finalPosition.file == initialPosition.file) && (finalPosition.rank == initialPosition.rank - 1) && (pawnColor == black)) {
        if (board->grid[finalPosition.file][finalPosition.rank].type == empty)  { return True; }
        else                                                                    { return False; }
    }

    /* pawn moves straight 2 spaces on its first move */
    //white case
    if ((finalPosition.file == initialPosition.file) && (finalPosition.rank == initialPosition.rank + 2) && (pawnColor == white)) {
        if (initialPosition.rank != two) { return False; } //pawn has moved before
        //if both the 2 spaces above the pawn are both empty
        if ((board->grid[finalPosition.file][finalPosition.rank].type == empty)
            && (board->grid[finalPosition.file][finalPosition.rank - 1].type == empty))
                                                                                { return True; }
        else                                                                    { return False; }
    }
    //black case
    if ((finalPosition.file == initialPosition.file) && (finalPosition.rank == initialPosition.rank - 2) && (pawnColor == black)) {
        if (initialPosition.rank != seven) { return False; } //pawn has moved before
        //if both the 2 spaces below the pawn are both empty
        if ((board->grid[finalPosition.file][finalPosition.rank].type == empty)
            && (board->grid[finalPosition.file][finalPosition.rank + 1].type == empty))
                                                                                { return True; }
        else                                                                    { return False; }
    }

    /* pawns captures piece and moves diagonally */
    //white case
    if (((finalPosition.file == initialPosition.file + 1) || (finalPosition.file == initialPosition.file - 1)) && 
        (finalPosition.rank == initialPosition.rank + 1) && (pawnColor == white)) {
        if (board->grid[finalPosition.file][finalPosition.rank].color == black) { return True; }
    }
    //black case
    if (((finalPosition.file == initialPosition.file + 1) || (finalPosition.file == initialPosition.file - 1)) && 
        (finalPosition.rank == initialPosition.rank - 1) && (pawnColor == black)) {
        if (board->grid[finalPosition.file][finalPosition.rank].color == white) { return True; }
    }

    /* En Passant scenario */
    //white case moving to the right
    if ((finalPosition.file == initialPosition.file + 1) && (finalPosition.rank == initialPosition.rank + 1) && (pawnColor == white)) {
        if ((initialPosition.rank == five) && (board->blackUpForEnPassant[initialPosition.file + 1] == True) &&
            (board->grid[initialPosition.file + 1][initialPosition.rank].color == black) && 
            (board->grid[initialPosition.file + 1][initialPosition.rank].type == pawn) && 
            (board->grid[finalPosition.file][finalPosition.rank].type == empty)) 
        {
            return True;
        }
    }
    //white case moving to the left
    if ((finalPosition.file == initialPosition.file - 1) && (finalPosition.rank == initialPosition.rank + 1) && (pawnColor == white)) {
        if ((initialPosition.rank == five) && (board->blackUpForEnPassant[initialPosition.file - 1] == True) &&
            (board->grid[initialPosition.file - 1][initialPosition.rank].color == black) && 
            (board->grid[initialPosition.file - 1][initialPosition.rank].type == pawn) && 
            (board->grid[finalPosition.file][finalPosition.rank].type == empty)) 
        {
            return True;
        }         
    }
    //black case moving to the right
    if ((finalPosition.file == initialPosition.file + 1) && (finalPosition.rank == initialPosition.rank - 1) && (pawnColor == black)) {
        if ((initialPosition.rank == four) && (board->whiteUpForEnPassant[initialPosition.file + 1] == True) &&
            (board->grid[initialPosition.file + 1][initialPosition.rank].color == white) && 
            (board->grid[initialPosition.file + 1][initialPosition.rank].type == pawn) && 
            (board->grid[finalPosition.file][finalPosition.rank].type == empty)) 
        {
            return True;
        }            
    }
    //black case moving to the left
    if ((finalPosition.file == initialPosition.file - 1) && (finalPosition.rank == initialPosition.rank - 1) && (pawnColor == black)) {
        if ((initialPosition.rank == four) && (board->whiteUpForEnPassant[initialPosition.file - 1] == True) &&
            (board->grid[initialPosition.file - 1][initialPosition.rank].color == white) && 
            (board->grid[initialPosition.file - 1][initialPosition.rank].type == pawn) && 
            (board->grid[finalPosition.file][finalPosition.rank].type == empty)) 
        {
            return True;
        }            
    }
    //else
    return False;
} 

Bool QueenRules(Position initialPosition, Position finalPosition, Board* board) {
    // queen can either move like a bishop or a rook
    if ((BishopRules(initialPosition, finalPosition, board) == True) || 
        (RookRules(initialPosition, finalPosition, board) == True))
        { return True; }
    //else
    return False;
}


Bool BishopRules(Position initialPosition, Position finalPosition, Board* board) {
    int checkDiagonal;
    
    //makes sure bishop is attempting to move diagonally
    if (abs(finalPosition.file - initialPosition.file) != abs(finalPosition.rank - initialPosition.rank)) { return False; }

	/* 4 if-statements to check which direction the bishop wants to go relative to its position */

    // 1. moving to bottom right
	if (finalPosition.file > initialPosition.file && finalPosition.rank < initialPosition.rank) {
        for (checkDiagonal = 1; checkDiagonal < abs(finalPosition.file - initialPosition.file); checkDiagonal++) 
        {
		    if (board->grid[initialPosition.file + checkDiagonal][initialPosition.rank - checkDiagonal].type != empty) 
            { return False; }
        }
        //else all relavent boxes inbetween are empty
        return True;
    }
    //2. moving to top right
	if (finalPosition.file > initialPosition.file && finalPosition.rank > initialPosition.rank) {
	    for (checkDiagonal = 1; checkDiagonal < abs(finalPosition.file - initialPosition.file); checkDiagonal++) 
        {
            if (board->grid[initialPosition.file + checkDiagonal][initialPosition.rank + checkDiagonal].type != empty) 
            { return False; }
        }
        //else all relavent boxes inbetween are empty
        return True;
	}
	// 3. moving to top left
	if (finalPosition.file < initialPosition.file && finalPosition.rank > initialPosition.rank) {
        for (checkDiagonal = 1; checkDiagonal < abs(finalPosition.file - initialPosition.file); checkDiagonal++) 
        {
            if (board->grid[initialPosition.file - checkDiagonal][initialPosition.rank + checkDiagonal].type != empty) 
            { return False; }
        }
        //else all relavent boxes inbetween are empty
        return True;
    }
	// 4. moving to bottom left
    if (finalPosition.file < initialPosition.file && finalPosition.rank < initialPosition.rank) {
        for (checkDiagonal = 1; checkDiagonal < abs(finalPosition.file - initialPosition.file); checkDiagonal++) 
        {
            if (board->grid[initialPosition.file - checkDiagonal][initialPosition.rank - checkDiagonal].type != empty) 
            { return False; }
        }
        //else all relavent boxes inbetween are empty
        return True;        
    }

    //else
    return False;
}

Bool KnightRules(Position initialPosition, Position finalPosition, Board* board) {

    /* 8 different move patterns for the knight */

    // 1. 
    if ((finalPosition.file == initialPosition.file + 2) && (finalPosition.rank == initialPosition.rank + 1)) { return True; }
    // 2. 
    if ((finalPosition.file == initialPosition.file + 1) && (finalPosition.rank == initialPosition.rank + 2)) { return True; }
    // 3. 
    if ((finalPosition.file == initialPosition.file - 1) && (finalPosition.rank == initialPosition.rank + 2)) { return True; }
    // 4. 
    if ((finalPosition.file == initialPosition.file - 2) && (finalPosition.rank == initialPosition.rank + 1)) { return True; }
    // 5. 
    if ((finalPosition.file == initialPosition.file - 2) && (finalPosition.rank == initialPosition.rank - 1)) { return True; }
    // 6. 
    if ((finalPosition.file == initialPosition.file - 1) && (finalPosition.rank == initialPosition.rank - 2)) { return True; }
    // 7. 
    if ((finalPosition.file == initialPosition.file + 1) && (finalPosition.rank == initialPosition.rank - 2)) { return True; }
    // 8. 
    if ((finalPosition.file == initialPosition.file + 2) && (finalPosition.rank == initialPosition.rank - 1)) { return True; }

    //else
    return False;
}

Bool RookRules(Position initialPosition, Position finalPosition, Board* board) {
    int checkLine;

    //check if rook is attemting to move either horizontally or vertically only
    if (!((abs(finalPosition.file - initialPosition.file) == 0 && abs(finalPosition.rank - initialPosition.rank) != 0) ||
        (abs(finalPosition.file - initialPosition.file) != 0 && abs(finalPosition.rank - initialPosition.rank) == 0 )))
    { return False; } 

    /* 4 if-statements to check which direction the rook wants to go relative to its position */

    // 1. moving up
    if (finalPosition.file == initialPosition.file && finalPosition.rank > initialPosition.rank) {
        for (checkLine = 1; checkLine < abs(finalPosition.rank - initialPosition.rank); checkLine++) {
            if (board->grid[initialPosition.file][initialPosition.rank + checkLine].type != empty) 
            { return False; }
        }
        //else all relavent boxes inbetween are empty
        return True;
    }
    // 2. moving down
    if (finalPosition.file == initialPosition.file && finalPosition.rank < initialPosition.rank) {
        for (checkLine = 1; checkLine < abs(finalPosition.rank - initialPosition.rank); checkLine++) {
            if (board->grid[initialPosition.file][initialPosition.rank - checkLine].type != empty) 
            { return False; }
        }
        //else all relavent boxes inbetween are empty
        return True;
    }
    // 3. moving left
    if (finalPosition.file < initialPosition.file && finalPosition.rank == initialPosition.rank) {
        for (checkLine = 1; checkLine < abs(finalPosition.file - initialPosition.file); checkLine++) {
            if (board->grid[initialPosition.file - checkLine][initialPosition.rank].type != empty) 
            { return False; }
        }
        //else all relavent boxes inbetween are empty
        return True;
    }
    // 4. moving right
    if (finalPosition.file > initialPosition.file && finalPosition.rank == initialPosition.rank) {
        for (checkLine = 1; checkLine < abs(finalPosition.file - initialPosition.file); checkLine++) {
            if (board->grid[initialPosition.file + checkLine][initialPosition.rank].type != empty) 
            { return False; }
        }
        //else all relavent boxes inbetween are empty
        return True;
    }

    //else
    return False;
}

//SUBJECT TO CHANGE:
//returns a Position struct sized block of memory of variable size, remember to free() later
PositionList getMoves(Position position, Board* board) {
    //create an array of a given size
    PositionList allPositions;
    Position arry[64];
    Position finalPos;
    int file;
    int rank;
    int errorcode;
    int totalPos;

    totalPos = 0;

    for (rank = one; rank <= eight; rank++) {
        finalPos.rank = rank;
        for (file = a; file <= h; file++) {
            finalPos.file = file;
            errorcode = Rules(position, finalPos, board);
            if (errorcode == 0) {
                arry[totalPos] = finalPos;
                totalPos++;
            }
        }
    }

    allPositions.size = totalPos;

    allPositions.list = malloc(totalPos * sizeof(Position));

    for (int i = 0; i < totalPos; i++) {
        allPositions.list[i] = arry[i];
    }

    return allPositions; //dynamic size
}

//more effcient algorithm may be possible
Bool CheckForCheck(Position kingPosition, Board *board) {
    int file;
    int rank;
    int errorcode;
    Position initialPos;

    //iterates through each position on board and asks if a piece can move to the position of the king
    for (rank = one; rank <= eight; rank++) {
        initialPos.rank = rank;
        for (file = a; file <= h; file++) {
            initialPos.file = file;
            errorcode = ModifiedRules(initialPos, kingPosition, board);
            if (errorcode == 0) {
                return True;
            }
        }
    }

    return False;
}


Bool CheckForCheckMate(Position kingPosition, Board *board) {
    //def. of checkmate: king is in check, and each way it can move will also make it in check
    if (CheckForCheck(kingPosition, board) == False) { return False; }

    PositionList possibleMoves;
    possibleMoves = getMoves(kingPosition, board);
    free(possibleMoves.list);
    if (possibleMoves.size == 0) {
        return True;
    }
    else {
        return False;
    }
}

//can be changed later to not allocate memory inside funciton and instead do it some other time
Board* StartingBoard() {
    //creating board in memory
    Board* board = malloc(sizeof(Board));

    /* Handling Colors */

    //making bottom two ranks all white
    for (int j = one; j <= two; j++) {
        for (int i = a; i <= h; i++) {
            board->grid[i][j].color = white;
        }
    }
    //making Ranks 3 to 6 all no_color
    for (int j = three; j <= six; j++) {
        for (int i = a; i <= h; i++) {
            board->grid[i][j].color = no_color;
        }
    }
    //making Ranks 7 to 8 all no_color
    for (int j = seven; j <= eight; j++) {
        for (int i = a; i <= h; i++) {
            board->grid[i][j].color = black;
        }
    }

    /* Handling Types */

    //rank 1
    board->grid[a][one].type = rook;
    board->grid[b][one].type = knight;
    board->grid[c][one].type = bishop;
    board->grid[d][one].type = queen;
    board->grid[e][one].type = king;
    board->grid[f][one].type = bishop;
    board->grid[g][one].type = knight;
    board->grid[h][one].type = rook;

    //rank 2
    for (int i = a; i <= h; i++) {
        board->grid[i][two].type = pawn;
    }

    //rank 3 - 6
    for (int j = three; j <= six; j++) {
        for (int i = a; i <= h; i++) {
            board->grid[i][j].type = empty;
        }
    }

    //rank 7
    for (int i = a; i <= h; i++) {
        board->grid[i][seven].type = pawn;
    }

    //rank 8
    board->grid[a][eight].type = rook;
    board->grid[b][eight].type = knight;
    board->grid[c][eight].type = bishop;
    board->grid[d][eight].type = queen;
    board->grid[e][eight].type = king;
    board->grid[f][eight].type = bishop;
    board->grid[g][eight].type = knight;
    board->grid[h][eight].type = rook;    

    /* Handling extra boad information */

    //black
    board->blackKingMoved = False;
    board->blackRookMoved[0] = False;
    board->blackRookMoved[1] = False;

    //white
    board->whiteKingMoved = False;
    board->whiteRookMoved[0] = False;
    board->whiteRookMoved[1] = False;

    //black and white
    for (int i = 0; i < 8; i++) {
        board->blackUpForEnPassant[i] = False;
        board->whiteUpForEnPassant[i] = False;
    }

    return board;
}

void PrintFileAndRank(Position pos) {
    printf("(");

    switch (pos.file)
    {
    case a:
        printf("a");
        break;
    case b:
        printf("b");
        break;
    case c:
        printf("c");
        break;
    case d:
        printf("d");
        break;
    case e:
        printf("e");
        break;
    case f:
        printf("f");
        break;
    case g:
        printf("g");
        break;
    case h:
        printf("h");
        break;
    default:
        printf("unknown_file");
        break;
    }

    printf(", ");

    switch (pos.rank)
    {
    case one:
        printf("1");
        break;
    case two:
        printf("2");
        break;
    case three:
        printf("3");
        break;
    case four:
        printf("4");
        break;
    case five:
        printf("5");
        break;
    case six:
        printf("6");
        break;
    case seven:
        printf("7");
        break;
    case eight:
        printf("8");
        break;
    default:
        printf("unknown_rank");
        break;
    }

    printf(")");
}

Position SetPosition(char file, char rank) {
    Position pos;
    switch (file)
    {
    case 'a':
        pos.file = a;
        break;
    case 'b':
        pos.file = b;
        break;
    case 'c':
        pos.file = c;
        break;
    case 'd':
        pos.file = d;
        break;
    case 'e':
        pos.file = e;
        break;
    case 'f':
        pos.file = f;
        break;
    case 'g':
        pos.file = g;
        break;
    case 'h':
        pos.file = h;
        break;    
    default:
        pos.file = unknownFile;
        break;
    }

    switch (rank)
    {
    case '1':
        pos.rank = one;
        break;
    case '2':
        pos.rank = two;
        break;
    case '3':
        pos.rank = three;
        break;
    case '4':
        pos.rank = four;
        break;
    case '5':
        pos.rank = five;
        break;
    case '6':
        pos.rank = six;
        break;
    case '7':
        pos.rank = seven;
        break;
    case '8':
        pos.rank = eight;
        break;    
    default:
        pos.rank = unknownRank;
        break;
    }

    return pos;
}

//must free(MoveList.list) memory block afterwards
MoveList GetAllMoves(Color teamColor, Board *board) {
    int file1, rank1;
    int file, rank;
    Position currentStartingPosition, currentFinalPosition;
    Move currentMove;
    Move allMoves[200];
    int totalMoves;
    int errorcode;
    MoveList listOfMoves;

    totalMoves = 0;

    /* iterating through each position on board using outer double for-loop, and for 
    each position (which will be a starting position), iterate through the entire board again
    using the inner double for-loop to see if a position can be a legal position (final position) 
    for the starting to move to. Will be iterating 64 * 64 times*/

    for (rank1 = one; rank1 <= eight; rank1++) {
        currentStartingPosition.rank = rank;
        for (file1 = a; file1 <= h; file1++) {
            currentStartingPosition.file = file;
            currentMove.iPos = currentStartingPosition;

            for (rank = one; rank <= eight; rank++) {
                currentFinalPosition.rank = rank;
                for (file = a; file <= h; file++) {
                    currentFinalPosition.file = file;
                    errorcode = Rules(currentStartingPosition, currentFinalPosition, board);
                    if (errorcode == 0) {
                        currentMove.fPos = currentFinalPosition;
                        allMoves[totalMoves] = currentMove;
                        totalMoves++;
                    }
                }
            }

        }
    }

    listOfMoves.size = totalMoves;
    listOfMoves.list = malloc(totalMoves * sizeof(Move));

    for (int i = 0; i < totalMoves; i++) {
        listOfMoves.list[i] = allMoves[i];
    }

    return listOfMoves; //dynamic size
}

