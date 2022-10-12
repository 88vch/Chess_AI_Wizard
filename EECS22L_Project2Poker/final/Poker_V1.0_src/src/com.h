#ifndef COM_H
#define COM_H
#include "table.h"

// functions that interprets info for each player -->may or may not be used
// string is the string that need to be interpreted
// dataType is the type of info required, like money, token etc.
// used on a local machine to interpret a string of info
char* GetPlayerInfo(char* string, int playerNum, int dataType, char* info);

// send gamestate data to correct player
// function that sends appropriate game state info to a given player
// server -> client
char *SendGameState(Table table, int PlayerNum, char *SendBuf);

// Get info for table
char* GetTableInfo(char* string, int dataType, char* info);

// need a function that finds the nth occurence of a given keyword, 
// then finds the word (not including spaces) after that keyword
char* findWord(const char *text, const char *keyword, int n);

char* findWord_V2(const char *text, const char *keyword, int n, char *output);

void TranslatePlayerCard(int *card, char*string, int playerNum, int pocketCardNum, char* info);

//Sets the table for a specific player playerNum
Table SetTable(Table table, int playerNum, char* string, int endGame);
#endif