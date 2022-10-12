/* GTK_ClockServer.c: GUI example TCP/IP server with timeout support
 * Author: Rainer Doemer, 2/23/17 (based on simple ClockServer.c)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <gtk/gtk.h>
#include <assert.h>
#include <math.h>

#include "card.h"
#include "deck.h"
#include "player.h"
//#include "table.h"

/* #define DEBUG */	/* be verbose */
#define BUFFSIZE 256
#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

/*** type definitions ****************************************************/

typedef void (*ClientHandler)(int DataSocketFD);
typedef void (*TimeoutHandler)(void);

/*** global variables ****************************************************/

const char *Program	/* program name for descriptive diagnostics */
	= "Goldfish Server";
int Shutdown		/* keep running until Shutdown == 1 */
	= 0;
char ClockBuffer[26]	/* current time in printable format */
	= "";
GtkLabel *DigitalClock	/* label widget displaying digital time */
	= NULL;
GtkWidget *AnalogClock	/* drawing widget displaying analog clock */
	= NULL;
//Labels

GtkLabel *title; 


//Windowsize
#define WindowBorder (10)
#define WindowWidth (1500 + 2 * (WindowBorder))
#define WindowHeight (1000 + 2 * (WindowBorder))
/*****************************Testing Purposes******************************/
//Keep this
Player PlayerOne;
Player PlayerTwo;
Player PlayerThree;
Player PlayerFour;
Player PlayerFive;
Player PlayerSix;
char UsernameText[] = "Username: ";
char PointsBuffer[20];
char RankBuffer[20];
//MB delete this
char testusername[] = "bobross";
int PointsTest = 500;

/*************************End of Testing Purposes***************************/


/***********************GTK_Labels Global for Updating********************/

//Username
GtkLabel *PlayerOneUser = NULL;
GtkLabel *PlayerTwoUser = NULL;
GtkLabel *PlayerThreeUser = NULL;
GtkLabel *PlayerFourUser = NULL;
GtkLabel *PlayerFiveUser = NULL;
GtkLabel *PlayerSixUser = NULL;
//Points
GtkLabel *PlayerOnePoints = NULL;
GtkLabel *PlayerTwoPoints = NULL;
GtkLabel *PlayerThreePoints = NULL;
GtkLabel *PlayerFourPoints = NULL;
GtkLabel *PlayerFivePoints = NULL;
GtkLabel *PlayerSixPoints = NULL;
//Action
GtkLabel *PlayerOneAction = NULL;
GtkLabel *PlayerTwoAction = NULL;
GtkLabel *PlayerThreeAction = NULL;
GtkLabel *PlayerFourAction = NULL;
GtkLabel *PlayerFiveAction = NULL;
GtkLabel *PlayerSixAction = NULL;
//Status
GtkLabel *PlayerOneStatus = NULL;
GtkLabel *PlayerTwoStatus = NULL;
GtkLabel *PlayerThreeStatus = NULL;
GtkLabel *PlayerFourStatus = NULL;
GtkLabel *PlayerFiveStatus = NULL;
GtkLabel *PlayerSixStatus = NULL;
//Rank Dealer or Player
GtkLabel *PlayerOneRank = NULL;
GtkLabel *PlayerTwoRank = NULL;
GtkLabel *PlayerThreeRank = NULL;
GtkLabel *PlayerFourRank = NULL;
GtkLabel *PlayerFiveRank = NULL;
GtkLabel *PlayerSixRank = NULL;

//Image for Avatar
GtkWidget *avatarOne;
GtkWidget *avatarTwo;
GtkWidget *avatarThree;
GtkWidget *avatarFour;
GtkWidget *avatarFive;
GtkWidget *avatarSix;
//Card image
char CardOneTopSub[40];
char CardOneBotSub[40];
char CardTwoTopSub[40];
char CardTwoBotSub[40];

GtkWidget *cardbox;

GtkWidget *PlayerOneCardoneTop;
GtkWidget *PlayerOneCardoneBot;
GtkWidget *PlayerOneCardtwoTop;
GtkWidget *PlayerOneCardtwoBot;

GtkWidget *PlayerTwoCardoneTop;
GtkWidget *PlayerTwoCardoneBot;
GtkWidget *PlayerTwoCardtwoTop;
GtkWidget *PlayerTwoCardtwoBot;

GtkWidget *PlayerThreeCardoneTop;
GtkWidget *PlayerThreeCardoneBot;
GtkWidget *PlayerThreeCardtwoTop;
GtkWidget *PlayerThreeCardtwoBot;

GtkWidget *PlayerFourCardoneTop;
GtkWidget *PlayerFourCardoneBot;
GtkWidget *PlayerFourCardtwoTop;
GtkWidget *PlayerFourCardtwoBot;

GtkWidget *PlayerFiveCardoneTop;
GtkWidget *PlayerFiveCardoneBot;
GtkWidget *PlayerFiveCardtwoTop;
GtkWidget *PlayerFiveCardtwoBot;

GtkWidget *PlayerSixCardoneTop;
GtkWidget *PlayerSixCardoneBot;
GtkWidget *PlayerSixCardtwoTop;
GtkWidget *PlayerSixCardtwoBot;




//Community Cards
Card CommunityCard[5];
char CCOneTop[40];
char CCOneBot[40];
char CCTwoTop[40];
char CCTwoBot[40];
char CCThreeTop[40];
char CCThreeBot[40];
char CCFourTop[40];
char CCFourBot[40];
char CCFiveTop[40];
char CCFiveBot[40];

GtkWidget *CCOneTopImage;
GtkWidget *CCOneBotImage;
GtkWidget *CCTwoTopImage;
GtkWidget *CCTwoBotImage;
GtkWidget *CCThreeTopImage;
GtkWidget *CCThreeBotImage;
GtkWidget *CCFourTopImage;
GtkWidget *CCFourBotImage;
GtkWidget *CCFiveTopImage;
GtkWidget *CCFiveBotImage;

//Pot Size 
GtkLabel *PotTotalSizeLabel;
char PotBuffer[20];


/*** GUI functions *******************************************************/



//Might not need this
void ShutdownClicked(		/* shutdown button was clicked */
	GtkWidget *Widget,
	gpointer Data)
{
#ifdef DEBUG
    printf("%s: ShutdownClicked callback starting...\n", Program);
#endif
    Shutdown = 1;	/* initiate server shutdown */
    DigitalClock = NULL;/* turn clocks off */
    AnalogClock = NULL;
#ifdef DEBUG
    printf("%s: ShutdownClicked callback done.\n", Program);
#endif
} /* end of ShutdownClicked */





//Creates the Actual window
GtkWidget *CreateWindow(	/* create the server window */
	int *argc,
	char **argv[])
{
    GtkWidget *Window;
    //GtkWidget *ShutdownButton;
    GtkWidget *table;
    //Makign Titles
    int n = 0;
    int m;
    int space = 3;
    

    /* initialize the GTK libraries */
    gtk_init(argc, argv);

    /* create the main, top level window */
    Window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(Window, WindowWidth, WindowHeight);
    gtk_container_set_border_width (GTK_CONTAINER(Window), WindowBorder);
    gtk_window_set_title(GTK_WINDOW(Window), Program);

    

    //gtk_window_set_default_size(GTK_WINDOW(Window), WindowWidth, WindowHeight);
    //gtk_window_set_resizable(GTK_WINDOW(Window), FALSE) ; 

    //Create table
    table = gtk_table_new(WindowWidth, WindowHeight, TRUE);
   

    //**********************************Creates the Titles************************************
    //Player 0-3
    title = (GtkLabel *) gtk_label_new("Player");
    gtk_widget_set_size_request((GtkWidget *) title, 50 * space,50);
    m = n + space; 
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) title, n, m, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
    //Avatar 3 - 6
    title = (GtkLabel *) gtk_label_new("Avatar");
    gtk_widget_set_size_request((GtkWidget *) title, 50 * space,50);
    n = m;
    m = n + space;
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) title, n, m, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
    //Hand 6 - 12
    title = (GtkLabel *) gtk_label_new("Hand");
    gtk_widget_set_size_request((GtkWidget *) title, 50 * space * 2,50);
    n = m;
    m = n+ 2 * space;
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) title, n, m, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
    //Points 12 - 15
    title = (GtkLabel *) gtk_label_new("Points");
    gtk_widget_set_size_request((GtkWidget *) title, 50 * space,50);
    n = m;
    m = n+space;
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) title, n, m, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
    //Rank 15 - 18
    title = (GtkLabel *) gtk_label_new("Rank");
    gtk_widget_set_size_request((GtkWidget *) title, 50 * space,50);
    n = m;
    m = n+space;
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) title, n, m, 0, 1, GTK_FILL, GTK_FILL, 0, 0);

    /*
    //Action 18 - 21
    title = (GtkLabel *) gtk_label_new("Action");
    gtk_widget_set_size_request((GtkWidget *) title, 50 * space,50);
    n = m;
    m = n+space;
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) title, n, m, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
    */

    //Status 21 - 24
    title = (GtkLabel *) gtk_label_new("Status");
    gtk_widget_set_size_request((GtkWidget *) title, 50 * space,50);
    n = m;
    m = n + space;
    printf("Last edge: %d\n", m);
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) title, n, m, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
    //Community Cards
    title = (GtkLabel *) gtk_label_new(" Commmunity Cards ");
    //gtk_widget_set_size_request((GtkWidget *) title, 50,50);
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) title, 1, 10, 13, 14, GTK_FILL, GTK_FILL, 0, 0);
    //Pot
    title = (GtkLabel *) gtk_label_new(" Pot Size ");
    //gtk_widget_set_size_request((GtkWidget *) title, 50,50);
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) title, 18, 20, 13, 14, GTK_FILL, GTK_FILL, 0, 0);

    //********************************************End of creating titles*********************************
    

    //****************************************Creating Username Labels**********************************
    char subUser[50];
    //strcat(dest, src) Add two strings into one
    
    //subUser = UsernameText;
    //PlayerOne
    if(PlayerOne.username [0] != '\0'){
    strcpy(subUser, UsernameText);
    strcat(subUser, PlayerOne.username);
    }else{
        strcpy(subUser, "Empty/ Vacant");
    }
    PlayerOneUser = (GtkLabel *) gtk_label_new(subUser);
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerOneUser, 0, 3, 1, 3, GTK_FILL, GTK_FILL, 0, 0);
    //PlayerTwo
    if(PlayerTwo.username[0] != '\0'){
    strcpy(subUser, UsernameText);
    strcat(subUser, PlayerTwo.username);
    }else{
        strcpy(subUser, "Empty/ Vacant");
    }
    PlayerTwoUser = (GtkLabel *) gtk_label_new(subUser);
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerTwoUser, 0, 3, 3, 5, GTK_FILL, GTK_FILL, 0, 0);
    //PlayerThree
    if(PlayerThree.username[0] != '\0'){
    strcpy(subUser, UsernameText);
    strcat(subUser, PlayerThree.username);
    }else{
        strcpy(subUser, "Empty/ Vacant");
    }
    PlayerThreeUser = (GtkLabel *) gtk_label_new(subUser);
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerThreeUser, 0, 3, 5, 7, GTK_FILL, GTK_FILL, 0, 0);
    //Player Four
    if(PlayerFour.username[0] != '\0'){
    strcpy(subUser, UsernameText);
    strcat(subUser, PlayerFour.username);
    }else{
        strcpy(subUser, "Empty/ Vacant");
    }
    PlayerFourUser = (GtkLabel *) gtk_label_new(subUser);
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerFourUser, 0, 3, 7,9, GTK_FILL, GTK_FILL, 0, 0);
    //Player Five
    if(PlayerFive.username[0] != '\0'){
    strcpy(subUser, UsernameText);
    strcat(subUser, PlayerFive.username);
    }else{
        strcpy(subUser, "Empty/ Vacant");
    }

    PlayerFiveUser = (GtkLabel *) gtk_label_new(subUser);
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerFiveUser, 0, 3, 9, 11, GTK_FILL, GTK_FILL, 0, 0);
    //Player Six
    if(PlayerSix.username[0] != '\0'){
      strcpy(subUser, UsernameText);
      strcat(subUser, PlayerSix.username);
    }else{
      strcpy(subUser, "Empty/ Vacant");
    }
    PlayerSixUser = (GtkLabel *) gtk_label_new(subUser);
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerSixUser, 0, 3, 11, 13, GTK_FILL, GTK_FILL, 0, 0);
    //*******************************************End of Creating Username Labels *************************

    //**********************************Creating Points Label**********************************
    //Player 1
    if(PlayerOne.money == -1){
        PlayerOnePoints = (GtkLabel *) gtk_label_new("0");
    }else{
        sprintf(PointsBuffer,"%d", PlayerOne.money);
        PlayerOnePoints = (GtkLabel *) gtk_label_new(PointsBuffer);
    }
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerOnePoints, 12, 15, 1, 3, GTK_FILL, GTK_FILL, 0, 0);

    //Player2
    if(PlayerTwo.money == -1){
        PlayerTwoPoints = (GtkLabel *) gtk_label_new("0");
    }else{
        sprintf(PointsBuffer,"%d", PlayerTwo.money);
        PlayerTwoPoints = (GtkLabel *) gtk_label_new(PointsBuffer);
    }
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerTwoPoints, 12, 15, 3, 5, GTK_FILL, GTK_FILL, 0, 0);

    //Player 3
    if(PlayerThree.money == -1){
        PlayerThreePoints = (GtkLabel *) gtk_label_new("0");
    }else{
        sprintf(PointsBuffer,"%d", PlayerThree.money);
        PlayerThreePoints = (GtkLabel *) gtk_label_new(PointsBuffer);
    }
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerThreePoints, 12, 15, 5, 7, GTK_FILL, GTK_FILL, 0, 0);
    //Player 4
    if(PlayerFour.money == -1){
        PlayerFourPoints = (GtkLabel *) gtk_label_new("0");
    }else{
        sprintf(PointsBuffer,"%d", PlayerFour.money);
        PlayerFourPoints = (GtkLabel *) gtk_label_new(PointsBuffer);
    }
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerFourPoints, 12, 15, 7, 9, GTK_FILL, GTK_FILL, 0, 0);
    //Player 5
    if(PlayerFive.money == -1){
        PlayerFivePoints = (GtkLabel *) gtk_label_new("0");
    }else{
        sprintf(PointsBuffer,"%d", PlayerFive.money);
        PlayerFivePoints = (GtkLabel *) gtk_label_new(PointsBuffer);
    }
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerFivePoints, 12, 15, 9, 11, GTK_FILL, GTK_FILL, 0, 0);
    //Player 6
    if(PlayerSix.money == -1){
        PlayerSixPoints = (GtkLabel *) gtk_label_new("0");
    }else{
        sprintf(PointsBuffer,"%c", PlayerSix.money);
        PlayerSixPoints = (GtkLabel *) gtk_label_new(PointsBuffer);
    }
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerSixPoints, 12, 15, 11, 13, GTK_FILL, GTK_FILL, 0, 0);

    //***************************************** End of Creating Points Label ***********************************

    //****************************************Creating Rank Label ******************************************
    //Player One
    switch(PlayerOne.token)
        {
            case 0:
                PlayerOneRank = (GtkLabel *) gtk_label_new("Dealer");
                break;
            case 1:
                PlayerOneRank = (GtkLabel *) gtk_label_new("Small Blind");
                break;
            case 2:
                PlayerOneRank = (GtkLabel *) gtk_label_new("Big Blind");
                break;
            case 3:
                PlayerOneRank = (GtkLabel *) gtk_label_new("Normal");
            default:
                PlayerOneRank = (GtkLabel *) gtk_label_new("Player does not exist");
                break;
        }
     gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerOneRank, 15, 18, 1, 3, GTK_FILL, GTK_FILL, 0, 0);
    //PlayerTwo
    switch(PlayerTwo.token)
        {
            case 0:
                PlayerTwoRank = (GtkLabel *) gtk_label_new("Dealer");
                break;
            case 1:
                PlayerTwoRank = (GtkLabel *) gtk_label_new("Small Blind");
                break;
            case 2:
                PlayerTwoRank = (GtkLabel *) gtk_label_new("Big Blind");
                break;
            case 3:
                PlayerTwoRank = (GtkLabel *) gtk_label_new("Normal");
            default:
                PlayerTwoRank = (GtkLabel *) gtk_label_new("Player does not exist");
                break;
        }
     gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerTwoRank, 15, 18, 3, 5, GTK_FILL, GTK_FILL, 0, 0);
    //PLayerThree
    switch(PlayerThree.token)
        {
            case 0:
                PlayerThreeRank = (GtkLabel *) gtk_label_new("Dealer");
                break;
            case 1:
                PlayerThreeRank = (GtkLabel *) gtk_label_new("Small Blind");
                break;
            case 2:
                PlayerThreeRank = (GtkLabel *) gtk_label_new("Big Blind");
                break;
            case 3:
                PlayerThreeRank = (GtkLabel *) gtk_label_new("Normal");
            default:
                PlayerThreeRank = (GtkLabel *) gtk_label_new("Player does not exist");
                break;
        }
     gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerThreeRank, 15, 18, 5, 7, GTK_FILL, GTK_FILL, 0, 0);
     //PlayerFour
    switch(PlayerFour.token)
        {
            case 0:
                PlayerFourRank = (GtkLabel *) gtk_label_new("Dealer");
                break;
            case 1:
                PlayerFourRank = (GtkLabel *) gtk_label_new("Small Blind");
                break;
            case 2:
                PlayerFourRank = (GtkLabel *) gtk_label_new("Big Blind");
                break;
            case 3:
                PlayerFourRank = (GtkLabel *) gtk_label_new("Normal");
            default:
                PlayerFourRank = (GtkLabel *) gtk_label_new("Player does not exist");
                break;
        }
     gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerFourRank, 15, 18, 7, 9, GTK_FILL, GTK_FILL, 0, 0);

     //Player Five
     switch(PlayerFive.token)
        {
            case 0:
                PlayerFiveRank = (GtkLabel *) gtk_label_new("Dealer");
                break;
            case 1:
                PlayerFiveRank = (GtkLabel *) gtk_label_new("Small Blind");
                break;
            case 2:
                PlayerFiveRank = (GtkLabel *) gtk_label_new("Big Blind");
                break;
            case 3:
                PlayerFiveRank = (GtkLabel *) gtk_label_new("Normal");
            default:
                PlayerFiveRank = (GtkLabel *) gtk_label_new("Player does not exist");
                break;
        }
     gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerFiveRank, 15, 18, 9, 11, GTK_FILL, GTK_FILL, 0, 0);
    //Player Six
    switch(PlayerSix.token)
        {
            case 0:
                PlayerSixRank = (GtkLabel *) gtk_label_new("Dealer");
                break;
            case 1:
                PlayerSixRank = (GtkLabel *) gtk_label_new("Small Blind");
                break;
            case 2:
                PlayerSixRank = (GtkLabel *) gtk_label_new("Big Blind");
                break;
            case 3:
                PlayerSixRank = (GtkLabel *) gtk_label_new("Normal");
            default:
                PlayerSixRank = (GtkLabel *) gtk_label_new("Player does not exist");
                break;
        }
     gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerSixRank, 15, 18, 11, 13, GTK_FILL, GTK_FILL, 0, 0);

//*******************************Avatar/Character*****************************
    
    //Do this later
    char subimageline[]= "./goldfishavatar/goldfishbloop.png";
    //Player One
    if(PlayerOne.username[0] != '\0'){
        avatarOne = gtk_image_new_from_file(subimageline);
    }else{
        avatarOne = gtk_image_new_from_file("./goldfishavatar/empty.png");
    }

    gtk_widget_set_size_request((GtkWidget *)avatarOne, 50 * 3, 100);
    gtk_table_attach(GTK_TABLE(table), avatarOne, 3, 6, 1, 3, GTK_FILL, GTK_FILL, 0, 0);

    //Player Two
    if(PlayerTwo.username[0] != '\0'){
        avatarTwo = gtk_image_new_from_file("./goldfishavatar/goldfishbloop.png");
    }else{
        avatarTwo = gtk_image_new_from_file("./goldfishavatar/empty.png");
    }
    gtk_widget_set_size_request((GtkWidget *)avatarTwo, 50 * 3, 100);
    gtk_table_attach(GTK_TABLE(table), avatarTwo, 3, 6, 3, 5, GTK_FILL, GTK_FILL, 0, 0);

    //Player Three
    if(PlayerThree.username[0] != '\0'){
        avatarThree = gtk_image_new_from_file("./goldfishavatar/goldfishbloop.png");
    }else{
        avatarThree = gtk_image_new_from_file("./goldfishavatar/empty.png");
    }
    gtk_widget_set_size_request((GtkWidget *)avatarThree, 50 * 3, 100);
    gtk_table_attach(GTK_TABLE(table), avatarThree, 3, 6, 5, 7, GTK_FILL, GTK_FILL, 0, 0);

    //Player Four
    if(PlayerFour.username[0] != '\0'){
        avatarFour = gtk_image_new_from_file("./goldfishavatar/goldfishbloop.png");
    }else{
        avatarFour = gtk_image_new_from_file("./goldfishavatar/empty.png");
    }
    gtk_widget_set_size_request((GtkWidget *)avatarFour, 50 * 3, 100);
    gtk_table_attach(GTK_TABLE(table), avatarFour, 3, 6, 7, 9, GTK_FILL, GTK_FILL, 0, 0);

    //Player Five
    if(PlayerFive.username[0] != '\0'){
        avatarFive = gtk_image_new_from_file("./goldfishavatar/goldfishbloop.png");
    }else{
        avatarFive = gtk_image_new_from_file("./goldfishavatar/empty.png");
    }
    gtk_widget_set_size_request((GtkWidget *) avatarFive, 50 * 3, 100);
    gtk_table_attach(GTK_TABLE(table), avatarFive, 3, 6, 9, 11, GTK_FILL, GTK_FILL, 0, 0);

    //Player Six
    if(PlayerSix.username[0] != '\0'){
        avatarSix = gtk_image_new_from_file("./goldfishavatar/goldfishbloop.png");
    }else{
        avatarSix = gtk_image_new_from_file("./goldfishavatar/empty.png");
    }
    gtk_widget_set_size_request((GtkWidget *) avatarSix, 50 * 3, 100);
    gtk_table_attach(GTK_TABLE(table), avatarSix, 3, 6, 11, 13, GTK_FILL, GTK_FILL, 0, 0);

//************************************ End of Creating Character****************************


//********************************Creating Cards 6-12 Incomplete
    


    PlayerOneCardoneTop = gtk_image_new_from_file("./cardsicon/backtop.png");
    PlayerOneCardoneBot = gtk_image_new_from_file("./cardsicon/backbot.png");
    gtk_table_attach(GTK_TABLE(table), PlayerOneCardoneTop, 7, 8, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), PlayerOneCardoneBot, 7, 8, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
    PlayerOneCardtwoTop = gtk_image_new_from_file("./cardsicon/Kingtop.png");
    PlayerOneCardtwoBot = gtk_image_new_from_file("./cardsicon/diamondbot.png");
    gtk_table_attach(GTK_TABLE(table), PlayerOneCardtwoTop, 10, 11, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), PlayerOneCardtwoBot, 10, 11, 2, 3, GTK_FILL, GTK_FILL, 0, 0);

    
    if(PlayerTwo.username[0] == '\0')
    {   
        //CardOneTopSub = "./cardsicon/backTop.png";
        strcpy(CardOneTopSub, "./cardsicon/backtop.png");
        strcpy(CardOneBotSub, "./cardsicon/backbot.png");
        strcpy(CardTwoTopSub, "./cardsicon/backtop.png");
        strcpy(CardTwoBotSub, "./cardsicon/backbot.png");
    }else{
        //Testing Purposes Acccess each player information
        //Call a function
        strcpy(CardOneTopSub, "./cardsicon/Tentop.png");
        strcpy(CardOneBotSub, "./cardsicon/backbot.png");
        strcpy(CardTwoTopSub, "./cardsicon/Twotop.png");
        strcpy(CardTwoBotSub, "./cardsicon/backtop.png");
    }
    PlayerTwoCardoneTop = gtk_image_new_from_file(CardOneTopSub);
    PlayerTwoCardoneBot = gtk_image_new_from_file(CardOneBotSub);
    PlayerTwoCardtwoTop = gtk_image_new_from_file(CardTwoTopSub);
    PlayerTwoCardtwoBot = gtk_image_new_from_file(CardTwoBotSub);
    gtk_table_attach(GTK_TABLE(table), PlayerTwoCardoneTop, 7, 8, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), PlayerTwoCardoneBot, 7, 8, 4, 5, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), PlayerTwoCardtwoTop, 10, 11, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), PlayerTwoCardtwoBot, 10, 11, 4, 5, GTK_FILL, GTK_FILL, 0, 0);


    //Use for Update Example **********
    //gtk_image_set_from_file((GtkImage *)PlayerTwoCardoneTop,"./cardsicon/Atop.png");
    //Use for Updating ********************

//***************************************Community Cards*****************************************
//Default
    strcpy(CCOneTop, "./cardsicon/backtop.png");
    strcpy(CCOneBot, "./cardsicon/backbot.png");
    strcpy(CCTwoTop, "./cardsicon/backtop.png");
    strcpy(CCTwoBot, "./cardsicon/backbot.png");
    strcpy(CCThreeTop, "./cardsicon/backtop.png");
    strcpy(CCThreeBot, "./cardsicon/backbot.png");
    strcpy(CCFourTop, "./cardsicon/backtop.png");
    strcpy(CCFourBot, "./cardsicon/backbot.png");
    strcpy(CCFiveTop, "./cardsicon/backtop.png");
    strcpy(CCFiveBot, "./cardsicon/backbot.png");

    CCOneTopImage   = gtk_image_new_from_file(CCOneTop);
    CCOneBotImage   = gtk_image_new_from_file(CCOneBot);
    CCTwoTopImage   = gtk_image_new_from_file(CCTwoTop);
    CCTwoBotImage   = gtk_image_new_from_file(CCTwoBot);
    CCThreeTopImage = gtk_image_new_from_file(CCThreeTop);
    CCThreeBotImage = gtk_image_new_from_file(CCThreeBot);
    CCFourTopImage  = gtk_image_new_from_file(CCFourTop);
    CCFourBotImage  = gtk_image_new_from_file(CCFourBot);
    CCFiveTopImage  = gtk_image_new_from_file(CCFiveTop);
    CCFiveBotImage  = gtk_image_new_from_file(CCFiveBot);


    gtk_table_attach(GTK_TABLE(table), CCOneTopImage, 1, 2, 14, 15, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), CCOneBotImage, 1, 2, 15, 16, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), CCTwoTopImage, 3, 4, 14, 15, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), CCTwoBotImage, 3, 4, 15, 16, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), CCThreeTopImage, 5, 6, 14, 15, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), CCThreeBotImage, 5, 6, 15, 16, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), CCFourTopImage, 7, 8, 14, 15, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), CCFourBotImage, 7, 8, 15, 16, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), CCFiveTopImage, 9, 10, 14, 15, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), CCFiveBotImage, 9, 10, 15, 16, GTK_FILL, GTK_FILL, 0, 0);




//***************************************End of Community Cards *********************************

//***************************************Pot Labels ********************************************
    int testingPotsize = 99999;
    sprintf(PotBuffer,"%d", testingPotsize);
    PotTotalSizeLabel = (GtkLabel *) gtk_label_new(PotBuffer);
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PotTotalSizeLabel, 15, 23, 14, 16, GTK_FILL, GTK_FILL, 0, 0);






//****************************************End of Pot Labels*************************************


//************************************End of Creating PLayers************************************
    /* make sure that everything becomes visible */
    gtk_container_add(GTK_CONTAINER(Window), table);
    gtk_widget_show_all(Window);


    
    /* make clock widgets public */
    
    /* connect window-close with function terminating this server */
    g_signal_connect(Window, "destroy", G_CALLBACK(ShutdownClicked), NULL);

    return(Window);
} /* end of CreateWindow */



//Need this code
void UpdateWindow(void)		/* render the window on screen */
{
   /* this server has it's own main loop for handling client connections;
    * as such, it can't have the usual GUI main loop (gtk_main);
    * instead, we call this UpdateWindow function on regular basis
    */
    while(gtk_events_pending())
    {
	gtk_main_iteration();
    }
} /* end of UpdateWindow */

void DisplayCurrentTime(void)	/* update and display the current real time */
{
    time_t CurrentTime;
    char   *TimeString;

    CurrentTime = time(NULL);
    TimeString = ctime(&CurrentTime);
    strncpy(ClockBuffer, TimeString, 25);
    ClockBuffer[24] = 0;
#ifdef DEBUG
    printf("\r%s: current time is %s", Program, ClockBuffer);
    fflush(stdout);
#endif
    if (DigitalClock)	/* if digital clock is functional, */
    {
	gtk_label_set_label(DigitalClock, ClockBuffer);	/* update it! */
    }
    if (AnalogClock)	/* if analog clock is functional, */
    {
	gdk_window_invalidate_rect(AnalogClock->window,	/* update it! */
		&AnalogClock->allocation, /* invalidate entire region */
		FALSE);	/* no children */
    }
} /* end of DisplayCurrentTime */

void DisplayUpdatedCards(void)//Should replace DisplayCurrentTime to update GUI
{
    //Check if User Existed
    //Updates Cards, Avatars, Money, Rank, Pot, Community Cards





}



/*** global functions ****************************************************/

void FatalError(		/* print error diagnostics and abort */
	const char *ErrorMsg)
{
    fputs(Program, stderr);
    fputs(": ", stderr);
    perror(ErrorMsg);
    fputs(Program, stderr);
    fputs(": Exiting!\n", stderr);
    exit(20);
} /* end of FatalError */

int MakeServerSocket(		/* create a socket on this server */
	uint16_t PortNo)
{
    int ServSocketFD;
    struct sockaddr_in ServSocketName;

    /* create the socket */
    ServSocketFD = socket(PF_INET, SOCK_STREAM, 0);
    if (ServSocketFD < 0)
    {   FatalError("service socket creation failed");
    }
    /* bind the socket to this server */
    ServSocketName.sin_family = AF_INET;
    ServSocketName.sin_port = htons(PortNo);
    ServSocketName.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(ServSocketFD, (struct sockaddr*)&ServSocketName,
		sizeof(ServSocketName)) < 0)
    {   FatalError("binding the server to a socket failed");
    }
    /* start listening to this socket */
    if (listen(ServSocketFD, 5) < 0)	/* max 5 clients in backlog */
    {   FatalError("listening on socket failed");
    }
    return ServSocketFD;
} /* end of MakeServerSocket */

void ProcessRequest(		/* process a time request by a client */
	int DataSocketFD)
{
    int  l, n;
    char RecvBuf[256];	/* message buffer for receiving a message */
    char SendBuf[256];	/* message buffer for sending a response */

    n = read(DataSocketFD, RecvBuf, sizeof(RecvBuf)-1);
    if (n < 0) 
    {   FatalError("reading from data socket failed");
    }
    RecvBuf[n] = 0;
#ifdef DEBUG
    printf("%s: Received message: %s\n", Program, RecvBuf);
#endif
    if (0 == strcmp(RecvBuf, "TIME"))
    {   strncpy(SendBuf, "OK TIME: ", sizeof(SendBuf)-1);
	SendBuf[sizeof(SendBuf)-1] = 0;
	strncat(SendBuf, ClockBuffer, sizeof(SendBuf)-1-strlen(SendBuf));
    }
    else if (0 == strcmp(RecvBuf, "SHUTDOWN"))
    {   Shutdown = 1;
	strncpy(SendBuf, "OK SHUTDOWN", sizeof(SendBuf)-1);
	SendBuf[sizeof(SendBuf)-1] = 0;
    }
    else
    {   strncpy(SendBuf, "ERROR unknown command ", sizeof(SendBuf)-1);
	SendBuf[sizeof(SendBuf)-1] = 0;
	strncat(SendBuf, RecvBuf, sizeof(SendBuf)-1-strlen(SendBuf));
    }
    l = strlen(SendBuf);
#ifdef DEBUG
    printf("%s: Sending response: %s.\n", Program, SendBuf);
#endif
    n = write(DataSocketFD, SendBuf, l);
    if (n < 0)
    {   FatalError("writing to data socket failed");
    }
} /* end of ProcessRequest */



void ServerMainLoop(		/* simple server main loop */
	int ServSocketFD,		/* server socket to wait on */
	ClientHandler HandleClient,	/* client handler to call */
	TimeoutHandler HandleTimeout,	/* timeout handler to call */
	int Timeout)			/* timeout in micro seconds */
{
    int DataSocketFD;	/* socket for a new client */
    socklen_t ClientLen;
    struct sockaddr_in
	ClientAddress;	/* client address we connect with */
    fd_set ActiveFDs;	/* socket file descriptors to select from */
    fd_set ReadFDs;	/* socket file descriptors ready to read from */
    struct timeval TimeVal;
    int res, i;

    FD_ZERO(&ActiveFDs);		/* set of active sockets */
    FD_SET(ServSocketFD, &ActiveFDs);	/* server socket is active */
    while(!Shutdown)
    {
	UpdateWindow();	/* update the GUI window */
	ReadFDs = ActiveFDs;
	TimeVal.tv_sec  = Timeout / 1000000;
	TimeVal.tv_usec = Timeout % 1000000;
	/* block until input arrives on active sockets or until timeout */
	res = select(FD_SETSIZE, &ReadFDs, NULL, NULL, &TimeVal);
	if (res < 0)
	{   FatalError("wait for input or timeout (select) failed");
	}
	if (res == 0)	/* timeout occurred */
	{
	    HandleTimeout();
	}
	else		/* some FDs have data ready to read */
	{   for(i=0; i<FD_SETSIZE; i++)
	    {   if (FD_ISSET(i, &ReadFDs))
		{   if (i == ServSocketFD)
		    {	/* connection request on server socket */
#ifdef DEBUG
			printf("%s: Accepting new client...\n", Program);
#endif
			ClientLen = sizeof(ClientAddress);
			DataSocketFD = accept(ServSocketFD,
				(struct sockaddr*)&ClientAddress, &ClientLen);
			if (DataSocketFD < 0)
			{   FatalError("data socket creation (accept) failed");
			}
#ifdef DEBUG
			printf("%s: New client connected from %s:%hu.\n",
				Program,
				inet_ntoa(ClientAddress.sin_addr),
				ntohs(ClientAddress.sin_port));
#endif
			FD_SET(DataSocketFD, &ActiveFDs);
		    }
		    else
		    {   /* active communication with a client */
#ifdef DEBUG
			printf("%s: Dealing with client on FD%d...\n",
				Program, i);
#endif
			HandleClient(i);
#ifdef DEBUG
			printf("%s: Closing client connection FD%d.\n",
				Program, i);
#endif
			close(i);
			FD_CLR(i, &ActiveFDs);
		    }
		}
	    }
	}
    }
} /* end of ServerMainLoop */



/*** main function *******************************************************/

int main(			/* the main function */
	int argc,
	char *argv[])
{
    int ServSocketFD;	/* socket file descriptor for service */
    int PortNo;		/* port number */
    GtkWidget *Window;	/* the server window */

    //Program = argv[0];	/* publish program name (for diagnostics) */
#ifdef DEBUG
    printf("%s: Starting...\n", Program);
#endif
    if (argc < 2)
    {   fprintf(stderr, "Usage: %s port\n", Program);
	exit(10);
    }
    PortNo = atoi(argv[1]);	/* get the port number */
    if (PortNo <= 2000)
    {   fprintf(stderr, "%s: invalid port number %d, should be >10111\n",
		Program, PortNo);
        exit(10);
    }
#ifdef DEBUG
    printf("%s: Creating the server socket...\n", Program);
#endif
    ServSocketFD = MakeServerSocket(PortNo);

#ifdef DEBUG
    printf("%s: Creating the server window...\n", Program);
#endif


//*************Only for testing purposes
    strcpy(PlayerOne.username, "testusername");
    printf("\nTesting Playerone: %s\n", PlayerOne.username);

    PlayerOne.money = PointsTest;
    PlayerTwo.money = PointsTest + 500;
    PlayerThree.money = PointsTest - 200;
    PlayerFour.money = -1;
    PlayerFive.money = -1;
    PlayerSix.money = -1;

    PlayerOne.token = dealer;
    PlayerTwo.token = smallBlind;
    PlayerThree.token = bigBlind;
    PlayerFour.token = bigBlind;
    PlayerFive.token = none;
    PlayerSix.token = none;



    PlayerTwo.pocket[0].suit = heart;
    PlayerTwo.pocket[0].num = ace;
    PlayerTwo.pocket[1].num =queen;
    PlayerTwo.pocket[1].suit = club;

//*************ENd of testing Purposes**********

    //Receive Start signal than create windows
    Window = CreateWindow(&argc, &argv);
    if (!Window)
    {   fprintf(stderr, "%s: cannot create GUI window\n", Program);
        exit(10);
    }
#ifdef DEBUG
    printf("%s: Providing current time at port %d...\n", Program, PortNo);
#endif
    ServerMainLoop(ServSocketFD, ProcessRequest,
			DisplayUpdatedCards, 250000);//DisplayUpdateCards Updates GUI
#ifdef DEBUG
    printf("\n%s: Shutting down.\n", Program);
#endif
    close(ServSocketFD);
    return 0;
} /* end of main */

/* EOF GTK_ClockServer.c */
