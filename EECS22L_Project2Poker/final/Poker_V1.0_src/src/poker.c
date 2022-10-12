#include "table.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define DEBUG
#define TEST

//temporary testing program that demonstrates game logic of poker
//to compile: gcc -Wall -std=c11 poker.c deck.c table.c -o poker

int main(void) {
    int totalPlayers, startingMoney;
    int smallBlind, bigBlind;
    int playerOfInterest, winningPlayer;
    int handNum;
    Table table;
    /*
    printf("please enter the total number of players: ");
    scanf("%d", &totalPlayers);
    printf("please enter the starting cash for each player: ");
    scanf("%d", &startingMoney);
    */

    startingMoney = 50;
    totalPlayers = 3;
    smallBlind = 4;
    bigBlind = 8;
#ifdef DEBUG
    printf("creating table\n");
#endif
    table = CreateTable(totalPlayers, startingMoney);    

    handNum = 0;
    //assume that board is being displayed current, assume board is graphically kept up to date

    /*** start of game ***/

    
    do
    { 
    #ifdef DEBUG
        printf("cycling board\n");
    #endif  
        //cycle board
    #ifdef DEBUG
        printf("creating a new hand\n");
    #endif  
        if (handNum == 0) { /* dont move tokens*/ }
        else  { 
            //must call NewHand() before MoveTokens()
            table = NewHand(table);
            table = MoveTokens(table);
        }

    #ifdef DEBUG
        printf("shuffling hand\n");
    #endif  
        //shuffle the deck
        table.deck = ShuffleDeck(table.deck);
    #ifdef DEBUG
        printf("Setting Small and Big Blinds\n");
    #endif
        //small blind is determined (initialize smallBlind)
        //big blind is determined (ex: minBet = bigBlind * 2)
        table = SetBigAndSmallBlind(table, smallBlind, bigBlind);
    #ifdef DEBUG
        printf("Giving Pocket Cards\n");
    #endif
        //hand out pocket cards
        table = GivePocketCards(table);
    
        //betting starts on player after the big blind
    #ifdef DEBUG
        printf("getting next player who has not folded:");
    #endif  
        playerOfInterest = GetNextPlayerWhoHasNotFolded(table, 3, table.dealerNum);
    #ifdef DEBUG
        printf("%d\n", playerOfInterest);
    #endif  
        //if big blind chooses to NOT raise, then pre-flop betting finishes
        //else big blind player has raised and must cycle through again and finish on small blind's completeion
    #ifdef DEBUG 
        printf("resetting bets\n");
    #endif
        table = ResetBets(table, table.currentMinBet);
    #ifdef DEBUG
        printf("starting betting round with minimum bet of %d\n", table.currentMinBet);
    #endif
        table = BettingRound(table, playerOfInterest);
        if (table.totActivePlayers == 1) {
    #ifdef TEST
            printf("awarding last player...\n");
    #endif  
            table = AwardLastPlayer(table);
            handNum++;
            continue;
        }
        //do the flop and show 3 cards
    #ifdef TEST
        printf("entering flop\n");
    #endif
        table = Flop(table);

        //starting from small blind, have the option to fold, check, or raise
        //aka the flop betting round
        assert(table.totActivePlayers > 1);
    #ifdef DEBUG
        printf("getting player next player who has not folded:");
    #endif  
        playerOfInterest = GetNextPlayerWhoHasNotFolded(table, 1, table.dealerNum);
    #ifdef DEBUG
        printf("%d\n", playerOfInterest);
    #endif
    #ifdef DEBUG 
        printf("resetting bets\n");
    #endif
        table = ResetBets(table, 0);
    #ifdef DEBUG
        printf("starting betting round with minimum bet of %d\n", table.currentMinBet);
    #endif
        table = BettingRound(table, playerOfInterest);
        if (table.totActivePlayers == 1) {
    #ifdef TEST
            printf("awarding last player...\n");
    #endif  
            table = AwardLastPlayer(table);
            handNum++;
            continue;
        }
    #ifdef TEST
        printf("showing fourth card\n");
    #endif  
        //show fourth card
        table = Show4or5Card(table, 4);

        //question: what happens if dealer folds --> must update where dealer is?? -->i dont think so

        //start at small blind again (assuming they have not folded)
        assert(table.totActivePlayers > 1);
    #ifdef DEBUG
        printf("Getting next player who has not folded: ");
    #endif  
        playerOfInterest = GetNextPlayerWhoHasNotFolded(table, 1, table.dealerNum);
    #ifdef DEBUG
        printf("%d\n", playerOfInterest);
    #endif  
    #ifdef DEBUG 
        printf("resetting bets\n");
    #endif
        table = ResetBets(table, 0);
    #ifdef DEBUG
        printf("starting betting round with minimum bet of %d\n", table.currentMinBet);
    #endif
        table = BettingRound(table, playerOfInterest);
        if (table.totActivePlayers == 1) {
    #ifdef TEST
            printf("awarding last player...\n");
    #endif  
            table = AwardLastPlayer(table);
            handNum++;
            continue;
        }

        //show fifth card
    #ifdef TEST
        printf("showing fifth card\n");
    #endif  
        table = Show4or5Card(table, 5);

        //start at small blind again (assuming they have not folded)
        assert(table.totActivePlayers > 1);
    #ifdef DEBUG
        printf("Getting next player who has not folded: ");
    #endif  
        playerOfInterest = GetNextPlayerWhoHasNotFolded(table, 1, table.dealerNum);
    #ifdef DEBUG
        printf("%d\n", playerOfInterest);
    #endif  
    #ifdef DEBUG 
        printf("resetting bets\n");
    #endif
        table = ResetBets(table, 0);
    #ifdef DEBUG
        printf("starting betting round with minimum bet of %d\n", table.currentMinBet);
    #endif
        table = BettingRound(table, playerOfInterest);

        /* same steps for awarding winner inbetween bettign rounds */
        //if 1 player is remaining, they win without havnig to show their cards
        if (table.totActivePlayers == 1) {
    #ifdef TEST
            printf("awarding last player...\n");
    #endif  
            table = AwardLastPlayer(table);
            handNum++;
            continue;
        }
        //else
    #ifdef TEST
            printf("\nshowdown\n\n");
    #endif
    printf("entering showdown\n");
        table = Showdown(table);
        printf("exiting showdown\n");
        handNum++;
    } while(PlayersWithMoney(table) > 1);
#ifdef TEST
    printf("determining winning player\n");
#endif  
    winningPlayer = findOnlyPlayerWithMoney(table);
    printf("The winner is player number %d!\n", winningPlayer);
    PrintCurrentBalance(table);
    DeleteTable(table);
    return 0;
}