#ifndef TABLE_H
#define TABLE_H
#include "player.h"
#include "deck.h"
#include "card.h"

typedef struct {
    Player* players; //assume that players start from position 0 to (totPlayers-1) moving clockwise
    Deck deck;
    int totPlayers;
    //int totPlayersWithMoney; //includes players that have folded
    int totActivePlayers; //only includes players that have money and have not folded
    int pot;
    int dealerNum; //specifies which player is the dealer
    int currentMinBet;
    Card tableCards[5];
} Table;


/* struct used for bestHand */
typedef struct {
    int playerPosition;
    int playerBestRanking;
    Card playerAllCards[7];
    int winnerOfTie;
    Card tieCards[5];
    int hasHighestPair;
} playerStatus;


Table CreateTable(int numOfPlayers, int startingMoney);

Table DeleteTable(Table table);

//assumes that the dealer position has already been updated
//finds small and big blind positions, and posts the blinds
Table SetBigAndSmallBlind(Table table, int smallblind, int bigblind);

//call before a betting round, set minBet to 0 for all but the first betting round
Table ResetBets(Table table, int minBet);

//has the possibility of looping over multiple times if playersAhead param is too large
int GetNextPlayerWithMoney(Table table, int playersAhead, int startingPos);

int GetPrevPlayerWithMoney(Table table, int playersBehind, int startingPos);

int GetNextPlayerWithMoneyAndNotFolded(Table table, int playersAhead, int startingPos);
//returns a valid move (choice and raise-amount (if player chooses to raise)), also sends a confirmation to player that move is legal
Move PlayerMoveChoice(Table table, int playerNum);

//ends with all active players either folding, or placing bets
Table BettingRound(Table table, int startingPlayer);

/* helper functions: */
//note: might be changed to find the next player who still has money
//moving clockwise
int GetNextPos(Table table, int currentPos);
//moving counter-clockwise
int GetPrevPos(Table table, int currentPos);

//returns position of player who wins has best hand on table
int BestHand(Table table);


void RequestPlayerInput(Table table, int PlayerNum);

/* helper functions for testing */
void TranslateMoveChoice(MoveChoice choice);
void PrintCurrentBalance(Table table);
void TranslateHandRanking(int ranking);
Move PlayerMoveChoice_Test(int playerNum);


Table GivePocketCards(Table table);

Table Flop(Table table);

//enter 4 or 5 for tableCardNum to display 4th or 5th round
Table Show4or5Card(Table table, int tableCardNum);

//call when only 1 active player remaining
int FindLastPlayer(Table table);

//awards player who wins by being last remaining
Table AwardLastPlayer(Table table);

Table Showdown(Table table);

//reseting everything for a new hand
Table NewHand(Table table);

//assumes NewHand has already been called
Table MoveTokens(Table table);

int PlayersWithMoney(Table table);

int findOnlyPlayerWithMoney(Table table);

/* returns position of player with best hand */
int BestHand(Table table);

/* helper functions for BestHand */
int checkOnePair(Card *card);
int checkTwoPair(Card *card);
int checkThreeOfAKind(Card *card);
int checkStraight(Card *card);
int checkFlush(Card *card);
int checkFullHouse(Card *card);
int checkFourOfAKind(Card *card);
int checkStraightFlush(Card *card);
int checkRoyalFlush(Card *card);


/* helper function for testing specific hand combinations */
Table giveAllCards(Table table);

#endif