#include "deck.h"
#include <assert.h>
#include <stdlib.h>
#include <time.h>

Deck ShuffleDeck(Deck deck) {
    assert(deck.totCard == 52);
    srand(time(NULL));

    Card temp_card;
    int random_number1, random_number2;
    // "shuffling" cards a random number of times
    for (int j = 0; j < 500; j++) {
        random_number1 = rand()%52;
        random_number2 = rand()%52;
        temp_card = deck.cards[random_number1];
        deck.cards[random_number1] = deck.cards[random_number2];
        deck.cards[random_number2] = temp_card;
    }
    return deck;
}


Deck CreateDeck() {
    Deck deck;
    deck.totCard = 52;
    //initializs deck with every type of card
    int suit = 0;
    int num = 2;

    for (int i = 0; i < 52; i++) {        
        if (num > 14) {
            num = 2;
            suit++;
        }
        deck.cards[i].num = num;
        deck.cards[i].suit = suit;

        num++;
    }
    return deck;
}


Card DrawCard(Deck *deck) {
    Card topCard;
    //draws a card, then update deck
    /* 
    note: sizeof(deck->cards[]) == 52, 
        important to note that size of cards array doesn't change
    */

    int total_cards_in_deck = 52 - deck->totCard;
    topCard = deck->cards[total_cards_in_deck];

    deck->totCard--;

    return topCard;
}