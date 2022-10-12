#include "table.h"
///#include "network.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>

//#define DEBUG
//#define TEST

Table CreateTable(int numOfPlayers, int startingMoney) {
    Table table;
#ifdef DEBUG 
    printf("allocating players in memory\n");
#endif
    table.players = malloc(sizeof(Player) * numOfPlayers);
#ifdef DEBUG 
    printf("creating deck\n");
#endif
    table.deck = CreateDeck();
    table.totPlayers = numOfPlayers;
    table.totActivePlayers = numOfPlayers;
    table.pot = 0;
    table.dealerNum = 0; //for now, could change later
    table.currentMinBet = 0;
#ifdef DEBUG 
    printf("setting table cards to blanks\n");
#endif
    for (int i = 0; i < 5; i++) { 
        table.tableCards[i].num = blankNum; 
        table.tableCards[i].suit = blankSuit;
    }
    
    /* setting player information */
    //setting tokens
    table.players[table.dealerNum].token = dealer;
    table.players[GetNextPos(table, table.dealerNum)].token = smallBlind;
    table.players[GetNextPos(table, GetNextPos(table, table.dealerNum))].token = bigBlind;
    for (int i = 3; i < numOfPlayers; i++) {
        table.players[i].token = none;
    }
    //setting all other information
    for (int i = 0; i < numOfPlayers; i++) {
        table.players[i].money = startingMoney;
        table.players[i].pocket[0].num = blankNum;
        table.players[i].pocket[0].suit = blankSuit;
        table.players[i].pocket[1].num = blankNum;
        table.players[i].pocket[1].suit = blankSuit;
        table.players[i].moveChoice = null;
        table.players[i].currentBetAmount = 0;
        table.players[i].position = i; //could change to i+1
        //strcpy(table.players[i].username, "Unknown_User");
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
        if (table.players[finalPos].moveChoice == fold) { continue; }
        i++;
    }
    return finalPos;
}


//prints move choice
void TranslateMoveChoice(MoveChoice choice) {
    switch (choice)
    {
    case 0:
        printf("null");
        break;
    case 1:
        printf("check");
        break;
    case 2:
        printf("call");
        break;
    case 3:
        printf("raise");
        break;
    case 4:
        printf("fold");
        break;
    case 5:
        printf("blind");
        break;
    default:
        printf("unknown move");
        break;
    }
}


// PrintCurrentBalance Function updated
//#ifdef TEST
void PrintCurrentBalance(Table table) {
    printf("Pot: %d\n", table.pot);
    printf("Current Bet Amount: %d\n", table.currentMinBet);
    printf("Community Cards (num/suit):");
    for (int i = 0; i < 5; i++) {
        printf(" %d/%d ", table.tableCards[i].num, table.tableCards[i].suit);
    }
    printf("\nPlayers Balances:\t");
    for (int i = 0; i < table.totPlayers; i++) {
        printf("(%d)%d\t\t", i, table.players[i].money);
    }
    printf("\nPlayer Tokens:\t\t");
    for (int i = 0; i < table.totPlayers; i++) {
        printf("(%d)%d\t\t", i, table.players[i].token);
    }
    printf("\nPlayer Choices:\t\t");
    for (int i = 0; i < table.totPlayers; i++) {
        printf("(%d)", i);
        TranslateMoveChoice(table.players[i].moveChoice);
        printf("   \t");
    }
    printf("\nPlayer Current Bets:\t");
    for (int i = 0; i < table.totPlayers; i++) {
        printf("(%d)%d\t\t", i, table.players[i].currentBetAmount);
    }
    printf("\nPocket Cards (num/suit):");
    for (int i = 0; i < table.totPlayers; i++) {
        printf("(%d)%d/%d ", i, table.players[i].pocket[0].num, table.players[i].pocket[0].suit);
        printf("%d/%d\t", table.players[i].pocket[1].num, table.players[i].pocket[1].suit);
    }
    printf("\n\n");
}
//#endif

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

#ifdef TEST
    printf("start Big and Small Blind function\n");
#endif

    //if player all-in's
    if (table.players[smallblind_pos].money < smallblind) {
        smallblind = table.players[smallblind_pos].money;
    }

    if (table.currentMinBet < smallblind) { table.currentMinBet = smallblind; }
    table.pot += smallblind;
    table.players[smallblind_pos].moveChoice = blind;
    table.players[smallblind_pos].currentBetAmount = smallblind;
    table.players[smallblind_pos].money -= smallblind;

#ifdef TEST
    printf("placed small blind\n");
    PrintCurrentBalance(table);
#endif

    if (table.players[bigblind_pos].money < bigblind) {
        bigblind = table.players[bigblind_pos].money;
    }
    if (table.currentMinBet < bigblind) { table.currentMinBet = bigblind; }
    table.pot += bigblind;
    table.players[bigblind_pos].moveChoice = blind;
    table.players[bigblind_pos].currentBetAmount = bigblind;
    table.players[bigblind_pos].money -= bigblind;
    

#ifdef TEST
    printf("placed big blind\n");
    PrintCurrentBalance(table);
#endif

    return table;
}


//call before a betting round, set minBet to 0 for all but the first betting round
Table ResetBets(Table table, int minBet) {
    //resetting player info
    for (int i = 0; i < table.totPlayers; i++) {
        if      (table.players[i].moveChoice == blind) { /* do nothing */ }
        else if (table.players[i].moveChoice == fold) {table.players[i].currentBetAmount = 0;}
        else    {
            table.players[i].currentBetAmount = 0;
            table.players[i].moveChoice = null;
        }
    }
    //resetting table info
    table.currentMinBet = minBet;
    return table;
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


Move PlayerMoveChoice_Test(int playerNum) {
    Move moveChoice;
    int choiceInput, choiceAmount;
    printf("Player[%d] Move Choice (type 5 for help): ", playerNum);
    scanf("%d", &choiceInput);
    while (choiceInput >= 5 || choiceInput <= 0) {
        printf("1: Check\n2: Call\n3: Raise\n4: Fold\n\n");
        printf("Player[%d] Move Choice (type 5 for help): ", playerNum);
        scanf("%d", &choiceInput);
    }
    if (choiceInput == raised) {
        printf("Enter Raise Amount: ");
        scanf("%d", &choiceAmount);
        assert(choiceAmount > 0);
    }
    else { choiceAmount = 0; }
    moveChoice.choice = choiceInput;
    moveChoice.raiseAmount = choiceAmount;
    return moveChoice;
}




//ends with all active players either folding, or placing bets
Table BettingRound(Table table, int startingPlayer) {
    int currentPlayer;
    int startingPlayerFolded = 0; //used only if a starting player ever folds
    Move userMove;
    int moneyGiven;
    //assume that there is more than one player who has not folded
    assert(table.totActivePlayers > 1);

#ifdef TEST
    printf("\nstart betting round (reset players choice)\n\n\n");
    PrintCurrentBalance(table);
#endif

    currentPlayer = startingPlayer;
    printf("starting betting round do loop\n"); /* remove later */

    do {
        if (table.totActivePlayers == 1) { 
            break;
        }

        if (startingPlayerFolded == 1) { 
            startingPlayer = currentPlayer;
            startingPlayerFolded = 0; //reset
        }

        //could update players client here, or wait till we do it for everybodys
        assert(table.players[currentPlayer].moveChoice != fold);
        if (table.players[currentPlayer].money == 0) {
            //player has already all-in'ed for this hand, so we skip their choice
            PrintCurrentBalance(table);
            currentPlayer = GetNextPlayerWhoHasNotFolded(table, 1, currentPlayer); 
            continue;
        }

        PrintCurrentBalance(table);

        userMove = PlayerMoveChoice_Test(currentPlayer);

        //userMove has been decided by player, but may need to be modified if not entirely legal

        if (userMove.choice == fold) {
            table.players[currentPlayer].moveChoice = fold;
            table.totActivePlayers--;
            if (currentPlayer == startingPlayer) {
                startingPlayerFolded = 1;
                printf("player has folded, new starting player: %d\n", startingPlayer);
            }
            currentPlayer = GetNextPlayerWhoHasNotFolded(table, 1, currentPlayer);
            continue;
        }

        if (userMove.choice != raised) {userMove.raiseAmount = 0;}

        moneyGiven = userMove.raiseAmount + (table.currentMinBet - table.players[currentPlayer].currentBetAmount);

        /* brand new all-in scenario */
        if (moneyGiven > table.players[currentPlayer].money) {
            //then play is considered an all-in
            int checkAmountNeededToGive = table.currentMinBet - table.players[currentPlayer].currentBetAmount;
            if (checkAmountNeededToGive >= table.players[currentPlayer].money) {
                userMove.raiseAmount = 0;
                userMove.choice = check;
            }
            else if (userMove.choice == raised) {
                userMove.raiseAmount = table.players[currentPlayer].money - checkAmountNeededToGive;
            }
            moneyGiven = table.players[currentPlayer].money;
        }

        //updating table info
        table.currentMinBet = userMove.raiseAmount + table.currentMinBet;
        table.pot = table.pot + moneyGiven;
        //updating player's info
        table.players[currentPlayer].moveChoice = userMove.choice;
        table.players[currentPlayer].currentBetAmount = table.players[currentPlayer].currentBetAmount + moneyGiven;
        table.players[currentPlayer].money = table.players[currentPlayer].money - moneyGiven;
        //updating where betting round ends on
        if (userMove.choice == raised) {
            startingPlayer = currentPlayer;
            printf("player has raised, new starting player: %d\n", startingPlayer);
        }

        currentPlayer = GetNextPlayerWhoHasNotFolded(table, 1, currentPlayer); 
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


int GetNextPlayerWhoHasNotFolded(Table table, int playersAhead, int startingPos) {
    int finalPos = startingPos;
    int i = 0;
    while (i < playersAhead) {
        finalPos = GetNextPos(table, finalPos);
        if (table.players[finalPos].moveChoice == fold) { continue; }
        i++;
    }
    return finalPos;
}

int GetPrevPlayerWhoHasNotFolded(Table table, int playersAhead, int startingPos) {
    int finalPos = startingPos;
    int i = 0;
    while (i < playersAhead) {
        finalPos = GetPrevPos(table, finalPos);
        if (table.players[finalPos].moveChoice == fold) { continue; }
        i++;
    }
    return finalPos;
}


// Give PocketCards Function updated
Table GivePocketCards(Table table) {
    int playerNum;
    //give each player 1 card starting at the smallblind, cyling through the table
    //do it once more such that each player has 2 pocket cards

    //start at small blind
    playerNum = GetNextPlayerWhoHasNotFolded(table, 1, table.dealerNum);

    for (int j = 0; j < 2; j++) {
        for (int i = 0; i < table.totActivePlayers; i++) {
            //give player a card
            table.players[playerNum].pocket[j] = DrawCard(&table.deck);
            /* insert network function (maybe) */
            playerNum = GetNextPlayerWhoHasNotFolded(table, 1, playerNum);
        }
    }
#ifdef TEST
    printf("delt pocket cards\n");
    PrintCurrentBalance(table);
#endif
    return table;
}

Table Flop(Table table) {
    for (int i = 0; i < 3; i++) {
        table.tableCards[i] = DrawCard(&table.deck);
        //display something on gui's ?
    #ifdef TEST
        printf("card %d: %d/%d\n", i + 1, table.tableCards[i].num, table.tableCards[i].suit);
    #endif
    }
#ifdef TEST
    printf("done with flop\n\n");
    PrintCurrentBalance(table);
#endif
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
    currentPlayer = GetNextPlayerWhoHasNotFolded(table, 1, currentPlayer);
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
#ifdef TEST
    printf("\nthe winner of this hand was player %d!\n", winningPlayer);
    printf("end of showdown: awarded money to winning player and reset pot\n");
    PrintCurrentBalance(table);
#endif
    //update gui's, and show everyone's cards
    return table;
}


//reseting everything for a new hand, does not move tokens
Table NewHand(Table table) {
    int activePlayers;
    //updating table info
#ifdef DEBUG
    printf("creating deck\n");
#endif
    table.deck = CreateDeck();
    table.currentMinBet = 0;
#ifdef DEBUG
    printf("Getting next player\n");
#endif
    table.pot = 0;
#ifdef DEBUG
    printf("setting table cards to blank\n");
#endif
    for (int i = 0; i < 5; i++) {
        table.tableCards[i].num = blankNum;
        table.tableCards[i].suit = blankSuit;
    }
#ifdef DEBUG
    printf("counting active players\n");
#endif
    activePlayers = 0;
    //update every player's info
    for (int i = 0; i < table.totPlayers; i++) {
        //reset every player's pockets to be blanks
        for (int j = 0; j < 2; j++) { 
            table.players[i].pocket[j].num = blankNum;
            table.players[i].pocket[j].suit = blankSuit;
        }
        if (table.players[i].money > 0) { 
            activePlayers++; 
            table.players[i].moveChoice = null;
        }
        else {
            table.players[i].moveChoice = fold;
        }
        table.players[i].currentBetAmount = 0;
        table.players[i].token = none;
    }
    table.totActivePlayers = activePlayers;
    return table;
}

//assumes NewHand has already been called
Table MoveTokens(Table table) {
    assert(table.totActivePlayers >= 2);
    table.dealerNum = GetNextPlayerWithMoney(table, 1, table.dealerNum);
    if (PlayersWithMoney(table) >= 3) {
        table.players[table.dealerNum].token = dealer;
        table.players[GetNextPlayerWithMoney(table, 1, table.dealerNum)].token = smallBlind;
        table.players[GetNextPlayerWithMoney(table, 2, table.dealerNum)].token = bigBlind;
    }
    else {
        //2 players remain at beginning of hand
        /* assumed rule(could change):  each player will have either a smallBlind or bigBlind token 
        and neither player's.token will equal the dealer, however table.dealerNum will still be set(as normal) to first active player after previous dealer
        This means that the dealer(on the table side) will also be the blind
        */                              
        table.players[GetNextPlayerWithMoney(table, 1, table.dealerNum)].token = smallBlind;
        table.players[GetNextPlayerWithMoney(table, 2, table.dealerNum)].token = bigBlind;
    }
    return table;
}

int PlayersWithMoney(Table table) {
    int playersWithMoney;
    playersWithMoney = 0;
    for (int i = 0; i < table.totPlayers; i++) {
        //accounts for players who have all-in'ed
        if (table.players[i].money > 0 || table.players[i].currentBetAmount > 0) { playersWithMoney++; }
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



/* BEST HAND STUFF */


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
                pairs = (cards[j].num == numForPair) ? (pairs + 1) : pairs;
            }
        }
        /* note: we could have 2 or 3 pairs here bc if 3 pairs that means 2 pairs exist*/
        if (pairs >= 2) {return 1;}
        else {return 0;}
    }
    else {return 0;}
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
                numInstances = (cardNumToCheck == cards[j].num) ? (numInstances + 1) : numInstances;
            }
            if (numInstances >= 3) {
                return 1;
            }
        }
    }
    return 0;
}

/* checks if an array of cards contains a straight
Straight: check if five cards are in sequential order (regardless of suit)
*/
int checkStraight(Card *cards) {
    int startCard, startCardPlus1, startCardPlus2, startCardPlus3, startCardPlus4;
    int sc_p1, sc_p2, sc_p3, sc_p4;
    for (int i = 0; i < 7; i ++) {
        startCard = cards[i].num;
        startCardPlus1 = 0;
        startCardPlus2 = 0;
        startCardPlus3 = 0;
        startCardPlus4 = 0;
        for (int j = 0; j < 7; j++) {
            sc_p1 = (startCard + 1 > 14) ? startCard - 14 + 2 : startCard + 1;
            sc_p2 = (startCard + 2 > 14) ? startCard - 14 + 3 : startCard + 2;
            sc_p3 = (startCard + 3 > 14) ? startCard - 14 + 4 : startCard + 3;
            sc_p4 = (startCard + 4 > 14) ? startCard - 14 + 5 : startCard + 4;
            if (sc_p1 == cards[j].num) {
                startCardPlus1 = 1;
            #ifdef DEBUG
                printf("card is in a possible straight flush!\n");
            #endif
            }
            else if (sc_p2 == cards[j].num) {
                startCardPlus2 = 1;
            #ifdef DEBUG
                printf("card is in a possible straight flush!\n");
            #endif
            }
            else if (sc_p3 == cards[j].num) {
                startCardPlus3 = 1;
            #ifdef DEBUG
                printf("card is in a possible straight flush!\n");
            #endif
            }
            else if (sc_p4 == cards[j].num) {
                startCardPlus4 = 1;
            #ifdef DEBUG
                printf("card is in a possible straight flush!\n");
            #endif
            }
        }
        if (startCardPlus1 && startCardPlus2 && startCardPlus3 && startCardPlus4) {
            return 1;
        }
    }
    return 0;
}

/* checks if an array of cards contains a flush
Flush: for every suit, if # suitCards == 5 
*/
int checkFlush(Card *cards) {
    int numSuit;
    for (int i = 0; i < 4; i++) {
        numSuit = 0;
        for (int j = 0; j < 7; j++) {
            numSuit = (cards[j].suit == i) ? (numSuit + 1) : numSuit;
        }
        if (numSuit >= 5) {
            return 1;
        }
    }
    return 0;
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
                numInstances = (cardNumToCheck == cards[j].num) ? (numInstances + 1) : numInstances;
            }
            if (numInstances >= 4) {
                return 1;
            }
        }
    }
    return 0;
}

/* checks if an array of cards contains a straight flush
Straight Flush: for every suit, call checkStraight()
*/
int checkStraightFlush(Card *cards) {
    /* make sure there exists a flush and a straight; 
        Note: we aren't sure if the flush and straight use the same cards though 
    */
#ifdef DEBUG
    printf("checking for straight flush..\n");
#endif
    int numSuit;
    int startCard, startCardPlus1, startCardPlus2, startCardPlus3, startCardPlus4;
    int sc_p1, sc_p2, sc_p3, sc_p4;
    if (checkFlush(cards) && checkStraight(cards)) {
    #ifdef DEBUG
        printf("flush and straight exist. checking existence for straight flush...\n\n");
    #endif
        for (int i = 0; i < 4; i++) {
    #ifdef DEBUG
            printf("suit %d\n", i);
    #endif
            for (int k = 0; k < 7; k++) {
                numSuit = 0;
                startCardPlus1 = 0;
                startCardPlus2 = 0;
                startCardPlus3 = 0;
                startCardPlus4 = 0;
                startCard = cards[k].num;
                #ifdef DEBUG
                printf("\n\ncurrent card value of start card: %d/%d\n", startCard, cards[k].suit);
                #endif
                for (int j = 0; j < 7; j++) {
                    #ifdef DEBUG
                    printf("* testing card number %d of suit %d\n", cards[j].num, cards[j].suit);
                    #endif
                    if (cards[j].suit == i) {
                        numSuit++;
                        sc_p1 = (startCard + 1 > 14) ? startCard - 14 + 2 : startCard + 1;
                        sc_p2 = (startCard + 2 > 14) ? startCard - 14 + 3 : startCard + 2;
                        sc_p3 = (startCard + 3 > 14) ? startCard - 14 + 4 : startCard + 3;
                        sc_p4 = (startCard + 4 > 14) ? startCard - 14 + 5 : startCard + 4;
                        if (sc_p1 == cards[j].num) {
                            startCardPlus1 = 1;
                            #ifdef DEBUG
                            printf("card is in a possible straight flush!\n");
                            #endif
                        }
                        else if (sc_p2 == cards[j].num) {
                            startCardPlus2 = 1;
                            #ifdef DEBUG
                            printf("card is in a possible straight flush!\n");
                            #endif
                        }
                        else if (sc_p3 == cards[j].num) {
                            startCardPlus3 = 1;
                            #ifdef DEBUG
                            printf("card is in a possible straight flush!\n");
                            #endif
                        }
                        else if (sc_p4 == cards[j].num) {
                            startCardPlus4 = 1;
                            #ifdef DEBUG
                            printf("card is in a possible straight flush!\n");
                            #endif
                        }
                    }
                }
                #ifdef TEST
                printf("number of cards of suit %d: %d\n", i, numSuit);
                printf("\nsummary for iteration no.%d of suit %d\n", k, i);
                printf("%d - start card number\n", startCard);
                printf("%d - existence of start card + 1 number\n", startCardPlus1);
                printf("%d - existence of start card + 2 number\n", startCardPlus2);
                printf("%d - existence of start card + 3 number\n", startCardPlus3);
                printf("%d - existence of start card + 4 number\n", startCardPlus4);
                #endif
                if (startCardPlus1 && startCardPlus2 && startCardPlus3 && startCardPlus4 && numSuit >= 5) {
                    #ifdef TEST
                    printf("straight flush exists!");
                    #endif
                    return 1;
                }
            }
        }
    }
    return 0;
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


//prints hand ranking
void TranslateHandRanking(int ranking) {
    switch (ranking)
    {
    case 1:
        printf("Royal Flush");
        break;
    case 2:
        printf("Straight Flush");
        break;
    case 3:
        printf("Four-of-a-Kind");
        break;
    case 4:
        printf("Full House");
        break;
    case 5:
        printf("Flush");
        break;
    case 6:
        printf("Straight");
        break;
    case 7:
        printf("Three-of-a-Kind");
        break;
    case 8:
        printf("Two Pair");
        break;
    case 9:
        printf("One Pair");
        break;
    case 10:
        printf("High Card");
        break;
    default:
        printf("unknown hand!");
        break;
    }
}


//returns position of player who wins has best hand on table
int BestHand(Table table) {
    //iterates through table of players who have not folded
    //returns winner

    /* hand rankings: 1 - 10 with 1 being the best. */
    assert (table.totActivePlayers > 1);

    int bestPlayer;
    int currentPlayer = table.dealerNum;

    int currentPlayerRanking;
    int bestPlayerRanking = 11;
    
    int rankingNum;

    bestPlayer = currentPlayer;
    do {
        currentPlayerRanking = 0;
        rankingNum = 1;
        Card currentPlayerCards[7] = {
            table.tableCards[0],
            table.tableCards[1], 
            table.tableCards[2],
            table.tableCards[3],
            table.tableCards[4],
            table.players[currentPlayer].pocket[0],
            table.players[currentPlayer].pocket[1],  
        };
        #ifdef TEST
        int testVar = 11;
        printf("\nchecking best hand for player %d...\n", currentPlayer);
        printf("showing player %d's cards to build the hand from:", currentPlayer);
        for (int i = 0; i < 7; i++) {
            printf(" %d/%d", currentPlayerCards[i].num, currentPlayerCards[i].suit);
        }
        printf("\n\n");
        #endif
        while (rankingNum <= 10) {
            switch(rankingNum) {    
                /* Royal Flush */
                case (1):
                    #ifdef TEST
                    printf("checking for Royal Flush...");
                    #endif
                    currentPlayerRanking = (checkRoyalFlush(currentPlayerCards)) ? rankingNum : currentPlayerRanking;
                    #ifdef TEST
                    if (currentPlayerRanking != 0) {
                        printf("\texists!\n");
                        testVar = (currentPlayerRanking < testVar) ? currentPlayerRanking : testVar;
                        currentPlayerRanking = 0;
                        rankingNum++;
                        continue;
                    }
                    else {
                        printf("\tplayer %d doesn't have it\n", currentPlayer);
                    }
                    #endif
                    break;
                /* Straight Flush */
                case (2):
                    #ifdef TEST
                    printf("checking for Straight Flush...");
                    #endif
                    currentPlayerRanking = (checkStraightFlush(currentPlayerCards)) ? rankingNum : currentPlayerRanking;
                    #ifdef TEST
                    if (currentPlayerRanking != 0) {
                        printf("\texists!\n");
                        testVar = (currentPlayerRanking < testVar) ? currentPlayerRanking : testVar;
                        currentPlayerRanking = 0;
                        rankingNum++;
                        continue;
                    }
                    else {
                        printf("\tplayer %d doesn't have it\n", currentPlayer);
                    }
                    #endif
                    break;
                /* Four-of-a-Kind */
                case (3):
                    #ifdef TEST
                    printf("checking for Four-of-a-Kind...");
                    #endif
                    currentPlayerRanking = (checkFourOfAKind(currentPlayerCards)) ? rankingNum : currentPlayerRanking;
                    #ifdef TEST
                    if (currentPlayerRanking != 0) {
                        printf("\texists!\n");
                        testVar = (currentPlayerRanking < testVar) ? currentPlayerRanking : testVar;
                        currentPlayerRanking = 0;
                        rankingNum++;
                        continue;
                    }
                    else {
                        printf("\tplayer %d doesn't have it\n", currentPlayer);
                    }
                    #endif
                    break;
                /* Full House */
                case (4):
                    #ifdef TEST
                    printf("checking for Full House...");
                    #endif
                    currentPlayerRanking = (checkFullHouse(currentPlayerCards)) ? rankingNum : currentPlayerRanking;
                    #ifdef TEST
                    if (currentPlayerRanking != 0) {
                        printf("\texists!\n");
                        testVar = (currentPlayerRanking < testVar) ? currentPlayerRanking : testVar;
                        currentPlayerRanking = 0;
                        rankingNum++;
                        continue;
                    }
                    else {
                        printf("\tplayer %d doesn't have it\n", currentPlayer);
                    }
                    #endif
                    break;
                /* Flush */
                case (5):
                    #ifdef TEST
                    printf("checking for Flush...");
                    #endif
                    currentPlayerRanking = (checkFlush(currentPlayerCards)) ? rankingNum : currentPlayerRanking;
                    #ifdef TEST
                    if (currentPlayerRanking != 0) {
                        printf("\t\texists!\n");
                        testVar = (currentPlayerRanking < testVar) ? currentPlayerRanking : testVar;
                        currentPlayerRanking = 0;
                        rankingNum++;
                        continue;
                    }
                    else {
                        printf("\t\tplayer %d doesn't have it\n", currentPlayer);
                    }
                    #endif
                    break;
                /* Straight */
                case (6):
                    #ifdef TEST
                    printf("checking for Straight...");
                    #endif
                    currentPlayerRanking = (checkStraight(currentPlayerCards)) ? rankingNum : currentPlayerRanking;
                    #ifdef TEST
                    if (currentPlayerRanking != 0) {
                        printf("\texists!\n");
                        testVar = (currentPlayerRanking < testVar) ? currentPlayerRanking : testVar;
                        currentPlayerRanking = 0;
                        rankingNum++;
                        continue;
                    }
                    else {
                        printf("\tplayer %d doesn't have it\n", currentPlayer);
                    }
                    #endif
                    break;
                /* Three-of-a-Kind */
                case (7):
                    #ifdef TEST
                    printf("checking for Three-of-a-Kind...");
                    #endif
                    currentPlayerRanking = (checkThreeOfAKind(currentPlayerCards)) ? rankingNum : currentPlayerRanking;
                    #ifdef TEST
                    if (currentPlayerRanking != 0) {
                        printf("\texists!\n");
                        testVar = (currentPlayerRanking < testVar) ? currentPlayerRanking : testVar;
                        currentPlayerRanking = 0;
                        rankingNum++;
                        continue;
                    }
                    else {
                        printf("\tplayer %d doesn't have it\n", currentPlayer);
                    }
                    #endif
                    break;
                /* Two Pair */
                case (8):
                    #ifdef TEST
                    printf("checking for Two Pair...");
                    #endif
                    currentPlayerRanking = (checkTwoPair(currentPlayerCards)) ? rankingNum : currentPlayerRanking;
                    #ifdef TEST
                    if (currentPlayerRanking != 0) {
                        printf("\texists!\n");
                        testVar = (currentPlayerRanking < testVar) ? currentPlayerRanking : testVar;
                        currentPlayerRanking = 0;
                        rankingNum++;
                        continue;
                    }
                    else {
                        printf("\tplayer %d doesn't have it\n", currentPlayer);
                    }
                    #endif
                    break;
                /* One Pair */
                case (9):
                    #ifdef TEST
                    printf("checking for One Pair...");
                    #endif
                    currentPlayerRanking = (checkOnePair(currentPlayerCards)) ? rankingNum : currentPlayerRanking;
                    #ifdef TEST
                    if (currentPlayerRanking != 0) {
                        printf("\texists!\n");
                        testVar = (currentPlayerRanking < testVar) ? currentPlayerRanking : testVar;
                        currentPlayerRanking = 0;
                        rankingNum++;
                        continue;
                    }
                    else {
                        printf("\tplayer %d doesn't have it\n", currentPlayer);
                    }
                    #endif
                    break;
                /* High Card */
                case (10):
                    currentPlayerRanking = rankingNum;
                    #ifdef TEST
                    printf("checking for High Card...\texists!\n");
                    testVar = (currentPlayerRanking < testVar) ? currentPlayerRanking : testVar;
                    currentPlayerRanking = 0;
                    #endif
                    break;
            }
            if (currentPlayerRanking != 0) {break;}
            rankingNum++;
        }
        #ifdef TEST
        currentPlayerRanking = testVar;
        printf("player %d's best hand ranking: (%d) ", currentPlayer, currentPlayerRanking);
        TranslateHandRanking(currentPlayerRanking);
        printf("\n");
        #endif
        /* 
            if they have the same ranking, then previous bestPlayer is still considered the bestPlayer 

            Note: more logic needs to be added here (case where multiple players have the same ranking)
        */
        if (currentPlayerRanking < bestPlayerRanking) {
            bestPlayer = currentPlayer;
            bestPlayerRanking = currentPlayerRanking;
            #ifdef TEST
            printf("\nnew best player: %d\n", bestPlayer);
            printf("best hand ranking: %d\n\n", bestPlayerRanking);
            #endif
        }
        currentPlayer = GetNextPlayerWhoHasNotFolded(table, 1, currentPlayer);
    } while (currentPlayer != table.dealerNum);
    #ifdef TEST
    printf("\nunofficially, the best player is: %d\nwith a hand ranking of: (%d)", bestPlayer, bestPlayerRanking);
    TranslateHandRanking(bestPlayerRanking);
    printf("\n");
    #endif
    return bestPlayer;
}   