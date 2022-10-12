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
#include "table.h"
#include "deck.h"
#include "com.h"





/* #define DEBUG */	/* be verbose */
#define BUFFSIZE 256




//Convert to Strings Function




/*** type definitions ****************************************************/

typedef void (*ClientHandler)(int DataSocketFD);
typedef void (*TimeoutHandler)(Table table);

//New Server
typedef Move (*ClientHandler_V2)(int DataSocketFD, int playerNum, int *quasi_shutdown, Table table);

typedef Move (*MoveGetter)(int ServSocketFD, int Timeout, fd_set* ActiveFDs, Table table, int playerNum, ClientHandler_V2 HandleClient);


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
#define WindowWidth (1200 + 2 * (WindowBorder))
#define WindowHeight (1000 + 2 * (WindowBorder))
/*****************************Testing Purposes******************************/
//Keep this
Player PlayerOne;
Player PlayerTwo;
Player PlayerThree;
Player PlayerFour;
Player PlayerFive;
Player PlayerSix;
Card CommunityCard[5];
char UsernameText[] = "Username: ";
char PointsBuffer[20];
char RankBuffer[20];
//MB delete this
char testusername[] = "bobross";
int PointsTest = 500;
int GlobalPotSize = 0;

Table table;
Table subtable;
int totPlayers;
int startingMoney;

/*************************End of Testing Purposes***************************/

char* GetCardTop(Player PlayerCardTop, int CardNumber);
char* GetCardBot(Player PlayerCardBot, int CardNumber);
char* GetStatus(Player PlayerStatus);
char* GetCCTop(Card CCTop);
char* GetCCBot(Card CCBot);
void SetToDefault();
void TestUserINFO();
void DisplayUpdatedCards();

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
GtkWidget *PThreeCardonebot;


GtkWidget *PlayerFourCardoneTop;
GtkWidget *PlayerFourCardoneBot;
//GdkPixbuf *PlayerFourCardoneBot;
GtkWidget *PlayerFourCardtwoTop;
GtkWidget *PlayerFourCardtwoBot;

GtkWidget *PFourCardonebot;





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
GtkWidget *CCTwoBotSub;
GtkWidget *CCThreeTopImage;
GtkWidget *CCThreeBotImage;
GtkWidget *CCFourTopImage;
GtkWidget *CCFourTopSub;
GtkWidget *CCFourBotImage;
GtkWidget *CCFiveTopImage;
GtkWidget *CCFiveBotImage;

//Pot Size 
GtkLabel *PotTotalSizeLabel;
char PotBuffer[20];

GtkWidget *Randomizebtn;
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

void Randomizebtn_clicked(GtkWidget *widget, gpointer data)
{
    g_print("Randomizing Info\n");
    TestUserINFO();
}



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
    //Creating A Randomizer
    Randomizebtn = gtk_button_new_with_label("Randomize Info");
    gtk_widget_set_size_request(Randomizebtn, 50, 50);
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) Randomizebtn, 20, 21, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
    g_signal_connect(Randomizebtn, "clicked", G_CALLBACK(Randomizebtn_clicked), NULL);

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

    //strcat(dest, src) Add two strings into one
    
    //subUser = UsernameText;

    //Default
    PlayerOneUser   = (GtkLabel *) gtk_label_new("Empty/Vacant");
    PlayerTwoUser   = (GtkLabel *) gtk_label_new("Empty/Vacant");
    PlayerThreeUser = (GtkLabel *) gtk_label_new("Empty/Vacant");
    PlayerFourUser  = (GtkLabel *) gtk_label_new("Empty/Vacant");
    PlayerFiveUser  = (GtkLabel *) gtk_label_new("Empty/Vacant");
    PlayerSixUser   = (GtkLabel *) gtk_label_new("Empty/Vacant");

    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerOneUser, 0, 3, 1, 3, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerTwoUser, 0, 3, 3, 5, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerThreeUser, 0, 3, 5, 7, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerFourUser, 0, 3, 7, 9, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerFiveUser, 0, 3, 9, 11, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerSixUser, 0, 3, 11, 13, GTK_FILL, GTK_FILL, 0, 0);
    //*******************************************End of Creating Username Labels *************************

    //**********************************Creating Points Label**********************************

    //Default Set Points to -1
    PlayerOnePoints = (GtkLabel *) gtk_label_new("0");
    PlayerTwoPoints = (GtkLabel *) gtk_label_new("0");
    PlayerThreePoints = (GtkLabel *) gtk_label_new("0");
    PlayerFourPoints = (GtkLabel *) gtk_label_new("0");
    PlayerFivePoints = (GtkLabel *) gtk_label_new("0");
    PlayerSixPoints = (GtkLabel *) gtk_label_new("0");
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerOnePoints, 12, 15, 1, 3, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerTwoPoints, 12, 15, 3, 5, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerThreePoints, 12, 15, 5, 7, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerFourPoints, 12, 15, 7, 9, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerFivePoints, 12, 15, 9, 11, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerSixPoints, 12, 15, 11, 13, GTK_FILL, GTK_FILL, 0, 0);
    //***************************************** End of Creating Points Label ***********************************

    //****************************************Creating Rank Label ******************************************
    //Default
    PlayerOneRank   = (GtkLabel *) gtk_label_new("Player does not exist");
    PlayerTwoRank   = (GtkLabel *) gtk_label_new("Player does not exist");
    PlayerThreeRank = (GtkLabel *) gtk_label_new("Player does not exist");
    PlayerFourRank  = (GtkLabel *) gtk_label_new("Player does not exist");
    PlayerFiveRank  = (GtkLabel *) gtk_label_new("Player does not exist");
    PlayerSixRank   = (GtkLabel *) gtk_label_new("Player does not exist");
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerOneRank, 15, 18, 1, 3, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerTwoRank, 15, 18, 3, 5, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerThreeRank, 15, 18, 5, 7, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerFourRank, 15, 18, 7, 9, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerFiveRank, 15, 18, 9, 11, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), (GtkWidget *) PlayerSixRank, 15, 18, 11, 13, GTK_FILL, GTK_FILL, 0, 0);
//*******************************Avatar/Character*****************************
    
    //Do this later
    //char subimageline[]= "./src/assets/goldfishavatar/empty.png";
    avatarOne   = gtk_image_new_from_file("./src/assets/goldfishavatar/bigbrainfish.png");
    avatarTwo   = gtk_image_new_from_file("./src/assets/goldfishavatar/blackgoldfish.png");
    avatarThree = gtk_image_new_from_file("./src/assets/goldfishavatar/goldfishbloop.png");
    avatarFour  = gtk_image_new_from_file("./src/assets/goldfishavatar/pixelfish.png");
    avatarFive  = gtk_image_new_from_file("./src/assets/goldfishavatar/susfish.png");
    avatarSix   = gtk_image_new_from_file("./src/assets/goldfishavatar/cracker.png");
    gtk_widget_set_size_request((GtkWidget *)avatarOne, 50 * 3, 100);
    gtk_table_attach(GTK_TABLE(table), avatarOne, 3, 6, 1, 3, GTK_FILL, GTK_FILL, 0, 0);
    gtk_widget_set_size_request((GtkWidget *)avatarTwo, 50 * 3, 100);
    gtk_table_attach(GTK_TABLE(table), avatarTwo, 3, 6, 3, 5, GTK_FILL, GTK_FILL, 0, 0);
    gtk_widget_set_size_request((GtkWidget *)avatarThree, 50 * 3, 100);
    gtk_table_attach(GTK_TABLE(table), avatarThree, 3, 6, 5, 7, GTK_FILL, GTK_FILL, 0, 0);
    gtk_widget_set_size_request((GtkWidget *)avatarFour, 50 * 3, 100);
    gtk_table_attach(GTK_TABLE(table), avatarFour, 3, 6, 7, 9, GTK_FILL, GTK_FILL, 0, 0);
    gtk_widget_set_size_request((GtkWidget *) avatarFive, 50 * 3, 100);
    gtk_table_attach(GTK_TABLE(table), avatarFive, 3, 6, 9, 11, GTK_FILL, GTK_FILL, 0, 0);
    gtk_widget_set_size_request((GtkWidget *) avatarSix, 50 * 3, 100);
    gtk_table_attach(GTK_TABLE(table), avatarSix, 3, 6, 11, 13, GTK_FILL, GTK_FILL, 0, 0);

//************************************ End of Creating Character****************************


//********************************Creating Cards 6-12 Incomplete
    //Default

    //Player One
    PlayerOneCardoneTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    PlayerOneCardoneBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    PlayerOneCardtwoTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    PlayerOneCardtwoBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    gtk_table_attach(GTK_TABLE(table), PlayerOneCardoneTop, 7, 8, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), PlayerOneCardoneBot, 7, 8, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), PlayerOneCardtwoTop, 10, 11, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), PlayerOneCardtwoBot, 10, 11, 2, 3, GTK_FILL, GTK_FILL, 0, 0);


    //Player Two
    PlayerTwoCardoneTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    PlayerTwoCardoneBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    PlayerTwoCardtwoTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    PlayerTwoCardtwoBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    gtk_table_attach(GTK_TABLE(table), PlayerTwoCardoneTop, 7, 8, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), PlayerTwoCardoneBot, 7, 8, 4, 5, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), PlayerTwoCardtwoTop, 10, 11, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), PlayerTwoCardtwoBot, 10, 11, 4, 5, GTK_FILL, GTK_FILL, 0, 0);

    //Player Three
    PlayerThreeCardoneTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    PlayerThreeCardoneBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    PlayerThreeCardtwoTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    PlayerThreeCardtwoBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    gtk_table_attach(GTK_TABLE(table), PlayerThreeCardoneTop, 7, 8, 5, 6, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), PlayerThreeCardoneBot, 7, 8, 6, 7, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), PlayerThreeCardtwoTop, 10, 11, 5, 6, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), PlayerThreeCardtwoBot, 10, 11, 6, 7, GTK_FILL, GTK_FILL, 0, 0);

    PThreeCardonebot = PlayerThreeCardoneBot;


    //Player Four
    PlayerFourCardoneTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    PlayerFourCardoneBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    //PlayerFourCardoneBot = gdk_pixbuf_new_from_file("./src/assets/cardsicon/backtop.png", NULL);
    PlayerFourCardtwoTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    PlayerFourCardtwoBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    gtk_table_attach(GTK_TABLE(table), PlayerFourCardoneTop, 7, 8, 7, 8, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), PlayerFourCardoneBot, 7, 8, 8, 9, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), PlayerFourCardtwoTop, 10, 11, 7, 8, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), PlayerFourCardtwoBot, 10, 11, 8, 9, GTK_FILL, GTK_FILL, 0, 0);

    PFourCardonebot = PlayerFourCardoneBot;

    //Player Five
    PlayerFiveCardoneTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    PlayerFiveCardoneBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    PlayerFiveCardtwoTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    PlayerFiveCardtwoBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    gtk_table_attach(GTK_TABLE(table), PlayerFiveCardoneTop, 7, 8, 9, 10, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), PlayerFiveCardoneBot, 7, 8, 10, 11, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), PlayerFiveCardtwoTop, 10, 11, 9, 10, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), PlayerFiveCardtwoBot, 10, 11, 10, 11, GTK_FILL, GTK_FILL, 0, 0);

    //Player Six
    PlayerSixCardoneTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    PlayerSixCardoneBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    PlayerSixCardtwoTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    PlayerSixCardtwoBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    gtk_table_attach(GTK_TABLE(table), PlayerSixCardoneTop, 7, 8, 11, 12, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), PlayerSixCardoneBot, 7, 8, 12, 13, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), PlayerSixCardtwoTop, 10, 11, 11, 12, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), PlayerSixCardtwoBot, 10, 11, 12, 13, GTK_FILL, GTK_FILL, 0, 0);


    //Use for Update Example **********
    //gtk_image_set_from_file((GtkImage *)PlayerTwoCardoneTop,"./cardsicon/Atop.png");
    //Use for Updating ********************

//***************************************Community Cards*****************************************
//Default
    strcpy(CCOneTop, "./src/assets/cardsicon/backtop.png");
    strcpy(CCOneBot, "./src/assets/cardsicon/backbot.png");
    strcpy(CCTwoTop, "./src/assets/cardsicon/backtop.png");
    strcpy(CCTwoBot, "./src/assets/cardsicon/backbot.png");
    strcpy(CCThreeTop, "./src/assets/cardsicon/backtop.png");
    strcpy(CCThreeBot, "./src/assets/cardsicon/backbot.png");
    strcpy(CCFourTop, "./src/assets/cardsicon/backtop.png");
    strcpy(CCFourBot, "./src/assets/cardsicon/backbot.png");
    strcpy(CCFiveTop, "./src/assets/cardsicon/backtop.png");
    strcpy(CCFiveBot, "./src/assets/cardsicon/backbot.png");
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

    CCTwoBotSub = CCTwoBotImage;
    CCFourTopSub = CCFourTopImage;




//***************************************End of Community Cards *********************************


//*********************************************Status 21-24********************************************

PlayerOneStatus     = (GtkLabel *) gtk_label_new("Null");
PlayerTwoStatus     = (GtkLabel *) gtk_label_new("Null");
PlayerThreeStatus   = (GtkLabel *) gtk_label_new("Null");
PlayerFourStatus    = (GtkLabel *) gtk_label_new("Null");
PlayerFiveStatus    = (GtkLabel *) gtk_label_new("Null");
PlayerSixStatus     = (GtkLabel *) gtk_label_new("Null");
gtk_table_attach(GTK_TABLE(table), (GtkWidget *)PlayerOneStatus, 18, 21, 1, 3, GTK_FILL, GTK_FILL, 0, 0);
gtk_table_attach(GTK_TABLE(table), (GtkWidget *)PlayerTwoStatus, 18, 21, 3, 5, GTK_FILL, GTK_FILL, 0, 0);
gtk_table_attach(GTK_TABLE(table), (GtkWidget *)PlayerThreeStatus, 18, 21, 5, 7, GTK_FILL, GTK_FILL, 0, 0);
gtk_table_attach(GTK_TABLE(table), (GtkWidget *)PlayerFourStatus, 18, 21, 7, 9, GTK_FILL, GTK_FILL, 0, 0);
gtk_table_attach(GTK_TABLE(table), (GtkWidget *)PlayerFiveStatus, 18, 21, 9, 11, GTK_FILL, GTK_FILL, 0, 0);
gtk_table_attach(GTK_TABLE(table), (GtkWidget *)PlayerSixStatus, 18, 21, 11, 13, GTK_FILL, GTK_FILL, 0, 0);

//****************************************End of Status**************************************
//***************************************Pot Labels ********************************************

    //Default
    PotTotalSizeLabel = (GtkLabel *) gtk_label_new("0");
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



void DisplayUpdatedCards()//Should replace DisplayCurrentTime to update GUI
{
    //Check if User Existed
    //Updates Cards, Avatars, Money, Rank, Pot, Community Cards
    //Update Cards Code
int n;
if(table.totPlayers <= 1){
PlayerOne = table.players[0];
strcpy(PlayerOne.username, "TestOne");
}
if(table.totPlayers <= 2){
PlayerOne = table.players[0];
PlayerTwo = table.players[1];
strcpy(PlayerOne.username, "TestOne");
strcpy(PlayerTwo.username, "TestTwo");
}
if(table.totPlayers <= 3){
PlayerOne = table.players[0];
PlayerTwo = table.players[1];
PlayerThree = table.players[2];
strcpy(PlayerOne.username, "TestOne");
strcpy(PlayerTwo.username, "TestTwo");
strcpy(PlayerThree.username, "TestThree");
}
if(table.totPlayers <= 4){
PlayerOne = table.players[0];
PlayerTwo = table.players[1];
PlayerThree = table.players[2];
PlayerFour = table.players[3];
strcpy(PlayerOne.username, "TestOne");
strcpy(PlayerTwo.username, "TestTwo");
strcpy(PlayerThree.username, "TestThree");
strcpy(PlayerFour.username, "TestFour");
}
if(table.totPlayers <= 5){
PlayerOne = table.players[0];
PlayerTwo = table.players[1];
PlayerThree = table.players[2];
PlayerFour = table.players[3];
PlayerFive = table.players[4];
strcpy(PlayerOne.username, "TestOne");
strcpy(PlayerTwo.username, "TestTwo");
strcpy(PlayerThree.username, "TestThree");
strcpy(PlayerFour.username, "TestFour");
strcpy(PlayerFive.username, "TestFive");
}
if(table.totPlayers <= 6){
PlayerOne = table.players[0];
PlayerTwo = table.players[1];
PlayerThree = table.players[2];
PlayerFour = table.players[3];
PlayerFive = table.players[4];
PlayerSix = table.players[5];
strcpy(PlayerOne.username, "TestOne");
strcpy(PlayerTwo.username, "TestTwo");
strcpy(PlayerThree.username, "TestThree");
strcpy(PlayerFour.username, "TestFour");
strcpy(PlayerFive.username, "TestFive");
strcpy(PlayerSix.username, "TestSix");
}




/*
for(n =0; n<5; n++){
CommunityCard[n] = table.tableCards[n];
}
*/


//remove later
//PrintCurrentBalance(table);

/*
printf("CC 1 =  %d \n", CommunityCard[0].num);
printf("CC 1 =  %d \n", CommunityCard[0].suit);

printf("TC 1 =  %d \n", table.tableCards[0].num);
printf("TC 1 =  %d \n", table.tableCards[0].suit);

printf("CC 2 =  %d \n", CommunityCard[1].num);
printf("CC 2 =  %d \n", CommunityCard[1].suit);

printf("TC 2 =  %d \n", table.tableCards[1].num);
printf("TC 2 =  %d \n", table.tableCards[1].suit);

printf("CC 3 =  %d \n", CommunityCard[2].num);
printf("CC 3 =  %d \n", CommunityCard[2].suit);

printf("TC 3 =  %d \n", table.tableCards[2].num);
printf("TC 3 =  %d \n", table.tableCards[2].suit);

printf("CC 4 =  %d \n", CommunityCard[3].num);
printf("CC 4 =  %d \n", CommunityCard[3].suit);

printf("TC 4 =  %d \n", table.tableCards[3].num);
printf("TC 4 =  %d \n", table.tableCards[3].suit);

printf("CC 5 =  %d \n", CommunityCard[4].num);
printf("CC 5 =  %d \n", CommunityCard[4].suit);

printf("TC 5 =  %d \n", table.tableCards[4].num);
printf("TC 5 =  %d \n", table.tableCards[4].suit);
*/
//Update Cards Code






char subUser[50];
//PotSize
sprintf(PotBuffer,"%d", GlobalPotSize);
gtk_label_set_text(PotTotalSizeLabel, PotBuffer);

char CardoneTopSub[40];
char CardtwoTopSub[40];
char CardoneBotSub[40];
char CardtwoBotSub[40];
char CCTopSub[40];
char CCBotSub[40];
char StatusSub[10];
//int CCounter = 0;
//Player 1
    if(PlayerOne.username [0] != '\0'){
        //Copy Username from struc to label
        strcpy(subUser, UsernameText);
        strcat(subUser, PlayerOne.username);
        gtk_label_set_text(PlayerOneUser, subUser);
        //Copy Points to label
        if(PlayerOne.money == -1){
            gtk_label_set_text(PlayerOnePoints, "0");
        }else{
            sprintf(PointsBuffer,"%d", PlayerOne.money);
            gtk_label_set_text(PlayerOnePoints, PointsBuffer);
        }
        //Rank
        switch(PlayerOne.token)
            {
                case 0:
                    gtk_label_set_text(PlayerOneRank, "Dealer");
                    break;
                case 1:
                    gtk_label_set_text(PlayerOneRank, "Small Blind");
                    break;
                case 2:
                    gtk_label_set_text(PlayerOneRank, "Big Blind");
                    break;
                case 3:
                    gtk_label_set_text(PlayerOneRank, "Normal");
                    break;
                default:
                    gtk_label_set_text(PlayerOneRank, "Player does not exist");
                    break;
            }
        //CardHand
        strcpy(CardoneTopSub, GetCardTop(PlayerOne, 0));
        strcpy(CardoneBotSub, GetCardBot(PlayerOne, 0));
        strcpy(CardtwoTopSub, GetCardTop(PlayerOne, 1));
        strcpy(CardtwoBotSub, GetCardBot(PlayerOne, 1));
        gtk_image_set_from_file((GtkImage *)PlayerOneCardoneTop, CardoneTopSub);
        gtk_image_set_from_file((GtkImage *)PlayerOneCardoneBot, CardoneBotSub);
        gtk_image_set_from_file((GtkImage *)PlayerOneCardtwoTop, CardtwoTopSub);
        gtk_image_set_from_file((GtkImage *)PlayerOneCardtwoBot, CardtwoBotSub);
        //Status
        strcpy(StatusSub,GetStatus(PlayerOne));
        gtk_label_set_text(PlayerOneStatus, StatusSub);



        }



//Player 2
    if(PlayerTwo.username [0] != '\0'){
        //Copy Username from struc to array
        strcpy(subUser, UsernameText);
        strcat(subUser, PlayerTwo.username);
        gtk_label_set_text(PlayerTwoUser, subUser);
        //Copy Points to label
        if(PlayerTwo.money == -1){
            gtk_label_set_text(PlayerTwoPoints, "0");
        }else{
            sprintf(PointsBuffer,"%d", PlayerTwo.money);
            gtk_label_set_text(PlayerTwoPoints, PointsBuffer);
        }
        switch(PlayerTwo.token)
        {
            case 0:
                gtk_label_set_text(PlayerTwoRank, "Dealer");
                break;
            case 1:
                gtk_label_set_text(PlayerTwoRank, "Small Blind");
                break;
            case 2:
                gtk_label_set_text(PlayerTwoRank, "Big Blind");
                break;
            case 3:
                gtk_label_set_text(PlayerTwoRank, "Normal");
                break;
            default:
                gtk_label_set_text(PlayerTwoRank, "Player does not exist");
                break;
        }
        //CardHand
        strcpy(CardoneTopSub, GetCardTop(PlayerTwo, 0));
        strcpy(CardoneBotSub, GetCardBot(PlayerTwo, 0));
        strcpy(CardtwoTopSub, GetCardTop(PlayerTwo, 1));
        strcpy(CardtwoBotSub, GetCardBot(PlayerTwo, 1));
        gtk_image_set_from_file((GtkImage *)PlayerTwoCardoneTop, CardoneTopSub);
        gtk_image_set_from_file((GtkImage *)PlayerTwoCardoneBot, CardoneBotSub);
        gtk_image_set_from_file((GtkImage *)PlayerTwoCardtwoTop, CardtwoTopSub);
        gtk_image_set_from_file((GtkImage *)PlayerTwoCardtwoBot, CardtwoBotSub);

        //Status
        strcpy(StatusSub,GetStatus(PlayerTwo));
        gtk_label_set_text(PlayerTwoStatus, StatusSub);

        }
//Player 3
    if(PlayerThree.username [0] != '\0'){
        //Copy Username from struc to array
        strcpy(subUser, UsernameText);
        strcat(subUser, PlayerThree.username);
        gtk_label_set_text(PlayerThreeUser, subUser);
        //Copy Points to label
        if(PlayerThree.money == -1){
            gtk_label_set_text(PlayerThreePoints, "0");
        }else{
            sprintf(PointsBuffer,"%d", PlayerThree.money);
            gtk_label_set_text(PlayerThreePoints, PointsBuffer);
        }

        //Rank
        switch(PlayerThree.token)
            {
                case 0:
                    gtk_label_set_text(PlayerThreeRank, "Dealer");
                    break;
                case 1:
                    gtk_label_set_text(PlayerThreeRank, "Small Blind");
                    break;
                case 2:
                    gtk_label_set_text(PlayerThreeRank, "Big Blind");
                    break;
                case 3:
                    gtk_label_set_text(PlayerThreeRank, "Normal");
                    break;
                default:
                    gtk_label_set_text(PlayerThreeRank, "Player does not exist");
                    break;
            }

        //CardHand
        strcpy(CardoneTopSub, GetCardTop(PlayerThree, 0));
        strcpy(CardoneBotSub, GetCardBot(PlayerThree, 0));
        strcpy(CardtwoTopSub, GetCardTop(PlayerThree, 1));
        strcpy(CardtwoBotSub, GetCardBot(PlayerThree, 1));
        gtk_image_set_from_file((GtkImage *)PlayerThreeCardoneTop, CardoneTopSub);
        gtk_image_set_from_file((GtkImage *)PThreeCardonebot, CardoneBotSub);
        gtk_image_set_from_file((GtkImage *)PlayerThreeCardtwoTop, CardtwoTopSub);
        gtk_image_set_from_file((GtkImage *)PlayerThreeCardtwoBot, CardtwoBotSub);

        //Status
        strcpy(StatusSub,GetStatus(PlayerThree));
        gtk_label_set_text(PlayerThreeStatus, StatusSub);

        }







//Player 4
    if(PlayerFour.username [0] != '\0'){
        //Copy Username from struc to array
        strcpy(subUser, UsernameText);
        strcat(subUser, PlayerFour.username);
        gtk_label_set_text(PlayerFourUser, subUser);
        //Copy Points to label
        if(PlayerFour.money == -1){
            gtk_label_set_text(PlayerFourPoints, "0");
        }else{
            sprintf(PointsBuffer,"%d", PlayerFour.money);
            gtk_label_set_text(PlayerFourPoints, PointsBuffer);
        }
        //Rank
        switch(PlayerFour.token)
            {
                case 0:
                    gtk_label_set_text(PlayerFourRank, "Dealer");
                    break;
                case 1:
                    gtk_label_set_text(PlayerFourRank, "Small Blind");
                    break;
                case 2:
                    gtk_label_set_text(PlayerFourRank, "Big Blind");
                    break;
                case 3:
                    gtk_label_set_text(PlayerFourRank, "Normal");
                    break;
                default:
                    gtk_label_set_text(PlayerFourRank, "Player does not exist");
                    break;
            }

        //CardHand
        strcpy(CardoneTopSub, GetCardTop(PlayerFour, 0));
        strcpy(CardoneBotSub, GetCardBot(PlayerFour, 0));
        strcpy(CardtwoTopSub, GetCardTop(PlayerFour, 1));
        strcpy(CardtwoBotSub, GetCardBot(PlayerFour, 1));

        gtk_image_set_from_file((GtkImage *)PlayerFourCardoneTop, CardoneTopSub);
        
        //gtk_image_clear((GtkImage *)PlayerFourCardoneBot);
        gtk_image_set_from_file((GtkImage *)PFourCardonebot, CardoneBotSub);
        gtk_image_set_from_file((GtkImage *)PlayerFourCardtwoTop, CardtwoTopSub);
        gtk_image_set_from_file((GtkImage *)PlayerFourCardtwoBot, CardtwoBotSub);
      





        //Status
        strcpy(StatusSub,GetStatus(PlayerFour));
        gtk_label_set_text(PlayerFourStatus, StatusSub);
        

        }

//Player 5
    if(PlayerFive.username [0] != '\0'){
        //Copy Username from struc to array
        strcpy(subUser, UsernameText);
        strcat(subUser, PlayerFive.username);
        gtk_label_set_text(PlayerFiveUser, subUser);
        //Copy Points to label
        if(PlayerFive.money == -1){
            gtk_label_set_text(PlayerFivePoints, "0");
        }else{
            sprintf(PointsBuffer,"%d", PlayerFive.money);
            gtk_label_set_text(PlayerFivePoints, PointsBuffer);
        }
        //Rank
        switch(PlayerFive.token)
            {
                case 0:
                    gtk_label_set_text(PlayerFiveRank, "Dealer");
                    break;
                case 1:
                    gtk_label_set_text(PlayerFiveRank, "Small Blind");
                    break;
                case 2:
                    gtk_label_set_text(PlayerFiveRank, "Big Blind");
                    break;
                case 3:
                    gtk_label_set_text(PlayerFiveRank, "Normal");
                    break;
                default:
                    gtk_label_set_text(PlayerFiveRank, "Player does not exist");
                    break;
            }
        
        //CardHand
        strcpy(CardoneTopSub, GetCardTop(PlayerFive, 0));
        strcpy(CardoneBotSub, GetCardBot(PlayerFive, 0));
        strcpy(CardtwoTopSub, GetCardTop(PlayerFive, 1));
        strcpy(CardtwoBotSub, GetCardBot(PlayerFive, 1));
        gtk_image_set_from_file((GtkImage *)PlayerFiveCardoneTop, CardoneTopSub);
        gtk_image_set_from_file((GtkImage *)PlayerFiveCardoneBot, CardoneBotSub);
        gtk_image_set_from_file((GtkImage *)PlayerFiveCardtwoTop, CardtwoTopSub);
        gtk_image_set_from_file((GtkImage *)PlayerFiveCardtwoBot, CardtwoBotSub);
        //Status
        strcpy(StatusSub,GetStatus(PlayerFive));
        gtk_label_set_text(PlayerFiveStatus, StatusSub); 

        }


//Player 6
    if(PlayerSix.username [0] != '\0'){
        //Copy Username from struc to array
        strcpy(subUser, UsernameText);
        strcat(subUser, PlayerSix.username);
        gtk_label_set_text(PlayerSixUser, subUser);
        //Copy Points to label
        if(PlayerSix.money == -1){
            gtk_label_set_text(PlayerSixPoints, "0");
        }else{
            sprintf(PointsBuffer,"%d", PlayerSix.money);
            gtk_label_set_text(PlayerSixPoints, PointsBuffer);
        }
        //Rank
        switch(PlayerSix.token)
            {
                case 0:
                    gtk_label_set_text(PlayerSixRank, "Dealer");
                    break;
                case 1:
                    gtk_label_set_text(PlayerSixRank, "Small Blind");
                    break;
                case 2:
                    gtk_label_set_text(PlayerSixRank, "Big Blind");
                    break;
                case 3:
                    gtk_label_set_text(PlayerSixRank, "Normal");
                    break;
                default:
                    gtk_label_set_text(PlayerSixRank, "Player does not exist");
                    break;
            }
        //CardHand
        strcpy(CardoneTopSub, GetCardTop(PlayerSix, 0));
        strcpy(CardoneBotSub, GetCardBot(PlayerSix, 0));
        strcpy(CardtwoTopSub, GetCardTop(PlayerSix, 1));
        strcpy(CardtwoBotSub, GetCardBot(PlayerSix, 1));
        gtk_image_set_from_file((GtkImage *)PlayerSixCardoneTop, CardoneTopSub);
        gtk_image_set_from_file((GtkImage *)PlayerSixCardoneBot, CardoneBotSub);
        gtk_image_set_from_file((GtkImage *)PlayerSixCardtwoTop, CardtwoTopSub);
        gtk_image_set_from_file((GtkImage *)PlayerSixCardtwoBot, CardtwoBotSub);
        
        //Status
        strcpy(StatusSub,GetStatus(PlayerSix));
        gtk_label_set_text(PlayerSixStatus, StatusSub);
        }


        //CC
        strcpy(CCTopSub, GetCCTop(CommunityCard[0]));
        strcpy(CCBotSub, GetCCBot(CommunityCard[0]));
        gtk_image_set_from_file((GtkImage *)CCOneTopImage, CCTopSub);
        gtk_image_set_from_file((GtkImage *)CCOneBotImage, CCBotSub);

        strcpy(CCTopSub, GetCCTop(CommunityCard[1]));
        strcpy(CCBotSub, GetCCBot(CommunityCard[1]));
        gtk_image_set_from_file((GtkImage *)CCTwoTopImage, CCTopSub);
        gtk_image_set_from_file((GtkImage *)CCTwoBotSub, CCBotSub);


        strcpy(CCTopSub, GetCCTop(CommunityCard[2]));
        strcpy(CCBotSub, GetCCBot(CommunityCard[2]));
        gtk_image_set_from_file((GtkImage *)CCThreeTopImage, CCTopSub);
        gtk_image_set_from_file((GtkImage *)CCThreeBotImage, CCBotSub);

        strcpy(CCTopSub, GetCCTop(CommunityCard[3]));
        strcpy(CCBotSub, GetCCBot(CommunityCard[3]));
        gtk_image_set_from_file((GtkImage *)CCFourTopSub, CCTopSub);
        gtk_image_set_from_file((GtkImage *)CCFourBotImage, CCBotSub);



        strcpy(CCTopSub, GetCCTop(CommunityCard[4]));
        strcpy(CCBotSub, GetCCBot(CommunityCard[4]));
        gtk_image_set_from_file((GtkImage *)CCFiveTopImage, CCTopSub);
        gtk_image_set_from_file((GtkImage *)CCFiveBotImage, CCBotSub);


}

char* GetCardTop(Player PlayerCardTop, int CardNumber){
    char backTop[] = "./src/assets/cardsicon/backtop.png";
    char Atop[] = "./src/assets/cardsicon/Atop.png";
    char twotop[] = "./src/assets/cardsicon/Twotop.png";
    char threetop[] = "./src/assets/cardsicon/Threetop.png";
    char fourtop[] = "./src/assets/cardsicon/Fourtop.png";
    char fivetop[] = "./src/assets/cardsicon/Fivetop.png";
    char sixtop[] = "./src/assets/cardsicon/Sixtop.png";
    char seventop[] = "./src/assets/cardsicon/Seventop.png";
    char eighttop[] = "./src/assets/cardsicon/Eighttop.png";
    char ninetop[] = "./src/assets/cardsicon/Ninetop.png";
    char tentop[] = "./src/assets/cardsicon/Tentop.png";
    char jacktop[] = "./src/assets/cardsicon/Jacktop.png";
    char queentop[] = "./src/assets/cardsicon/Queentop.png";
    char kingtop[] = "./src/assets/cardsicon/Kingtop.png";
    char* CardTopReturn;
    CardTopReturn = malloc(sizeof(char)*40);

    switch(PlayerCardTop.pocket[CardNumber].num)
    {
        case 2:
            strcpy(CardTopReturn, twotop);
            break;
        case 3:
            strcpy(CardTopReturn, threetop);
            break;
        case 4:
            strcpy(CardTopReturn, fourtop);
            break;
        case 5:
            strcpy(CardTopReturn, fivetop);
            break;
        case 6:
            strcpy(CardTopReturn, sixtop);
            break;
        case 7:
            strcpy(CardTopReturn, seventop);
            break;
        case 8:
            strcpy(CardTopReturn, eighttop);
            break;
        case 9:
            strcpy(CardTopReturn, ninetop);
            break;
        case 10:
            strcpy(CardTopReturn, tentop);
            break;
        case 11:
            strcpy(CardTopReturn, jacktop);
            break;
        case 12:
            strcpy(CardTopReturn, queentop);
            break;
        case 13:
            strcpy(CardTopReturn, kingtop);
            break;
        case 14:
            strcpy(CardTopReturn, Atop);
            break;
        default:
            strcpy(CardTopReturn, backTop);
            break;
    }

    return CardTopReturn;

}


char* GetCardBot(Player PlayerCardBot, int CardNumber){

    char backbot[]      = "./src/assets/cardsicon/backbot.png";
    char diamondbot[]   = "./src/assets/cardsicon/diamondbot.png";
    char cloverbot[]    = "./src/assets/cardsicon/cloverbot.png";
    char heartbot[]     = "./src/assets/cardsicon/heartbot.png";
    char spadebot[]     = "./src/assets/cardsicon/spadebot.png";

    char* CardBotReturn;
    CardBotReturn = malloc(sizeof(char)*40);
    switch(PlayerCardBot.pocket[CardNumber].suit)
    {
        case 0:
            strcpy(CardBotReturn, heartbot);
            break;
        case 1:
            strcpy(CardBotReturn, diamondbot);
            break;
        case 2:
            strcpy(CardBotReturn, spadebot);
            break;
        case 3:
            strcpy(CardBotReturn, cloverbot);
            break;
        case 4:
            strcpy(CardBotReturn, backbot);
            break;
        case 5:
            strcpy(CardBotReturn, backbot);
            break;
        default:
            strcpy(CardBotReturn, backbot);
            break;


    }

return CardBotReturn;

}

char* GetStatus(Player PlayerStatus){
    char* StatusReturn =  malloc(sizeof(char)*10);
    switch(PlayerStatus.moveChoice){
        case 0:
            strcpy(StatusReturn, "NULL");
            break;
        case 1:
            strcpy(StatusReturn, "CHECK");
            break;
        case 2:
            strcpy(StatusReturn, "CALL");
            break;
        case 3:
            strcpy(StatusReturn, "RAISED");
            break;
        case 4:
            strcpy(StatusReturn, "FOLD");
            break;
        case 5:
            strcpy(StatusReturn, "BLIND");
            break;
        default:
            strcpy(StatusReturn, "NULL");
            break;
    }



    return(StatusReturn);

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
    if (listen(ServSocketFD, 6) < 0)	/* max 6 clients in backlog */
    {   FatalError("listening on socket failed");
    }
    return ServSocketFD;
} /* end of MakeServerSocket */

//new
void PrintIntArray(int* ptr, int size) {
    printf("array of ints: ");
    for (int i = 0; i < size; i++) {
        printf("%d ", ptr[i]);
    }
    printf("\n");
}


Move ProcessRequest_V2(int DataSocketFD, int playerNum, int *quasi_shutdown, Table table) {
    int  l, n;
    char RecvBuf[10000];	/* message buffer for receiving a message */
    char SendBuf[10000];	/* message buffer for sending a response */

    Move userInput;
    char currentPlayerNum[3];
    int currentPlayer = 0; //default value to get no warning of uninitialized var
    char playerChoiceNum[4];
    char playerChoiceRaiseAmount[4];

    PrintIntArray(quasi_shutdown, table.totPlayers);

    n = read(DataSocketFD, RecvBuf, sizeof(RecvBuf)-1);
    if (n < 0) 
    {   FatalError("reading from data socket failed");
    }
    RecvBuf[n] = 0;
#ifdef DEBUG
    printf("%s: Received message: %s\n", Program, RecvBuf);
#endif

    printf("Recived message:\n%s\n", RecvBuf);

    if (0 == strncmp(RecvBuf, "REQUEST_MESSAGE", 15))
    {
        //get player number
        findWord_V2(RecvBuf, "REQUEST_MESSAGE", 1, currentPlayerNum);
        currentPlayer = atoi(currentPlayerNum);
    #ifdef DEBUG
        printf("%d", currentPlayer);
    #endif
              
        //if player number does not match 
        //send back appropritate info for given player
        //update size 6 array aswell for specific player
        if (currentPlayer != playerNum && quasi_shutdown[currentPlayer] == 0) {
            //need send gamestate
            SendGameState(table, currentPlayer, SendBuf);
            quasi_shutdown[currentPlayer] = 1;
            printf("current player: %d\n", currentPlayer);
            PrintIntArray(quasi_shutdown, table.totPlayers);
        }
        //if player number does match
        //send back 
        else if (currentPlayer == playerNum && quasi_shutdown[currentPlayer] == 0) {
            SendGameState(table, currentPlayer, SendBuf);
            quasi_shutdown[currentPlayer] = 2;
        }
        else if (currentPlayer == playerNum && quasi_shutdown[currentPlayer] == 2) {
            //send back user input request
            strcpy(SendBuf, "USER_INPUT_REQUEST");
        }
        else {
            //SendGameState(table, currentPlayer, SendBuf);
            /* send nothing */
            strcpy(SendBuf, "");
        }   
    }
    else if (0 == strncmp(RecvBuf, "PLAYER_NUM", 10)) {
        //get player num
        findWord_V2(RecvBuf, "PLAYER_NUM", 1, currentPlayerNum);
        currentPlayer = atoi(currentPlayerNum);

        printf("current player: %d\n", currentPlayer);
            PrintIntArray(quasi_shutdown, table.totPlayers);
        //is a user input response msg
        //parse data
        findWord_V2(RecvBuf, "CHOICE", 1, playerChoiceNum);
        findWord_V2(RecvBuf, "RAISE_AMOUNT", 1, playerChoiceRaiseAmount);
        //update userMove
        userInput.choice = atoi(playerChoiceNum);
        userInput.raiseAmount = atoi(playerChoiceRaiseAmount);
        //send back gamestate for that specific player (sending the old info) (might change for later)
        SendGameState(table, currentPlayer, SendBuf);
        //update size 6 array for player number
        quasi_shutdown[currentPlayer] = 1; //2 to indicate to send gamestate one more time
        printf("current player: %d\n", currentPlayer);
        PrintIntArray(quasi_shutdown, table.totPlayers);
    }

    //strcat(SendBuf, str);

    l = strlen(SendBuf);

    printf("sending message:\n%s\n", SendBuf);

#ifdef DEBUG
    printf("%s: Sending response: %s.\n", Program, SendBuf);
#endif
    printf("about to write to sockett..\n");
    n = write(DataSocketFD, SendBuf, l);
    if (n < 0)
    {   FatalError("writing to data socket failed");
    }
    printf("done writing to socket...\n");

    return userInput;
}


//helper
int checkSentInforToAllPlayer(int *ptr, int totPlayers) {
    int answer;
    answer = 1;
    //assumed size of 6 players
    for (int i = 0; i < totPlayers; i++) {
        if (ptr[i] != 1) {answer = 0;}
    }
    return answer;
}

Move modifiedShutdownLoop(int ServSocketFD, int Timeout, fd_set* ActiveFDs, Table table, int playerNum, 
                            ClientHandler_V2 HandleClient, TimeoutHandler HandleTimeout) {

    int DataSocketFD;	/* socket for a new client */
    socklen_t ClientLen;
    struct sockaddr_in
	ClientAddress;	/* client address we connect with */
    struct timeval TimeVal;
    fd_set ReadFDs;
    int res, i;

    int haveSentInfoToAllPlayers[6] = {0,0,0,0,0,0};
    Move userMove;


    while(!checkSentInforToAllPlayer(haveSentInfoToAllPlayers, table.totPlayers))
    {
	UpdateWindow();	/* update the GUI window */
    ReadFDs = *ActiveFDs;
	TimeVal.tv_sec  = Timeout / 1000000;
	TimeVal.tv_usec = Timeout % 1000000;
	/* block until input arrives on active sockets or until timeout */
	res = select(FD_SETSIZE, &ReadFDs, NULL, NULL, &TimeVal);
	if (res < 0)
	{   FatalError("wait for input or timeout (select) failed");
	}
	if (res == 0)	/* timeout occurred */
	{
        
	    HandleTimeout(table);

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
			FD_SET(DataSocketFD, ActiveFDs);
		    }
		    else
		    {   /* active communication with a client */
#ifdef DEBUG
			printf("%s: Dealing with client on FD%d...\n",
				Program, i);
#endif
			userMove = HandleClient(i, playerNum, haveSentInfoToAllPlayers, table);
            printf("printing after handling client\n");
            PrintCurrentBalance(table);
            int n;
            for(n =0; n<5; n++){
            CommunityCard[n] = table.tableCards[n];
            }
            GlobalPotSize = table.pot;
#ifdef DEBUG
			printf("%s: Closing client connection FD%d.\n",
				Program, i);
#endif
			close(i);
			FD_CLR(i, ActiveFDs);
		    }
		}
	    }
	}
    }
    return userMove;
}

Table BettingRoundV2(Table table, int startingPlayer, MoveGetter shutdownLoop, int ServSocketFD, int Timeout, 
                    fd_set* ActiveFDs, ClientHandler_V2 HandleClient, TimeoutHandler HandleTimeout) {
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

        assert(table.players[currentPlayer].moveChoice != fold);
        if (table.players[currentPlayer].money == 0) {
            //player has already all-in'ed for this hand, so we skip their choice
            PrintCurrentBalance(table);
            currentPlayer = GetNextPlayerWhoHasNotFolded(table, 1, currentPlayer); 
            continue;
        }

        PrintCurrentBalance(table); /* test */

        //userMove = PlayerMoveChoice(table, currentPlayer);
        printf("using ModifiedServerLoop on player: %d\n", currentPlayer);
        userMove = modifiedShutdownLoop(ServSocketFD, Timeout, ActiveFDs, table, currentPlayer, ProcessRequest_V2, HandleTimeout);
        printf("got user move: %d-%d\n", userMove.choice, userMove.raiseAmount);
    
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
    
    printf("end of betting round:");
    PrintCurrentBalance(table);

    return table;
}



int ModifiedServerMainLoop_V3(int ServSocketFD,		/* server socket to wait on */
	ClientHandler_V2 HandleClient,	/* client handler to call */
	TimeoutHandler HandleTimeout,	/* timeout handler to call */
	int Timeout, Table table) 
{
    
    fd_set ActiveFDs;	/* socket file descriptors to select from */
    FD_ZERO(&ActiveFDs);		/* set of active sockets */
    FD_SET(ServSocketFD, &ActiveFDs);	/* server socket is active */
    

    /* start of game */

    //Move userMove;
    //userMove = modifiedShutdownLoop(ServSocketFD, Timeout, &ActiveFDs, table, playerNum, HandleClient);

    int smallBlind, bigBlind;
    int playerOfInterest, winningPlayer;
    int handNum;
    /*
    printf("please enter the total number of players: ");
    scanf("%d", &totalPlayers);
    printf("please enter the starting cash for each player: ");
    scanf("%d", &startingMoney);
    */

    smallBlind = 4;
    bigBlind = 8;
#ifdef DEBUG
    printf("creating table\n");
#endif 
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
        if (handNum == 0) {/* dont move tokens*/}
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
        printf("getting next player with money:");
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
        table = BettingRoundV2(table, playerOfInterest, modifiedShutdownLoop, ServSocketFD, Timeout, &ActiveFDs, ProcessRequest_V2, HandleTimeout);
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
        printf("getting player next player with money:");
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
        table = BettingRoundV2(table, playerOfInterest, modifiedShutdownLoop, ServSocketFD, Timeout, &ActiveFDs, ProcessRequest_V2, HandleTimeout);
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
        printf("Getting next player with money and who has not folded: ");
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
        table = BettingRoundV2(table, playerOfInterest, modifiedShutdownLoop, ServSocketFD, Timeout, &ActiveFDs, ProcessRequest_V2, HandleTimeout);
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
        printf("Getting next player with money and who has not folded: ");
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
        table = BettingRoundV2(table, playerOfInterest, modifiedShutdownLoop, ServSocketFD, Timeout, &ActiveFDs, ProcessRequest_V2, HandleTimeout);
        
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

    return winningPlayer;
} 


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
SetToDefault();



/* intializing table */
printf("Total Players (max is 6): ");
scanf("%d", &totPlayers);
startingMoney = 50;
    
table = CreateTable(totPlayers, startingMoney);




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
    ModifiedServerMainLoop_V3(ServSocketFD, ProcessRequest_V2, //used to be ProcessRequest_V2, but if
			DisplayUpdatedCards, 250000, table); //DisplayUpdateCards Updates GUI
#ifdef DEBUG
    printf("\n%s: Shutting down.\n", Program);
#endif
    printf("exiting server main loop\n");
    close(ServSocketFD);
    DeleteTable(table);
    return 0;
} /* end of main */





void SetToDefault(){

        //Player One Default;
        PlayerOne.token = 4;
        PlayerOne.money = -1;
        PlayerOne.position = 1;
        PlayerOne.moveChoice = 0;
        PlayerOne.pocket[0].suit = 5;
        PlayerOne.pocket[0].num = 16;
        PlayerOne.pocket[1].num = 16;
        PlayerOne.pocket[1].suit = 5;

         //Player Two Default;
        PlayerTwo.token = 4;
        PlayerTwo.money = -1;
        PlayerTwo.position = 2;
        PlayerTwo.moveChoice = 0;
        PlayerTwo.pocket[0].suit = 5;
        PlayerTwo.pocket[0].num = 16;
        PlayerTwo.pocket[1].num = 16;
        PlayerTwo.pocket[1].suit = 5;

        //Player Three Default;
        PlayerThree.token = 4;
        PlayerThree.money = -1;
        PlayerThree.position = 3;
        PlayerThree.moveChoice = 0;
        PlayerThree.pocket[0].suit = 5;
        PlayerThree.pocket[0].num = 16;
        PlayerThree.pocket[1].num = 16;
        PlayerThree.pocket[1].suit = 5;
        //Player Four Default;
        PlayerFour.token = 4;
        PlayerFour.money = -1;
        PlayerFour.position = 4;
        PlayerFour.moveChoice = 0;
        PlayerFour.pocket[0].suit = 5;
        PlayerFour.pocket[0].num = 16;
        PlayerFour.pocket[1].num = 16;
        PlayerFour.pocket[1].suit = 5;

        //Player Five Default;
        PlayerFive.token = 4;
        PlayerFive.money = -1;
        PlayerFive.position = 5;
        PlayerFive.moveChoice = 0;
        PlayerFive.pocket[0].suit = 5;
        PlayerFive.pocket[0].num = 16;
        PlayerFive.pocket[1].num = 16;
        PlayerFive.pocket[1].suit = 5;

        //Player Six Default;
        PlayerSix.token = 4;
        PlayerSix.money = -1;
        PlayerSix.position = 6;
        PlayerSix.moveChoice = 0;
        PlayerSix.pocket[0].suit = 5;
        PlayerSix.pocket[0].num = 16;
        PlayerSix.pocket[1].num = 16;
        PlayerSix.pocket[1].suit = 5;


        //Community Cards
        CommunityCard[0].num = 16;
        CommunityCard[0].suit = 5;
        CommunityCard[1].num = 16;
        CommunityCard[1].suit = 5;
        CommunityCard[2].num = 16;
        CommunityCard[2].suit = 5;
        CommunityCard[3].num = 16;
        CommunityCard[3].suit = 5;
        CommunityCard[4].num = 16;
        CommunityCard[4].suit = 5;






}

void TestUserINFO(){
    
    time_t t;
    srand((unsigned) time(&t));

    if(PlayerOne.username[0] == '\0'){
    strcpy(PlayerOne.username, "TestOne");
    strcpy(PlayerTwo.username, "TestTwo");
    strcpy(PlayerThree.username, "TestThree");
    strcpy(PlayerFour.username, "TestFour");
    strcpy(PlayerFive.username, "TestFive");
    strcpy(PlayerSix.username, "TestSix");
    }



    //Player One Default;
        PlayerOne.token = rand() % 3;
        PlayerOne.money = rand() % 500;
        PlayerOne.position = 1;
        PlayerOne.moveChoice = rand() % 5;
        PlayerOne.pocket[0].suit = rand() % 3;
        PlayerOne.pocket[0].num = (rand() % 13) +1;
        PlayerOne.pocket[1].num = (rand() % 13) +1;
        PlayerOne.pocket[1].suit = rand() % 3;
       
         //Player Two Default;
        PlayerTwo.token = rand() % 3;
        PlayerTwo.money = rand() % 500;
        PlayerTwo.position = 2;
        PlayerTwo.moveChoice = rand() % 5;
        PlayerTwo.pocket[0].suit = rand() % 3;
        PlayerTwo.pocket[0].num = (rand() % 13) +1;
        PlayerTwo.pocket[1].num = (rand() % 13) +1;
        PlayerTwo.pocket[1].suit = rand() % 3;

        //Player Three Default;
        PlayerThree.token = rand() % 3;
        PlayerThree.money = rand() % 500;
        PlayerThree.position = 3;
        PlayerThree.moveChoice = rand() % 5;
        PlayerThree.pocket[0].suit = rand() % 3;
        PlayerThree.pocket[0].num = (rand() % 13) +1;
        PlayerThree.pocket[1].num = (rand() % 13) +1;
        PlayerThree.pocket[1].suit = rand() % 3;
        

        //Player Four Default;
        PlayerFour.token = rand() % 3;
        PlayerFour.money = rand() % 500;
        PlayerFour.position = 4;
        PlayerFour.moveChoice = rand() % 5;
        PlayerFour.pocket[0].suit = rand() % 3;
        PlayerFour.pocket[0].num = (rand() % 13) +1;
        PlayerFour.pocket[1].num = (rand() % 13) +1;
        PlayerFour.pocket[1].suit = rand() % 3;
        
        //Player Five Default;
        PlayerFive.token = rand() % 3;
        PlayerFive.money = rand() % 500;
        PlayerFive.position = 5;
        PlayerFive.moveChoice = rand() % 5;
        PlayerFive.pocket[0].suit = rand() % 3;
        PlayerFive.pocket[0].num = (rand() % 13) +1;
        PlayerFive.pocket[1].num = (rand() % 13) +1;
        PlayerFive.pocket[1].suit = rand() % 3;
        
        //Player Six Default;
        PlayerSix.token = rand() % 3;
        PlayerSix.money = rand() % 500;
        PlayerSix.position = 6;
        PlayerSix.moveChoice = rand() % 5;
        PlayerSix.pocket[0].suit = rand() % 3;
        PlayerSix.pocket[0].num = (rand() % 13) +1;
        PlayerSix.pocket[1].num = (rand() % 13) +1;
        PlayerSix.pocket[1].suit = rand() % 3;
        

        CommunityCard[0].num = (rand() % 13) +1;
        CommunityCard[0].suit = rand() % 3;
        CommunityCard[1].num = (rand() % 13) +1;
        CommunityCard[1].suit = rand() % 3;
        CommunityCard[2].num = (rand() % 13) +1;
        CommunityCard[2].suit = rand() % 3;
        CommunityCard[3].num = (rand() % 13) +1;
        CommunityCard[3].suit = rand() % 3;
        CommunityCard[4].num = (rand() % 13) +1;
        CommunityCard[4].suit = rand() % 3;




}



char* GetCCTop(Card CCTop){
    char backTop[] = "./src/assets/cardsicon/backtop.png";
    char Atop[] = "./src/assets/cardsicon/Atop.png";
    char twotop[] = "./src/assets/cardsicon/Twotop.png";
    char threetop[] = "./src/assets/cardsicon/Threetop.png";
    char fourtop[] = "./src/assets/cardsicon/Fourtop.png";
    char fivetop[] = "./src/assets/cardsicon/Fivetop.png";
    char sixtop[] = "./src/assets/cardsicon/Sixtop.png";
    char seventop[] = "./src/assets/cardsicon/Seventop.png";
    char eighttop[] = "./src/assets/cardsicon/Eighttop.png";
    char ninetop[] = "./src/assets/cardsicon/Ninetop.png";
    char tentop[] = "./src/assets/cardsicon/Tentop.png";
    char jacktop[] = "./src/assets/cardsicon/Jacktop.png";
    char queentop[] = "./src/assets/cardsicon/Queentop.png";
    char kingtop[] = "./src/assets/cardsicon/Kingtop.png";
    char* CCardTopReturn;
    CCardTopReturn = malloc(sizeof(char)*40);

    switch(CCTop.num)
    {
        case 2:
            strcpy(CCardTopReturn, twotop);
            break;
        case 3:
            strcpy(CCardTopReturn, threetop);
            break;
        case 4:
            strcpy(CCardTopReturn, fourtop);
            break;
        case 5:
            strcpy(CCardTopReturn, fivetop);
            break;
        case 6:
            strcpy(CCardTopReturn, sixtop);
            break;
        case 7:
            strcpy(CCardTopReturn, seventop);
            break;
        case 8:
            strcpy(CCardTopReturn, eighttop);
            break;
        case 9:
            strcpy(CCardTopReturn, ninetop);
            break;
        case 10:
            strcpy(CCardTopReturn, tentop);
            break;
        case 11:
            strcpy(CCardTopReturn, jacktop);
            break;
        case 12:
            strcpy(CCardTopReturn, queentop);
            break;
        case 13:
            strcpy(CCardTopReturn, kingtop);
            break;
        case 14:
            strcpy(CCardTopReturn, Atop);
            break;
        default:
            strcpy(CCardTopReturn, backTop);
            break;
    }

    return CCardTopReturn;


}


char* GetCCBot(Card CCBot){
    char backbot[]      = "./src/assets/cardsicon/backbot.png";
    char diamondbot[]   = "./src/assets/cardsicon/diamondbot.png";
    char cloverbot[]    = "./src/assets/cardsicon/cloverbot.png";
    char heartbot[]     = "./src/assets/cardsicon/heartbot.png";
    char spadebot[]     = "./src/assets/cardsicon/spadebot.png";

    char* CardBotReturn;
    CardBotReturn = malloc(sizeof(char)*40);
    switch(CCBot.suit)
    {
        case 0:
            strcpy(CardBotReturn, heartbot);
            break;
        case 1:
            strcpy(CardBotReturn, diamondbot);
            break;
        case 2:
            strcpy(CardBotReturn, spadebot);
            break;
        case 3:
            strcpy(CardBotReturn, cloverbot);
            break;
        case 4:
            strcpy(CardBotReturn, backbot);
            break;
        case 5:
            strcpy(CardBotReturn, backbot);
            break;
        default:
            strcpy(CardBotReturn, backbot);
            break;


    }

return CardBotReturn;

}









/* EOF GTK_ClockServer.c */
