#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <assert.h>
#include "com.h"
//#define TEST
//#define STRING_TEST
//#define MEMORY_TEST

//send gamestate data to correct player
//function that sends appropriate game state info to a given player
//server -> client
char *SendGameState(Table table, int PlayerNum, char *SendBuf)
{
    strcpy(SendBuf, "");
    char ConvertBuf[10];
    
    // Gamestate
    // *Not Finished*
    strcat(SendBuf, "GAMESTATE(1) TABLE ");   

    // pot
    strcat(SendBuf, "POT: ");
    sprintf(ConvertBuf, "%d", table.pot);
    strcat(SendBuf, ConvertBuf);
    strcat(SendBuf, " ");

    // mininum bet
    strcat(SendBuf, "MINIMUM_BET: ");
    sprintf(ConvertBuf, "%d", table.currentMinBet);
    strcat(SendBuf, ConvertBuf);
    strcat(SendBuf, " ");

    // dealer number
    strcat(SendBuf, "DEALER_NUM: ");
    sprintf(ConvertBuf, "%d", table.dealerNum);
    strcat(SendBuf, ConvertBuf);
    strcat(SendBuf, " ");

    // total active player
    strcat(SendBuf, "TOTAL_ACTIVE_PLAYERS: ");
    sprintf(ConvertBuf, "%d", table.totActivePlayers);
    strcat(SendBuf, ConvertBuf); 
    strcat(SendBuf, " ");

    // total player
    strcat(SendBuf, "TOTAL_PLAYERS: ");
    sprintf(ConvertBuf, "%d", table.totPlayers);
    strcat(SendBuf, ConvertBuf);    
    strcat(SendBuf, " ");


    // Five sets of table cards
    // e.g. TABLE_CARD_1: TwoClubs encodes to TABLE_CARD_1: 2-3
    // table card 1
    strcat(SendBuf, "TABLE_CARD_1: ");
    
    sprintf(ConvertBuf, "%d", table.tableCards[0].num);
    strcat(SendBuf, ConvertBuf);
    strcat(SendBuf, "-");    
    
    sprintf(ConvertBuf, "%d", table.tableCards[0].suit);
    strcat(SendBuf, ConvertBuf);    
    strcat(SendBuf, " ");    

    // table card 2
    strcat(SendBuf, "TABLE_CARD_2: ");
    
    sprintf(ConvertBuf, "%d", table.tableCards[1].num);
    strcat(SendBuf, ConvertBuf);
    strcat(SendBuf, "-");    
    
    sprintf(ConvertBuf, "%d", table.tableCards[1].suit);
    strcat(SendBuf, ConvertBuf);
    strcat(SendBuf, " ");

    // table card 3
    strcat(SendBuf, "TABLE_CARD_3: ");
    
    sprintf(ConvertBuf, "%d", table.tableCards[2].num);
    strcat(SendBuf, ConvertBuf);
    strcat(SendBuf, "-");    
    
    sprintf(ConvertBuf, "%d", table.tableCards[2].suit);
    strcat(SendBuf, ConvertBuf);
    strcat(SendBuf, " ");

    // table card 4
    strcat(SendBuf, "TABLE_CARD_4: ");
    
    sprintf(ConvertBuf, "%d", table.tableCards[3].num);
    strcat(SendBuf, ConvertBuf);
    strcat(SendBuf, "-");    
    
    sprintf(ConvertBuf, "%d", table.tableCards[3].suit);
    strcat(SendBuf, ConvertBuf);
    strcat(SendBuf, " ");

    // table card 5
    strcat(SendBuf, "TABLE_CARD_5: ");
    
    sprintf(ConvertBuf, "%d", table.tableCards[4].num);
    strcat(SendBuf, ConvertBuf);
    strcat(SendBuf, "-");    
    
    sprintf(ConvertBuf, "%d", table.tableCards[4].suit);
    strcat(SendBuf, ConvertBuf);
    strcat(SendBuf, " ");


    // info for all players
    // based on Player Num
    for (int i = 0; i < table.totPlayers; i++)
    {
        // PLAYER_NUM
        strcat(SendBuf, "PLAYER_NUM: ");
        sprintf(ConvertBuf, "%d", i);
        strcat(SendBuf, ConvertBuf);
        strcat(SendBuf, " "); 

        // USERNAME
        // *Need adjust* username length not finialized
        strcat(SendBuf, "USERNAME: ");
        strcat(SendBuf, table.players[i].username);        
        strcat(SendBuf, " "); 

        // POCKET_1
        strcat(SendBuf, "POCKET_1: ");

        // 1. first set
        sprintf(ConvertBuf, "%d", table.players[i].pocket[0].num);
        strcat(SendBuf, ConvertBuf);
        strcat(SendBuf, "-"); 
        
        sprintf(ConvertBuf, "%d", table.players[i].pocket[0].suit);
        strcat(SendBuf, ConvertBuf);
        strcat(SendBuf, " "); 

        // POCKET_2
        strcat(SendBuf, "POCKET_2: ");

        // 2. second set
        sprintf(ConvertBuf, "%d", table.players[i].pocket[1].num);
        strcat(SendBuf, ConvertBuf);
        strcat(SendBuf, "-"); 
        
        sprintf(ConvertBuf, "%d", table.players[i].pocket[1].suit);
        strcat(SendBuf, ConvertBuf);
        strcat(SendBuf, " "); 

        // MONEY
        strcat(SendBuf, "MONEY: ");
        sprintf(ConvertBuf, "%d", table.players[i].money);
        strcat(SendBuf, ConvertBuf);
        strcat(SendBuf, " "); 

        // BET_AMOUNT
        strcat(SendBuf, "BET_AMOUNT: ");
        sprintf(ConvertBuf, "%d", table.players[i].currentBetAmount);
        strcat(SendBuf, ConvertBuf);
        strcat(SendBuf, " "); 

        // MOVE_CHOICE
        // *Not Finished*
        strcat(SendBuf, "MOVE_CHOICE: ");
        sprintf(ConvertBuf, "%d", table.players[i].moveChoice);
        strcat(SendBuf, ConvertBuf);
        strcat(SendBuf, " "); 

        // TOKEN
        strcat(SendBuf, "TOKEN: ");
        sprintf(ConvertBuf, "%d", table.players[i].token);
        strcat(SendBuf, ConvertBuf);
        strcat(SendBuf, " "); 
    }

    return SendBuf;
}
/*
char *serialize(Table table) {
    int str_len = strlen(table.str);

    int size = 2 * sizeof(int) + str_len;
    char *buffer = malloc(sizeof(char) * (size+1));

    strcpy(buffer, &table.a, sizeof(int));
    strcpy(buffer + sizeof(int), &str_len, sizeof(int));
    strcpy(buffer + sizeof(int) * 2, table.str, str_len);
    buffer[size] = '\0';

    return buffer;
}

// table.players[PlayerNum].token               button
// table.players[PlayerNum].money               int
// table.players[PlayerNum].pocket              card
// table.players[PlayerNum].hasFolded           hasfolded
// table.players[PlayerNum].currentBetAmount    int
// table.players[PlayerNum].username            char
// table.players[PlayerNum].position            int

*/



/* a function that finds the nth occurence after a given keyword, then finds the word (not including spaces) after that keyword */
char* findWord(const char *text, const char *keyword, int n) {
  //assume that everyword we are looking for has a space after it
    const char *finder;
    char *data;
    int lengthOfKeyword, lengthOfData;
    finder = text;
    if (strstr(text,keyword) == text) {
        finder++;
        n--;
    }
    for (int i = 0; i < n; i++) { 
        finder++;
        finder = strstr(finder, keyword);
    }
    //now finder points to beginning of keyword
    lengthOfKeyword = strlen(keyword);
    #ifdef STRING_TEST
    printf("value of n: %d", n);
    printf("\ncharacter in question:\n%s", (finder + lengthOfKeyword));
    #endif
    assert(*(finder + lengthOfKeyword) == ' ');
    for (int i = 0; i < lengthOfKeyword + 1; i++) { finder++; }
    //now pointer to first letter of desired info
    //need to determine length of desired info
    lengthOfData = 0;
    #ifdef MEMORY_TEST
    printf("\nentering do while loop\n");
    #endif
    do {
        lengthOfData++;
    } while(*(finder + lengthOfData) != ' ');
    #ifdef MEMORY_TEST
    printf("exited do while loop. malloc data\n");
    printf("value of finder: \n%s\n", finder);
    printf("lengthOfData: %d\n", lengthOfData);
    #endif
    data = (char*)malloc(sizeof(char) * (lengthOfData + 1));
    #ifdef MEMORY_TEST
    printf("finished malloc. entering last for loop\n");
    #endif
    for (int i = 0; i < lengthOfData; i++) {
        data[i] = *(finder + i);
    }
    data[lengthOfData] = '\0';
    #ifdef MEMORY_TEST
    printf("exited last for loop. findWord function runs!\n");
    #endif
    return data;
}

//need a function that finds the nth occurence of a given keyword, then finds the word (not including spaces) after that keyword
char* findWord_V2(const char *text, const char *keyword, int n, char *output) {
  //assume that everyword we are looking for has a space after it
    const char *finder;
    strcpy(output, "");
    int lengthOfKeyword, lengthOfData;
    finder = text;
    if (strstr(text,keyword) == text) {
        if (n != 1) { finder++; }
        n--;
    }
    for (int i = 0; i < n; i++) { 
        finder++;
        finder = strstr(finder, keyword); 
    }
    //now finder points to beginning of keyword
    lengthOfKeyword = strlen(keyword);
    assert(*(finder + lengthOfKeyword) == ' ');
    for (int i = 0; i < lengthOfKeyword + 1; i++) { finder++; }
    //now pointer to first letter of desired info
    //need to determine length of desired info
    lengthOfData = 0;
    do {
        lengthOfData++;
    } while(*(finder + lengthOfData) != ' ');
    //data = malloc(sizeof(char) * (lengthOfData + 1));
    strncpy(output, finder, lengthOfData);
    output[lengthOfData] = '\0';
    return output;
}

/* finds, interprets, and returns the player value requested from string */
char* GetPlayerInfo(char* string, int playerNum, int dataType, char* info) {
    #ifdef TEST
    printf("in function GetPlayerInfo\n");
    #endif
    char keyword[32];
    switch (dataType) {
    /* PLAYER_NUM */
    case 1:
        strcpy(keyword, "PLAYER_NUM:");
        break;
    /* USERNAME */
    case 2:
        strcpy(keyword, "USERNAME:");
        break;
    /* POCKET_1 */
    case 3:
        strcpy(keyword, "POCKET_1:");
        break;
    /* POCKET_2 */
    case 4:
        strcpy(keyword, "POCKET_2:");
        break;
    /* MONEY */
    case 5:
        strcpy(keyword, "MONEY:");
        break;
    /* BET_AMOUNT */
    case 6:
        strcpy(keyword, "BET_AMOUNT:");
        break;
    /* MOVE_CHOICE */
    case 7:
        strcpy(keyword, "MOVE_CHOICE:");
        break;
    /* TOKEN */
    case 8:
        strcpy(keyword, "TOKEN:");
        break;
    default:
        break;
    }
    #ifdef TEST
    printf("\nkeyword in question: %s\n", keyword);
    #endif
    findWord_V2(string, keyword, playerNum + 1, info);
    return info;
}

/* finds, interprets, and returns the table value requested from string */
char* GetTableInfo(char* string, int dataType, char* info) {
    char keyword[32];
    #ifdef TEST
    printf("in function GetTableInfo\n");
    #endif
    switch (dataType) {
    /* POT */
    case 1:
        strcpy(keyword, "POT:");
        break;
    /* TABLE_CARD_1 */
    case 2:
        strcpy(keyword, "TABLE_CARD_1:");
        break;
    /* TABLE_CARD_2 */
    case 3:
        strcpy(keyword, "TABLE_CARD_2:");
        break;
    /* TABLE_CARD_3 */
    case 4:
        strcpy(keyword, "TABLE_CARD_3:");
        break;
    /* TABLE_CARD_4 */
    case 5:
        strcpy(keyword, "TABLE_CARD_4:");
        break;
    /* TABLE_CARD_5 */
    case 6:
        strcpy(keyword, "TABLE_CARD_5:");
        break;
    /* MINIMUM_BET */
    case 7:
        strcpy(keyword, "MINIMUM_BET:");
        break;
    /* DEALER_NUM */
    case 8:
        strcpy(keyword, "DEALER_NUM:");
        break;
    /* TOTAL_ACTIVE_PLAYERS */
    case 9:
        strcpy(keyword, "TOTAL_ACTIVE_PLAYERS:");
        break;
    /* TOTAL_PLAYERS */
    case 10:
        strcpy(keyword, "TOTAL_PLAYERS:");
        break;
    default:
        break;
    }
    #ifdef TEST
    printf("\nkeyword in question: %s\n", keyword);
    #endif
    findWord_V2(string, keyword, 1, info);
    return info;
}


/* translates a card from the string into integers stored in the input pointer to card */
void TranslateCard(int *card, char*string, int playerNum, int cardNum, char* info) {
    /* 
    GetPlayerInfo 
        case 3 = Pocket_1; 
        case 4 = Pocket_2; 
    therefore pocketCardNum + 3

    playerNum = -1 if community cards updating

    :return array[2] where array[0] == card number and array[1] == card suit
    */
    int dataType;

    char *token;
    char card_values[32];
    int j, card_num, card_suit;

    if (playerNum != -1) {
        dataType = cardNum + 3;
        strcpy(card_values, GetPlayerInfo(string, playerNum, dataType, info));
        token = strtok(card_values, "-");
        j = 0;
        while (token != NULL) {
            if (j == 0) {
                card_num = atoi(token);
            }
            else if (j == 1) {
                card_suit = atoi(token);
            }
            token = strtok(NULL, "-");
            j++;
        }
        card[0] = card_num;
        card[1] = card_suit;
    }
    else {
        dataType = cardNum + 2;
        strcpy(card_values, GetTableInfo(string, dataType, info));
        token = strtok(card_values, "-");
        j = 0;
        while (token != NULL) {
            if (j == 0) {
                card_num = atoi(token);
            }
            else if (j == 1) {
                card_suit = atoi(token);
            }
            token = strtok(NULL, "-");
            j++;
        }
        card[0] = card_num;
        card[1] = card_suit; 
    }
}

/* 
Sets the table for a specific player playerNum (table should be playerNum's visible version of table)
    Note: int atoi(const char *string) 
*/
Table SetTable(Table table, int playerNum, char* string, int endGame) {
    char info[50];
    /* table.players */
    #ifdef TEST
    printf("in function SetTable\n");
    #endif
    for (int i = 0; i < table.totActivePlayers; i++) {
        #ifdef TEST
        printf("\n\n\n\nplayer %d;\n", i);
        #endif
        table.players[i].token = atoi(GetPlayerInfo(string, i, 8, info));
        #ifdef TEST
        printf("\nupdated token\n");
        #endif
        table.players[i].money = atoi(GetPlayerInfo(string, i, 5, info));
        #ifdef TEST
        printf("\nupdated money\n");
        #endif
        table.players[i].currentBetAmount = atoi(GetPlayerInfo(string, i, 6, info));
        #ifdef TEST
        printf("\nupdated currentBetAmount\n");
        #endif
        table.players[i].position = i;
        table.players[i].moveChoice = atoi(GetPlayerInfo(string, i, 7, info));
        #ifdef TEST
        printf("\nupdated moveChoice\n");
        #endif
        
        strcpy(table.players[i].username, GetPlayerInfo(string, i, 2, info));
        #ifdef TEST
        printf("\nupdated username\n");
        #endif
        /* Note: in showdown we can display all other players cards */
        for (int j = 0; j < 2; j++) {
                int card_value[2];
                #ifdef TEST
                printf("string going into TranslateCard function\n%s\n", string);
                #endif
                TranslateCard(card_value, string, i, j, info);
                table.players[i].pocket[j].num = card_value[0];
                table.players[i].pocket[j].suit = card_value[1];
                #ifdef TEST
                printf("\nupdated values of playerNum pocket card %d;\nnum = %d\nsuit = %d\n\n", j + 1, card_value[0], card_value[1]);
                #endif
        } 
        if (endGame == 0) {
            if (i != playerNum) {
                table.players[i].pocket[0].num = 15;
                table.players[i].pocket[0].suit = 4;
                table.players[i].pocket[1].num = 15;
                table.players[i].pocket[1].suit = 4;
                #ifdef TEST
                printf("not in showdown. hiding cards of player %d\n", i);
                printf("\nupdated values of playerNum pocket card 1;\nnum = %d\nsuit = %d\n\n", table.players[i].pocket[0].num, table.players[i].pocket[0].suit);
                printf("\nupdated values of playerNum pocket card 2;\nnum = %d\nsuit = %d\n\n", table.players[i].pocket[1].num, table.players[i].pocket[1].suit);
                #endif
            }
        }
    }
    /* Note: we don't care about table.deck here */
    #ifdef TEST
    printf("\n\n\nstarting to update table info\n");
    #endif
    table.totPlayers = atoi(GetTableInfo(string, 10, info));
    #ifdef TEST
    printf("\nupdated totPlayers\n");
    #endif
    table.totActivePlayers = atoi(GetTableInfo(string, 9, info));
    #ifdef TEST
    printf("\nupdated totActivePlayers\n");
    #endif
    table.pot = atoi(GetTableInfo(string, 1, info));
    #ifdef TEST
    printf("\nupdated pot\n");
    #endif
    table.dealerNum = atoi(GetTableInfo(string, 8, info));
    #ifdef TEST
    printf("\nupdated dealerNum\n");
    #endif
    table.currentMinBet = atoi(GetTableInfo(string, 7, info));
    #ifdef TEST
    printf("\nupdated currentMinBet\n");
    #endif
    /* table.tableCards */
    for (int i = 0; i < 5; i++) {
        #ifdef TEST
        printf("\nupdating card %d\n", i + 1);
        #endif
        int card_value[2];
        TranslateCard(card_value, string, -1, i, info);
        table.tableCards[i].num = card_value[0];
        table.tableCards[i].suit = card_value[1];
        #ifdef TEST
        printf("\nupdated values of playerNum pocket card 0;\nnum = %d\nsuit = %d\n\n", card_value[0], card_value[1]);
        #endif 
    }
    return table;
}

#ifdef TEST
/* create table, initialize table, test function */
int main(void) {
    Table table, playerNum_table;
    int numPlayers = 6, startingMoney = 1000;
    int smallBlind = 4, bigBlind = 8;
    playerNum_table = CreateTable(numPlayers, startingMoney);
    table = CreateTable(numPlayers, startingMoney);
    table.deck = ShuffleDeck(table.deck);
    table = SetBigAndSmallBlind(table, smallBlind, bigBlind);
    table = GivePocketCards(table);
    table = Flop(table);

    int playerNum;
    printf("enter playerNum:");
    scanf("%d", &playerNum);
    //char info[10000];
    int i = 0, j = 0;
    while (1) {
        char buffer[10000];
        SendGameState(table, playerNum, buffer);
        playerNum_table = SetTable(playerNum_table, playerNum, buffer, j);
        printf("original table values\n");
        PrintCurrentBalance(table);
        printf("playerNum table values\n");
        PrintCurrentBalance(playerNum_table);

        switch (i) {
            case 0:
                printf("successfully updated player number %d's table, printing available info...\n", playerNum);
                PrintCurrentBalance(playerNum_table);
                table = Show4or5Card(table, 4);
                table = Show4or5Card(table, 5);
                printf("updated 4th and 5th community card\n");
                break;
            case 1:
                printf("successfully updated player number %d's table, printing available info...\n", playerNum);
                PrintCurrentBalance(playerNum_table);
                table = Showdown(table); 
                printf("finished showdown\n");
                j = 1;
                break;
            case 2:
                SendGameState(table, playerNum, buffer);
                playerNum_table = SetTable(playerNum_table, playerNum, buffer, 1);
                printf("successfully updated player number %d's table, printing available showdown info...\n", playerNum);
                PrintCurrentBalance(playerNum_table);
                DeleteTable(playerNum_table);
                DeleteTable(table);
                printf("\n\n\nsuccess! everything has finished updating!\n");
                break;
        }
        if (i == 2) {break;}
        i++;
    }
    return 0;
}
#endif
