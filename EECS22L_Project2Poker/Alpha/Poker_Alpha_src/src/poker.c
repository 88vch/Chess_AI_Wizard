#include "table.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int totalPlayers, startingMoney;
    int smallBlind, bigBlind;
    int playerOfInterest, winningPlayer;
    Table table;
    printf("please enter the total number of players: ");
    scanf("%d", &totalPlayers);
    printf("please enter the starting cash for each player: ");
    scanf("%d", &startingMoney);

    table = CreateTable(table, totalPlayers, startingMoney);    

    //assume that board is being displayed current, assume board is graphically kept up to date

    /*** start of game ***/

    do
    { 
        //cycle board
        table = NewHand(table);

        //shuffle the deck
        table.deck = ShuffleDeck(table.deck);

        //small blind is determined (initialize smallBlind)
        //big blind is determined (ex: minBet = bigBlind * 2)
        table = SetBigAndSmallBlind(table, smallBlind, bigBlind);

        //hand out pocket cards
        table = GivePocketCards(table);

        //betting starts on player after the big blind
        assert(table.totActivePlayers >= 4);
        playerOfInterest = GetNextPlayerWithMoney(table, 3, table.dealerNum);
        //if big blind chooses to NOT raise, then pre-flop betting finishes
        //else big blind player has raised and must cycle through again and finish on small blind's completeion
        table = BettingRound(table, playerOfInterest);

        //do the flop and show 3 cards
        table = Flop(table);

        //starting from small blind, have the option to fold, check, or raise
        //aka the flop betting round
        assert(table.totActivePlayers > 1);
        playerOfInterest = GetNextPlayerWithMoney(table, 1, table.dealerNum);
        table = BettingRound(table, playerOfInterest);

        //show fourth card
        table = Show4or5Card(table, 4);

        //question: what happens if dealer folds --> must update where dealer is?? -->i dont think so

        //start at small blind again (assuming they have not folded)
        assert(table.totActivePlayers > 1);
        playerOfInterest = GetNextPlayerWithMoneyAndNotFolded(table, 1, table.dealerNum);
        table = BettingRound(table, playerOfInterest);

        //show fifth card
        table = Show4or5Card(table, 5);

        /* same steps for awarding winner inbetween bettign rounds */
        //if 1 player is remaining, they win without havnig to show their cards
        if (table.totActivePlayers == 1) {
            table = AwardLastPlayer(table);
        }
        else if (table.totActivePlayers > 1) {
            table = Showdown(table);
        }

    } while(PlayersWithMoney(table) > 1);

    winningPlayer = findOnlyPlayerWithMoney(table);
    printf("The winner is...");

    return 0;
}