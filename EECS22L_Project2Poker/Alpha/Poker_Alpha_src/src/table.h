#ifndef TABLE_H
#define TABLE_H
#include "player.h"
#include <stdlib.h>

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

Table CreateTable(Table table, int numOfPlayers, int startingMoney) {
    free(table.players);
    table.players = malloc(sizeof(Player) * numOfPlayers);
    table.deck = CreateDeck();
    table.totPlayers = numOfPlayers;
    table.totActivePlayers = numOfPlayers;
    //give each player startingMoney amount
    table.pot = 0;
    table.dealerNum = 0; //for now, could change later
    for (int i = 0; i < 5; i++) { 
        table.tableCards[i].num = blankNum; 
        table.tableCards[i].suit = blankSuit;
    }
    return table;
}

Table DeleteTable(Table table) {
    free(table.players);
    table.players = NULL;
    table.totPlayers = 0;
    table.totActivePlayers = 0;
    return table;
}

//

//assumes that table tokens have already been properly assigned for a given round
//this wont work b/c we cant know amount allowed until we know oher factors-->check if bet amoount is allowed somewhere else?s
//assumes that player.currentBetAmount has been updated, and that player money amount has been updated
/*
Table PlaceBetOnTable(Table table, int playerNum, int amount) {
    int moneySubtracted;
    moneySubtracted = table.currentMinBet - amount;
    table.pot = table.pot + moneySubtracted;
    return table;
}
Table PlaceCall(Table table, int playerNum) {
    int moneySubtracted;
    moneySubtracted = table.currentMinBet - amount;
    table.pot = table.pot + moneySubtracted;
    return table;
}
*/

//assumes that the dealer position has already been updated
//finds small and big blind positions, and posts the blinds
Table SetBigAndSmallBlind(Table table, int smallblind, int bigblind) {
    // given dealer position, use GetNextPlayerWithMoney() to find player to assign smallblind & bigblind to
    int smallblind_pos = GetNextPlayerWithMoney(table, 1, table.dealerNum);
    int bigblind_pos = GetNextPlayerWithMoney(table, 2, table.dealerNum);

    table.players[smallblind_pos].currentBetAmount = smallblind;
    table.players[smallblind_pos].money -= smallblind;

    table.pot += smallBlind;
    table.currentMinBet = smallBlind;

    table.players[bigblind_pos].currentBetAmount = bigblind;
    table.players[bigblind_pos].money -= bigblind;

    table.pot += bigBlind;
    table.currentMinBet = bigBlind;

    return table;
}


//call before a betting round
Table ResetBets(Table table) {
    //iterates through table and resets all players current bets to 0
    //also reset folding status of all players with any money left over to not folded
    return table;
}

//has the possibility of looping over multiple times if playersAhead param is too large
int GetNextPlayerWithMoney(Table table, int playersAhead, int startingPos) {
    int i = 0;
    int finalPos = startingPos;
    while (i < playersAhead)
    {
        finalPos = GetNextPos(table, finalPos);
        if (table.players[finalPos].money <= 0) { continue; }
        i++;
    }
    return finalPos;
}
int GetPrevPlayerWithMoney(Table table, int playersBehind, int startingPos) {
    int i = 0;
    int finalPos = startingPos;
    while (i < playersBehind)
    {
        finalPos = GetPrevPos(table, finalPos);
        if (table.players[finalPos].money <= 0) { continue; }
        i++;
    }
    return finalPos;
}

int GetNextPlayerWithMoneyAndNotFolded(Table table, int playersAhead, int startingPos) {
    int finalPos = startingPos;
    int i = 0;
    while (i < playersAhead) {
        finalPos = GetNextPlayerWithMoney(table, 1, finalPos);
        if (table.players[finalPos].hasFolded == folded) { continue; }
        i++;
    }
    return finalPos;
}

//returns a valid move (choice and raise-amount (if player chooses to raise)), also sends a confirmation to player that move is legal
Move PlayerMoveChoice(Table table, int playerNum) {
    Move moveChoice;
    //call, check, raise, or fold (check is the same as call, but betting 0)
    /* insert some network function */
    //if player chooses to raise, send over amount as well
    //checks if move is legal. If not sends another request
    return moveChoice;
}

//ends with all active players either folding, or placing bets
Table BettingRound(Table table, int startingPlayer) {
    int currentPlayer;
    Move userMove;
    int moneyGiven;
    //assume that there is more than one player who has not folded
    assert(table.totActivePlayers > 1);
    assert(table.players[startingPlayer].money > 0 && table.players[startingPlayer].hasFolded == notfolded);

    currentPlayer = startingPlayer;

    do {
        //player can either call, check, raise, or fold --> could be enums
        //could update players client here, or wait till we do it for everybodys
        userMove = PlayerMoveChoice(table, currentPlayer);

        if (userMove.choice == check || userMove.choice == call) {
            moneyGiven = table.currentMinBet - table.players[currentPlayer].currentBetAmount;
            //updating table's info
            table.pot = table.pot + moneyGiven;
            //updating player's info
            table.players[currentPlayer].hasFolded = notfolded;
            table.players[currentPlayer].currentBetAmount = table.currentMinBet;
            table.players[currentPlayer].money = table.players[currentPlayer].money - moneyGiven;
        }
        else if (userMove.choice == raised) {
            moneyGiven = userMove.raiseAmount + (table.currentMinBet - table.players[currentPlayer].currentBetAmount);
            //updating table's info
            table.currentMinBet = userMove.raiseAmount + table.currentMinBet;
            table.pot = table.pot + moneyGiven;
            //updating player's info
            table.players[currentPlayer].hasFolded = notfolded;
            table.players[currentPlayer].currentBetAmount = table.currentMinBet;
            table.players[currentPlayer].money = table.players[currentPlayer].money - moneyGiven;
            //so that do loop ends on player before the currentPlayer
            startingPlayer = currentPlayer;
        }
        else if (userMove.choice == folded) {
            //update player's info
            table.players[currentPlayer].hasFolded = folded;
        }

        //update server gui & client ui

        currentPlayer = GetNextPlayerWithMoneyAndNotFolded(table, 1, currentPlayer); 
    }
    while(currentPlayer != startingPlayer);
    
    return table;
}

/* helper functions: */
//note: might be changed to find the next player who still has money
//moving clockwise
int GetNextPos(Table table, int currentPos) {
    if (currentPos + 1 <= table.totPlayers - 1) {
        return currentPos + 1;
    }
    else {
        return 0;
    }
}
//moving counter-clockwise
int GetPrevPos(Table table, int currentPos) {
    if (currentPos - 1 >= 0) {
        return currentPos - 1;
    }
    else {
        return table.totPlayers - 1; 
    }
}


//returns position of player who wins has best hand on table
int BestHand(Table table) {
    //iterates through table of players who have not folded
    //returns winner

    /* suit rankings: 1 - 10 with 1 being the best. */
    assert (table.totActivePlayers > 1);

    int bestPlayer;
    int currentPlayer = table.dealerNum;

    int currentPlayerRanking;
    int bestPlayerRanking = 11;
    
    int rankingNum, i;

    bestPlayer = currentPlayer;
    do {
        currentPlayerRanking = 0;
        rankingNum = 1;
        while (rankingNum <= 10) {
            Card currentPlayerCards[7] = {
                table.tableCards[0],
                table.tableCards[1], 
                table.tableCards[2],
                table.tableCards[3],
                table.tableCards[4],
                table.players[currentPlayer].pocket[0],
                table.players[currentPlayer].pocket[1],  
            };
            switch(rankingNum) {    
                /* Royal Flush */
                case (1):
                    currentPlayerRanking = (checkRoyalFlush(currentPlayerCards)) ? rankingNum : currentPlayerRanking;
                /* Straight Flush */
                case (2):
                    currentPlayerRanking = (checkStraightFlush(currentPlayerCards)) ? rankingNum : currentPlayerRanking;
                /* Four-of-a-Kind */
                case (3): 
                    currentPlayerRanking = (checkFourOfAKind(currentPlayerCards)) ? rankingNum : currentPlayerRanking;
                /* Full House */
                case (4): 
                    currentPlayerRanking = (checkFullHouse(currentPlayerCards)) ? rankingNum : currentPlayerRanking;
                /* Flush */
                case (5): 
                    currentPlayerRanking = (checkFlush(currentPlayerCards)) ? rankingNum : currentPlayerRanking;
                /* Straight */
                case (6): 
                    currentPlayerRanking = (checkStraight(currentPlayerCards)) ? rankingNum : currentPlayerRanking;
                /* Three-of-a-Kind */
                case (7):
                    currentPlayerRanking = (checkThreeOfAKind(currentPlayerCards)) ? rankingNum : currentPlayerRanking;
                /* Two Pair */
                case (8): 
                    currentPlayerRanking = (checkTwoPair(currentPlayerCards)) ? rankingNum : currentPlayerRanking;
                /* One Pair */
                case (9): 
                    currentPlayerRanking = (checkOnePair(currentPlayerCards)) ? rankingNum : currentPlayerRanking;
                /* High Card */
                case (10):
                    currentPlayerRanking = rankingNum;
            }
            if (currentPlayerRanking != 0) {break;}
            rankingNum++;
        }
        // if they have the same ranking, then previous bestPlayer is still considered the bestPlayer
        if (currentPlayerRanking < bestPlayerRanking) {
            bestPlayer = currentPlayer;
            bestPlayerRanking = currentPlayerRanking;
        }
        currentPlayer = GetNextPlayerWithMoneyAndNotFolded(table, 1, currentPlayer);
    } while (currentPlayer != table.dealerNum);

    return bestPlayer;
}   

/* checks if an array of cards contains a royal flush 
Royal Flush: for every suit, check if 10, J, Q, K, A exist of that suit 
*/
int checkRoyalFlush(Card *cards) {
    int suitColor, i;

    for (suitColor = 0; suitColor < 4; suitColor++) {
        int color10 = 0;
        int colorJ  = 0;
        int colorQ  = 0;
        int colorK  = 0;
        int colorA  = 0;

        for (i = 0; i < 7; i++) {
            if (cards[i].suit == suitColor && cards[i].num == 10) {
                color10 = 1;
            }    
            else if (cards[i].suit == suitColor && cards[i].num == 11) {
                colorJ = 1;
            }
            else if (cards[i].suit == suitColor && cards[i].num == 12) {
                colorQ = 1;
            }
            else if (cards[i].suit == suitColor && cards[i].num == 13) {
                colorK = 1;
            }
            else if (cards[i].suit == suitColor && cards[i].num == 14) {
                colorA = 1;
            }
        }
        if (color10 && colorJ && colorQ && colorK && colorA) {
            /* indicates we have a royal flush*/
            return 1;
        }
    }
    /* indicates no royal flush */
    return 0;
}

/* checks if an array of cards contains a straight flush
Straight Flush: for every suit, call checkStraight()
*/
int checkStraightFlush(Card *cards) {
    /* make sure there exists a flush and a straight; 
        Note: we aren't sure if the flush and straight use the same cards though 
    */
    int numSuit;
    int startCard, startCardPlus1, startCardPlus2, startCardPlus3, startCardPlus4;
    if (checkFlush(cards) && checkStraight(cards)) {
        for (int i = 0; i < 4; i++) {
            numSuit = 0;
            startCard = cards[i].num;
            for (int j = 0; j < 7; j++) {
                if (cards[j].suit == i) {
                    numSuit++;
                    startCardPlus1 = (startCard + 1 == cards[j].num) ? 1 : startCardPlus1;
                    startCardPlus2 = (startCard + 2 == cards[j].num) ? 1 : startCardPlus2;
                    startCardPlus3 = (startCard + 3 == cards[j].num) ? 1 : startCardPlus3;
                    startCardPlus4 = (startCard + 4 == cards[j].num) ? 1 : startCardPlus4;
                }
            }
            if (startCardPlus1 && startCardPlus2 && startCardPlus3 && startCardPlus4 && numSuit >= 5) {
                return 1;
            }
        }

    }
    else {return 0;}
}

/* checks if an array of cards contains four-of-a-kind
Four-of-a-Kind: for every num, if numOfAny == 4
*/
int checkFourOfAKind(Card *cards) {
    int cardNumToCheck, numInstances;

    if (checkThreeOfAKind(cards)) {
        /* i < 4 because if i == 4, we are checking 5th card, & at that point no way to get four-of-a-kind */
        for (int i = 0; i < 4; i++) {
            cardNumToCheck = cards[i].num;
            numInstances = 1;
            for (int j = i + 1; j < 7; j++) {
                numInstances = (cardNumToCheck == cards[j].num) ? numInstances++ : numInstances;
            }
            if (numInstances >= 4) {
                return 1;
            }
        }
    }
    else {return 0;}
}

/* checks if an array of cards contains a full house
Full House: if (checkThreeOfAKind() and checkTwoPair())
*/
int checkFullHouse(Card *cards) {
    if (checkThreeOfAKind(cards) && checkTwoPair(cards)) {
        return 1;
    }
    else {return 0;}
}

/* checks if an array of cards contains a flush
Flush: for every suit, if # suitCards == 5 
*/
int checkFlush(Card *cards) {
    int numSuit;
    for (int i = 0; i < 4; i++) {
        numSuit = 0;
        for (int j = 0; j < 7; j++) {
            numSuit = (cards[j].suit == i) ? numSuit++ : numSuit;
        }
        if (numSuit >= 5) {
            return 1;
        }
    }
    return 0;
}

/* checks if an array of cards contains a straight
Straight: check if five cards are in sequential order (regardless of suit)
*/
int checkStraight(Card *cards) {
    int startCard, startCardPlus1 = 0, startCardPlus2 = 0, startCardPlus3 = 0, startCardPlus4 = 0;
    for (int i = 0; i < 7; i ++) {
        startCard = cards[i].num;
        for (int j = 0; j < 7; j++) {
            startCardPlus1 = (startCard + 1 == cards[j].num) ? 1 : startCardPlus1;
            startCardPlus2 = (startCard + 2 == cards[j].num) ? 1 : startCardPlus2;
            startCardPlus3 = (startCard + 3 == cards[j].num) ? 1 : startCardPlus3;
            startCardPlus4 = (startCard + 4 == cards[j].num) ? 1 : startCardPlus4;
        }
        if (startCardPlus1 && startCardPlus2 && startCardPlus3 && startCardPlus4) {
            return 1;
        }
    }
    return 0;
}

/* checks if an array of cards contains three-of-a-kind
Three-of-a-kind: for every num, if numOfAny == 3 
*/
int checkThreeOfAKind(Card *cards) {
    int cardNumToCheck, numInstances;
    /* i < 5 because if i == 5, we are checking 6th card, & at that point no way to get three-of-a-kind */
    if (checkOnePair(cards)) {
        for (int i = 0; i < 5; i++) {
            cardNumToCheck = cards[i].num;
            numInstances = 1;
            for (int j = i + 1; j < 7; j++) {
                numInstances = (cardNumToCheck == cards[j].num) ? numInstances++ : numInstances;
            }
            if (numInstances >= 3) {
                return 1;
            }
        }
    }
    else {return 0;}
}

/* checks if an array of cards contains a two pair
Two Pair: for every num, keep iterating and check if the number of pairs that exist >= 2
*/
int checkTwoPair(Card *cards) {
    /* first make sure one pair exists */
    if (checkOnePair(cards)) {
        int numForPair;
        int pairs = 0;

        for (int i = 0; i < 6; i++) {
            numForPair = cards[i].num;
            for (int j = i + 1; j < 7; j++) {
                pairs = (cards[j].num == numForPair) ? pairs++ : pairs;
            }
        }
        /* note: we could have 2 or 3 pairs here bc if 3 pairs that means 2 pairs exist*/
        if (pairs >= 2) {return 1;}
        else {return 0;}
    }
    else {return 0;}
}

/* checks if an array of cards contains a one pair
One Pair: for every num, if numOfAny == 2 exists 
*/
int checkOnePair(Card *cards) {
    int numForPair;
    for (int i = 0; i < 6; i++) {
        numForPair = cards[i].num;
        for (int j = i + 1; j < 7; j++) {
            if (cards[j].num == numForPair) {
                return 1;
            }
        }
    }
    return 0;
}

//send gamestate data to correct player
void SendGameState(Table table, int PlayerNum);

void RequestPlayerInput(Table table, int PlayerNum);

Table GivePocketCards(Table table) {
    int playerNum;
    //give each player 1 card starting at the smallblind, cyling through the table
    //do it once more such that each player has 2 pocket cards

    //start at small blind
    playerNum = GetNextPlayerWithMoney(table, 1, table.dealerNum);

    for (int j = 0; j < 2; j++) {
        for (int i = 0; i < table.totActivePlayers; i++) {
            //give player a card
            table.players[playerNum].pocket[j] = DrawCard(&table.deck);
            /* insert network function (maybe) */
            playerNum = GetNextPlayerWithMoney(table, 1, playerNum);
        }
    }
    return table;
}

Table Flop(Table table) {
    for (int i = 0; i < 3; i++) {
        table.tableCards[i] = DrawCard(&table.deck);
        //display something on gui's ?
    }
    return table;
}

//enter 4 or 5 for tableCardNum to display 4th or 5th round
Table Show4or5Card(Table table, int tableCardNum) {
    assert(tableCardNum == 4 || tableCardNum == 5);
    table.tableCards[tableCardNum - 1] = DrawCard(&table.deck);
    //display something on gui's ?
    return table;
}

//call when only 1 active player remaining
int FindLastPlayer(Table table) {
    int currentPlayer;
    currentPlayer = table.dealerNum;
    do {
        currentPlayer = GetNextPlayerWithMoneyAndNotFolded(table, 1, currentPlayer);
    } while(table.players[currentPlayer].hasFolded == folded);
    return currentPlayer;
}

//awards player who wins by being last remaining
Table AwardLastPlayer(Table table) {
    assert(table.totActivePlayers == 1);
    int winningPlayer;
    winningPlayer = FindLastPlayer(table);
    table.players[winningPlayer].money += table.pot;
    table.pot = 0;
    //update gui's without showing cards on clients
    return table;
}

Table Showdown(Table table) {
    assert(table.totActivePlayers > 1);
    int winningPlayer;
    winningPlayer = BestHand(table);
    table.players[winningPlayer].money += table.pot;
    table.pot = 0;
    //update gui's, and show everyone's cards
    return table;
}

//reseting everything for a new hand
Table NewHand(Table table) {
    int activePlayers;
    //updating table info
    table.deck = CreateDeck();
    table.currentMinBet = 0;
    table.dealerNum = GetNextPlayerWithMoney(table, 1, table.dealerNum);
    table.pot = 0;
    for (int i = 0; i < 5; i++) {
        table.tableCards[i].num = blankNum;
        table.tableCards[i].suit = blankSuit;
    }
    activePlayers = 0;
    for (int i = 0; i < table.totPlayers; i++) {
        if (table.players[i].money > 0) { activePlayers++; }
    }
    table.totActivePlayers = activePlayers;

    //update every player's info
    for (int i = 0; i < table.totPlayers; i++) {
        //reset every player's pockets to be blanks
        for (int j = 0; j < 2; j++) { 
            table.players[i].pocket[j].num = blankNum;
            table.players[i].pocket[j].suit = blankSuit;
        }
        table.players[i].currentBetAmount = 0;
        table.players[i].hasFolded = notfolded;
        table.players[i].token = none;
    }
    //setting tokens
    assert(activePlayers >= 3);
    //assumes dealNum has been updated
    table.players[table.dealerNum].token = dealer;
    table.players[GetNextPlayerWithMoney(table, 1, table.dealerNum)].token = smallBlind;
    table.players[GetNextPlayerWithMoney(table, 2, table.dealerNum)].token = bigBlind;

    return table;
}

int PlayersWithMoney(Table table) {
    int playersWithMoney;
    playersWithMoney = 0;
    for (int i = 0; i < table.totPlayers; i++) {
        if (table.players[i].money > 0) { playersWithMoney++; }
    }
    return playersWithMoney;
}

int findOnlyPlayerWithMoney(Table table) {
    assert(PlayersWithMoney(table) == 1);
    int playerWithMoney;
    for (int i = 0; i < table.totPlayers; i++) {
        if (table.players[i].money > 0) { 
            playerWithMoney = i; 
            }
    }
    return playerWithMoney;
}

#endif