#ifndef CARD_H
#define CARD_H

typedef enum {
    heart = 0,
    diamond = 1,
    spade = 2,
    club = 3,
    unknownSuit = 4, //could be used by clients to represent other player's pocket cards
    blankSuit = 5 //used to make table cards that have not been shown to be 'invisible'
} Suit;

typedef enum {
    two = 2,
    three = 3,
    four = 4,
    five = 5,
    six = 6,
    seven = 7,
    eight = 8,
    nine = 9,
    ten = 10,
    jack = 11,
    queen = 12,
    king = 13,
    ace = 14,
    unknownNum = 15, //could be used by clients to represent other player's pocket cards
    blankNum = 16 //used to make table cards that have not been shown to be 'invisible'
} Num;

typedef struct {
    Suit suit;
    Num num;
} Card; //a playing card

#endif