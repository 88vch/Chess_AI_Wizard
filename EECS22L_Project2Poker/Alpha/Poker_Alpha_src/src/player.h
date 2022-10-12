#ifndef PLAYER_H
#define PLAYER_H
#include "deck.h"

typedef enum {
    dealer = 0,
    smallBlind = 1,
    bigBlind = 2,
    none = 3
} Button; //pretty sure this is not the correct name

typedef enum {
    notfolded = 0, //or call
    folded = 1
} HasFolded;

typedef enum {
    check = 0,
    call = 1,
    raised = 3,
    fold = 4
} MoveChoice;

typedef struct
{
    MoveChoice choice;
    int raiseAmount; //only relevant when player chooses to raise
} Move;


typedef struct {
    Button token;
    int money;
    Card pocket[2];
    HasFolded hasFolded;
    int currentBetAmount;
    int position;
    char username[];
} Player;



#endif