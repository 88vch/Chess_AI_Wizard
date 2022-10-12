#ifndef DECK_H
#define DECK_H
#include "card.h"

typedef struct {
    int totCard;
    Card cards[52];
} Deck;

/* deck.h: updated shuffle, CreateDeck, and DrawCard functions */

Deck ShuffleDeck(Deck deck);

Deck CreateDeck() ;

Card DrawCard(Deck *deck);
#endif


