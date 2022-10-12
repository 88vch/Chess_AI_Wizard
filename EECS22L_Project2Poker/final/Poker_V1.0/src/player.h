#ifndef PLAYER_H
#define PLAYER_H
#include "card.h"

typedef enum {
    dealer = 0,
    smallBlind = 1,
    bigBlind = 2,
    none = 3
} Button; //pretty sure this is not the correct name


//i think it would be best to use MoveChoice instead of this enum
/*
typedef enum {
    notfolded = 0, //or call
    folded = 1
} HasFolded;
*/

typedef enum {
    null = 0,
    check = 1,
    call = 2,
    raised = 3,
    fold = 4,
    blind = 5
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
    int currentBetAmount;
    int position;
    MoveChoice moveChoice;
    char username[50];
} Player;

#endif