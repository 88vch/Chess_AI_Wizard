/* GTK_ClockClient.c: GUI example interactive TCP/IP client for ClockServer
 * Author: Rainer Doemer, 04/22/22 (based on simple ClockClient.c)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <gtk/gtk.h>
#include <assert.h>
#include "GUIClient.h"


#include "card.h"
#include "deck.h"
#include "player.h"
#include "com.h"
#include "table.h"

/* #define DEBUG */	/* be verbose */
#define BUFFSIZE 2000

/*** global variables ****************************************************/

const char *Program1	/* program name for descriptive diagnostics */
	= NULL;
struct sockaddr_in
	ServerAddress;	/* server address we connect with */

//CLient Variables
    char RecvBuf[BUFFSIZE];
    char *Response;
    char requestMessage[30] = "REQUEST_MESSAGE ";
    char user_num_in_text[4];
    char userInput[200];
    char userInputChoice[2];
    char userRaiseAmount[6];
    int personalNum;
    Move userMove;
    int usec = 1000000; //1 seconds

    Table table1;
    int totPlayers;
    int UserPlayerSet =0;
    int UserPositionSet = 0;
    int c  = 1;

    char RedBox[40]= "./src/assets/table/redbox.png";
    char GreenBox[40] = "./src/assets/table/greenbox.png";


/*** global functions ****************************************************/
  

    GtkWidget *window;
    GtkWidget *window1; 
    GtkWidget *window2;

    GtkWidget *about;
    GtkWidget *table;
    GtkWidget *gametable;
    GtkWidget *settingtable;
    GtkWidget *turncolor;
    GtkWidget *menubtn;
    GtkWidget *halign;
    GtkWidget *halign2;
    GtkWidget *valign;
    GtkWidget *Ptable;
    GtkWidget *Player1, *Player2, *Player3, *Player4, *Player5, *Player6;
    GtkWidget *Foldbtn;
    GtkWidget *Callbtn;
    GtkWidget *Checkbtn;
    GtkWidget *Raisebtn;
    GtkWidget *Settingbtn;
    GtkWidget *startbtn;
    GtkWidget *Sendbtn;
    GtkWidget *Redobtn;
  
    GdkColor color;
    
    GtkWidget *Ruleimage;
    GtkWidget *Ruleimage1;
    GtkWidget *scroll;

    GtkObject *adjustment;
    GtkObject *adjustment2;
    GtkObject *adjustment3;
    //GtkWidget *scale;
    
    GtkWidget *entry3;
    GtkWidget *spin;
    GtkWidget *spin2;
    GtkWidget *spin3;
    //Bubbles
    GtkWidget *bubblesimgone;
    GtkWidget *bubblesimgtwo;
    GtkWidget *bubblesimgthree;
    GtkWidget *bubblesimgfour;
    GtkWidget *bubblesimgfive;
    GtkWidget *bubblesimgsix;
    //Gold
    GtkWidget *GoldBars;

    //Text
    GtkTextIter textiter;
    GtkWidget *text;
    GtkTextBuffer *buffer;
    GtkWidget *entry;
    GtkWidget *scroll1;
    GtkWidget *enterbtn;
    GtkWidget *enter2btn;
    
    //Label
    GtkWidget *playerturn;
    GtkWidget *PotSize;
    GtkWidget *raiseamount;
    GtkWidget *Playernmbr;
    GtkWidget *Playerposi;
    GtkWidget *PlayerOnePot;
    GtkWidget *PlayerTwoPot;
    GtkWidget *PlayerThreePot;
    GtkWidget *PlayerFourPot;
    GtkWidget *PlayerFivePot;
    GtkWidget *PlayerSixPot;

    //Table
    GtkWidget *TableTop;
    GtkWidget *TableBot;
    GtkWidget *TableLeft;
    GtkWidget *TableRight;
    GtkWidget *InnerRightFourTwo;
    GtkWidget *InnerLeftFourTwo;
    GtkWidget *TopLeftTwoTwo;
    GtkWidget *TopRightTwoTwo;
    GtkWidget *TopMiddle;
    GtkWidget *BotLeftTwoTwo;
    GtkWidget *BotRightTwoTwo;
    GtkWidget *BotMiddle;
    GtkWidget *MiniTop;
    GtkWidget *MiniBot;
    GtkWidget *MiniLeft;
    GtkWidget *MiniRight;

    // Player 1
    GtkWidget *PlayerOneCardoneTop;
    GtkWidget *PlayerOneCardoneBot;
    GtkWidget *PlayerOneCardtwoTop;
    GtkWidget *PlayerOneCardtwoBot;
    GtkWidget *PlayerOneAvatar;
    
    // Player 2
    GtkWidget *PlayerTwoCardoneTop;
    GtkWidget *PlayerTwoCardoneBot;
    GtkWidget *PlayerTwoCardtwoTop;
    GtkWidget *PlayerTwoCardtwoBot;
    GtkWidget *PlayerTwoAvatar;
    GtkWidget *P2c1Top;
    GtkWidget *P2c1Bot;
    GtkWidget *P2c2Top;
    GtkWidget *P2c2Bot;
    // Player 3
    GtkWidget *PlayerThreeCardoneTop;
    GtkWidget *PlayerThreeCardoneBot;
    GtkWidget *PlayerThreeCardtwoTop;
    GtkWidget *PlayerThreeCardtwoBot;
    GtkWidget *PlayerThreeAvatar;
    GtkWidget *P3c1Top;
    GtkWidget *P3c1Bot;
    // Player 4
    GtkWidget *PlayerFourCardoneTop;
    GtkWidget *PlayerFourCardoneBot;
    GtkWidget *PlayerFourCardtwoTop;
    GtkWidget *PlayerFourCardtwoBot;
    GtkWidget *PlayerFourAvatar;
    GtkWidget *P4c1Top;
    GtkWidget *P4c1Bot;
    // Player 5
    GtkWidget *PlayerFiveCardoneTop;
    GtkWidget *PlayerFiveCardoneBot;
    GtkWidget *PlayerFiveCardtwoTop;
    GtkWidget *PlayerFiveCardtwoBot;
    GtkWidget *PlayerFiveAvatar;
    // Player 6
    GtkWidget *PlayerSixCardoneTop;
    GtkWidget *PlayerSixCardoneBot;
    GtkWidget *PlayerSixCardtwoTop;
    GtkWidget *PlayerSixCardtwoBot;
    GtkWidget *PlayerSixAvatar;

    // Community Card 1
    GtkWidget *CommunityCardOneTop;
    GtkWidget *CommunityCardOneBot;
    // Community Card 2
    GtkWidget *CommunityCardTwoTop;
    GtkWidget *CommunityCardTwoBot;

    // Community Card 3
    GtkWidget *CommunityCardThreeTop;
    GtkWidget *CommunityCardThreeBot;

    // Community Card 4
    GtkWidget *CommunityCardFourTop;
    GtkWidget *CommunityCardFourBot;

    // Community Card 5
    GtkWidget *CommunityCardFiveTop;
    GtkWidget *CommunityCardFiveBot;

    
    char* GetCardTop(Card CardTop);
    char* GetCardBot(Card CardBot);
    //Startbutton
    void startbtnclicked(GtkWidget *widget, gpointer data);

    // Player
    Player PlayerOne;
    Player PlayerTwo;
    Player PlayerThree;
    Player PlayerFour;
    Player PlayerFive;
    Player PlayerSix;

    // Update 
    void UpdateCards();
    void UpdateCards2();
    // Community Cards
    Card CommunityCard[5];

    // Set to Default
    void SetToDefault();
    
    // User info testing
    void TestUserINFO();
    GtkWidget *Randombtn;
    void Randombtnclicked(GtkWidget *widget, gpointer data)
    {
         UpdateCards(); 
       
    }

    //Btns Declaration
    void Callbtnclicked(GtkWidget *widget, gpointer data);

    void Enterbtnclicked(GtkWidget *widget, gpointer data);

    void Checkbtnclicked(GtkWidget *widget, gpointer data);

    void Foldbtnclicked(GtkWidget *widget, gpointer data);

    void Raisebtnclicked(GtkWidget *widget, gpointer data);

    void Settingbtnclicked(GtkWidget *widget, gpointer data);

    void startbtnclicked(GtkWidget *widget, gpointer data);

    void Sendbtnclicked(GtkWidget *widget, gpointer data);

    void Redobtnclicked(GtkWidget *widget, gpointer data);
    
    void GameStart();
    // Rules button signal 
void PokerRuleManual(GtkWidget *widget, gpointer window)
{
    window1 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window1), "Poker Rules");
    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(window1, 750, 1000);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    gtk_container_add(GTK_CONTAINER(window1), scroll);
    //Ruleimage = gtk_image_new_from_file("./src/assets/rulesicon/Rulesimage.png");
    Ruleimage1 = gtk_image_new_from_file("./src/assets/rulesicon/Rules.png");
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroll), Ruleimage1);
    
  
    gtk_widget_show_all(window1);
    
    
}



// Player number signal
void Playernumber(GtkWidget *widget, gpointer data)
{
    g_print("%d\n", gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin)));
    totPlayers = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin));
    table1 = CreateTable(totPlayers, 400);
    UserPlayerSet = 1;
}

// Player position signal
void PlayerPosition(GtkWidget *widget, gpointer data)
{
    g_print("%d\n", gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin2)));
    personalNum = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin2));
    personalNum--;
    sprintf(user_num_in_text, "%d", personalNum);
    strcat(requestMessage, user_num_in_text);
    UserPositionSet = 1;
}

void Raiseamounttyped(GtkWidget *widget, gpointer data)
{
    g_print("%d\n", gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin3)));
}

// Update
void UpdateCards()
  {
      int GUIPotSize = 9999;
      int UserPotSize = 0;
      char UserPotBuffer[20];
      char CardoneTopSub[40];
      char CardtwoTopSub[40];
      char CardoneBotSub[40];
      char CardtwoBotSub[40];
      char CCTopSub[40];
      char CCBotSub[40];

    //Link Players to Cards






    int n;
if(totPlayers == 1){
PlayerOne = table1.players[0];
strcpy(PlayerOne.username, "PlayerOne");
}
if(totPlayers == 2){
PlayerOne = table1.players[0];
PlayerTwo = table1.players[1];
strcpy(PlayerOne.username, "PlayerOne");
strcpy(PlayerTwo.username, "PlayerTwo");
}
if(totPlayers == 3){
PlayerOne = table1.players[0];
PlayerTwo = table1.players[1];
PlayerThree = table1.players[2];
strcpy(PlayerOne.username, "PlayerOne");
strcpy(PlayerTwo.username, "PlayerTwo");
strcpy(PlayerThree.username, "PlayerThree");
}
if(totPlayers == 4){
PlayerOne = table1.players[0];
PlayerTwo = table1.players[1];
PlayerThree = table1.players[2];
PlayerFour = table1.players[3];
strcpy(PlayerOne.username, "PlayerOne");
strcpy(PlayerTwo.username, "PlayerTwo");
strcpy(PlayerThree.username, "PlayerThree");
strcpy(PlayerFour.username, "PlayerFour");
}
if(totPlayers == 5){
PlayerOne = table1.players[0];
PlayerTwo = table1.players[1];
PlayerThree = table1.players[2];
PlayerFour = table1.players[3];
PlayerFive = table1.players[4];
strcpy(PlayerOne.username, "PlayerOne");
strcpy(PlayerTwo.username, "PlayerTwo");
strcpy(PlayerThree.username, "PlayerThree");
strcpy(PlayerFour.username, "PlayerFour");
strcpy(PlayerFive.username, "PlayerFive");
}
if(totPlayers == 6){
PlayerOne = table1.players[0];
PlayerTwo = table1.players[1];
PlayerThree = table1.players[2];
PlayerFour = table1.players[3];
PlayerFive = table1.players[4];
PlayerSix = table1.players[5];
strcpy(PlayerOne.username, "PlayerOne");
strcpy(PlayerTwo.username, "PlayerTwo");
strcpy(PlayerThree.username, "PlayerThree");
strcpy(PlayerFour.username, "PlayerFour");
strcpy(PlayerFive.username, "PlayerFive");
strcpy(PlayerSix.username, "PlayerSix");
}



    for(n =0; n<5; n++){
       CommunityCard[n] = table1.tableCards[n];
    }
    

//PotSize
char PotBuffer[10];
char BasicPot[] = "Pot Size: ";
GUIPotSize = table1.pot;
sprintf(PotBuffer,"%d", GUIPotSize);
strcat(BasicPot, PotBuffer);
gtk_label_set_text(PotSize, BasicPot);
      
    //Update PlayerOne Cards
    if(PlayerOne.username [0] != '\0'){
        //CardHand
        strcpy(CardoneTopSub, GetCardTop(PlayerOne.pocket[0]));
        strcpy(CardoneBotSub, GetCardBot(PlayerOne.pocket[0]));
        strcpy(CardtwoTopSub, GetCardTop(PlayerOne.pocket[1]));
        strcpy(CardtwoBotSub, GetCardBot(PlayerOne.pocket[1]));
        gtk_image_set_from_file((GtkImage *)PlayerOneCardoneTop, CardoneTopSub);
        gtk_image_set_from_file((GtkImage *)PlayerOneCardoneBot, CardoneBotSub);
        gtk_image_set_from_file((GtkImage *)PlayerOneCardtwoTop, CardtwoTopSub);
        gtk_image_set_from_file((GtkImage *)PlayerOneCardtwoBot, CardtwoBotSub);

        UserPotSize = PlayerOne.money;
        sprintf(UserPotBuffer, "%d", UserPotSize);
        gtk_label_set_text(PlayerOnePot, UserPotBuffer);
        }

    if (PlayerTwo.username [0] != '\0'){
        strcpy(CardoneTopSub, GetCardTop(PlayerTwo.pocket[0]));
        strcpy(CardoneBotSub, GetCardBot(PlayerTwo.pocket[0]));
        strcpy(CardtwoTopSub, GetCardTop(PlayerTwo.pocket[1]));
        strcpy(CardtwoBotSub, GetCardBot(PlayerTwo.pocket[1]));
        
        gtk_image_set_from_file((GtkImage *)PlayerTwoCardoneTop, CardoneTopSub);     
        gtk_image_set_from_file((GtkImage *)PlayerTwoCardoneBot, CardoneBotSub);
        gtk_image_set_from_file((GtkImage *)PlayerTwoCardtwoTop, CardtwoTopSub);
        gtk_image_set_from_file((GtkImage *)PlayerTwoCardtwoBot, CardtwoBotSub);  

        UserPotSize = PlayerTwo.money;
        sprintf(UserPotBuffer, "%d", UserPotSize);
        gtk_label_set_text(PlayerTwoPot, UserPotBuffer);
    }
     

    if(PlayerThree.username [0] != '\0'){
        //CardHand
        strcpy(CardoneTopSub, GetCardTop(PlayerThree.pocket[0]));
        strcpy(CardoneBotSub, GetCardBot(PlayerThree.pocket[0]));
        strcpy(CardtwoTopSub, GetCardTop(PlayerThree.pocket[1]));
        strcpy(CardtwoBotSub, GetCardBot(PlayerThree.pocket[1]));
        gtk_image_set_from_file((GtkImage *)PlayerThreeCardoneTop, CardoneTopSub);
        gtk_image_set_from_file((GtkImage *)PlayerThreeCardoneBot, CardoneBotSub);
        gtk_image_set_from_file((GtkImage *)PlayerThreeCardtwoTop, CardtwoTopSub);
        gtk_image_set_from_file((GtkImage *)PlayerThreeCardtwoBot, CardtwoBotSub);
        UserPotSize = PlayerThree.money;
        sprintf(UserPotBuffer, "%d", UserPotSize);
        gtk_label_set_text(PlayerThreePot, UserPotBuffer);
        }

      if(PlayerFour.username [0] != '\0'){
        //CardHand
        strcpy(CardoneTopSub, GetCardTop(PlayerFour.pocket[0]));
        strcpy(CardoneBotSub, GetCardBot(PlayerFour.pocket[0]));
        strcpy(CardtwoTopSub, GetCardTop(PlayerFour.pocket[1]));
        strcpy(CardtwoBotSub, GetCardBot(PlayerFour.pocket[1]));
        gtk_image_set_from_file((GtkImage *)PlayerFourCardoneTop, CardoneTopSub);
        gtk_image_set_from_file((GtkImage *)PlayerFourCardoneBot, CardoneBotSub);
        gtk_image_set_from_file((GtkImage *)PlayerFourCardtwoTop, CardtwoTopSub);
        gtk_image_set_from_file((GtkImage *)PlayerFourCardtwoBot, CardtwoBotSub);
        UserPotSize = PlayerFour.money;
        sprintf(UserPotBuffer, "%d", UserPotSize);
        gtk_label_set_text(PlayerFourPot, UserPotBuffer);
        }

    if(PlayerFive.username [0] != '\0'){
        //CardHand
        strcpy(CardoneTopSub, GetCardTop(PlayerFive.pocket[0]));
        strcpy(CardoneBotSub, GetCardBot(PlayerFive.pocket[0]));
        strcpy(CardtwoTopSub, GetCardTop(PlayerFive.pocket[1]));
        strcpy(CardtwoBotSub, GetCardBot(PlayerFive.pocket[1]));
        gtk_image_set_from_file((GtkImage *)PlayerFiveCardoneTop, CardoneTopSub);
        gtk_image_set_from_file((GtkImage *)PlayerFiveCardoneBot, CardoneBotSub);
        gtk_image_set_from_file((GtkImage *)PlayerFiveCardtwoTop, CardtwoTopSub);
        gtk_image_set_from_file((GtkImage *)PlayerFiveCardtwoBot, CardtwoBotSub);
        UserPotSize = PlayerFive.money;
        sprintf(UserPotBuffer, "%d", UserPotSize);
        gtk_label_set_text(PlayerFivePot, UserPotBuffer);
        }

    if(PlayerSix.username [0] != '\0'){
        //CardHand
        strcpy(CardoneTopSub, GetCardTop(PlayerSix.pocket[0]));
        strcpy(CardoneBotSub, GetCardBot(PlayerSix.pocket[0]));
        strcpy(CardtwoTopSub, GetCardTop(PlayerSix.pocket[1]));
        strcpy(CardtwoBotSub, GetCardBot(PlayerSix.pocket[1]));
        gtk_image_set_from_file((GtkImage *)PlayerSixCardoneTop, CardoneTopSub);
        gtk_image_set_from_file((GtkImage *)PlayerSixCardoneBot, CardoneBotSub);
        gtk_image_set_from_file((GtkImage *)PlayerSixCardtwoTop, CardtwoTopSub);
        gtk_image_set_from_file((GtkImage *)PlayerSixCardtwoBot, CardtwoBotSub);
        UserPotSize = PlayerSix.money;
        sprintf(UserPotBuffer, "%d", UserPotSize);
        gtk_label_set_text(PlayerSixPot, UserPotBuffer);
        }
    //DO it for all players


    //Update Community Cards 
    //Replace the CCTopImage with whatever u used
        strcpy(CCTopSub, GetCardTop(CommunityCard[0]));
        strcpy(CCBotSub, GetCardBot(CommunityCard[0]));
        gtk_image_set_from_file((GtkImage *)CommunityCardOneTop, CCTopSub);
        gtk_image_set_from_file((GtkImage *)CommunityCardOneBot, CCBotSub);

        strcpy(CCTopSub, GetCardTop(CommunityCard[1]));
        strcpy(CCBotSub, GetCardBot(CommunityCard[1]));
        gtk_image_set_from_file((GtkImage *)CommunityCardTwoTop, CCTopSub);
        gtk_image_set_from_file((GtkImage *)CommunityCardTwoBot, CCBotSub);


        strcpy(CCTopSub, GetCardTop(CommunityCard[2]));
        strcpy(CCBotSub, GetCardBot(CommunityCard[2]));
        gtk_image_set_from_file((GtkImage *)CommunityCardThreeTop, CCTopSub);
        gtk_image_set_from_file((GtkImage *)CommunityCardThreeBot, CCBotSub);

        strcpy(CCTopSub, GetCardTop(CommunityCard[3]));
        strcpy(CCBotSub, GetCardBot(CommunityCard[3]));
        gtk_image_set_from_file((GtkImage *)CommunityCardFourTop, CCTopSub);
        gtk_image_set_from_file((GtkImage *)CommunityCardFourBot, CCBotSub);



        strcpy(CCTopSub, GetCardTop(CommunityCard[4]));
        strcpy(CCBotSub, GetCardBot(CommunityCard[4]));
        gtk_image_set_from_file((GtkImage *)CommunityCardFiveTop, CCTopSub);
        gtk_image_set_from_file((GtkImage *)CommunityCardFiveBot, CCBotSub);
    



  }

// Get cards top
    char* GetCardTop(Card CardTop){
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

    switch(CardTop.num)
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

// get cards bot
char* GetCardBot(Card CardBot){
    char backbot[]      = "./src/assets/cardsicon/backbot.png";
    char diamondbot[]   = "./src/assets/cardsicon/diamondbot.png";
    char cloverbot[]    = "./src/assets/cardsicon/cloverbot.png";
    char heartbot[]     = "./src/assets/cardsicon/heartbot.png";
    char spadebot[]     = "./src/assets/cardsicon/spadebot.png";

    char* CardBotReturn;
    CardBotReturn = malloc(sizeof(char)*40);
    switch(CardBot.suit)
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

void FatalError1(		/* print error diagnostics and abort */
	const char *ErrorMsg)
{
    fputs(Program1, stderr);
    fputs(": ", stderr);
    perror(ErrorMsg);
    fputs(Program1, stderr);
    fputs(": Exiting!\n", stderr);
    exit(20);
} /* end of FatalError1 */

char *Talk2Server(		/* communicate with the server */
	const char *Message,
	char *RecvBuf)
{
    int n;
    int SocketFD;

    SocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (SocketFD < 0)
    {   FatalError1("socket creation failed");
    }
#ifdef DEBUG
    printf("%s: Connecting to the server at port %d...\n",
		Program1, ntohs(ServerAddress.sin_port));
#endif
    if (connect(SocketFD, (struct sockaddr*)&ServerAddress,
		sizeof(struct sockaddr_in)) < 0)
    {   FatalError1("connecting to server failed");
    }
#ifdef DEBUG
    printf("%s: Sending message '%s'...\n", Program1, Message);
#endif
    n = write(SocketFD, Message, strlen(Message));
    if (n < 0)
    {   FatalError1("writing to socket failed");
    }
#ifdef DEBUG
    printf("%s: Waiting for response...\n", Program1);
#endif
    n = read(SocketFD, RecvBuf, BUFFSIZE-1);
    if (n < 0) 
    {   FatalError1("reading from socket failed");
    }
    RecvBuf[n] = 0;
#ifdef DEBUG
    printf("%s: Received response: %s\n", Program1, RecvBuf);
    printf("%s: Closing the connection...\n", Program1);
#endif
    close(SocketFD);
    return(RecvBuf);
} /* end of Talk2Server */


void GetTimeFromServer(		/* ask server for current time, display it */
	GtkWidget *Widget,
	gpointer Data)
{
    GtkLabel *LabelToUpdate;
    char RecvBuf[BUFFSIZE];
    const char *Response;

#ifdef DEBUG
    printf("%s: GetTimeFromServer callback starting...\n", Program);
#endif
    LabelToUpdate = Data;
    assert(LabelToUpdate);
    Response = Talk2Server("TIME", RecvBuf);
    if (0 == strncmp(Response, "OK TIME: ", 9))
    {	/* ok, strip off the protocol header and display the time */
	gtk_label_set_label(LabelToUpdate, Response + 9);
    }
    else
    {	/* unexpected, display entire response */
	gtk_label_set_label(LabelToUpdate, Response);
    }
#ifdef DEBUG
    printf("%s: GetTimeFromServer callback done.\n", Program);
#endif
} /* end of GetTimeFromServer */

void ShutdownServer(		/* ask server to shutdown */
	GtkWidget *Widget,
	gpointer Data)
{
    char RecvBuf[BUFFSIZE];
    const char *Response;

#ifdef DEBUG
    printf("%s: ShutdownServer callback starting...\n", Program);
#endif
    Response = Talk2Server("SHUTDOWN", RecvBuf);
    if (0 == strcmp(Response, "OK SHUTDOWN"))
    {	/* ok, the server shuts down, so should this client */
	gtk_main_quit();
    }
    else
    {	/* unexpected response, ignore it as invalid */
    }
#ifdef DEBUG
    printf("%s: ShutdownServer callback done.\n", Program);
#endif
} /* end of ShutdownServer */





int main(			/* the main function */
	int argc,
	char *argv[])
{
    int PortNo;
    struct hostent *Server;
  


    Program1 = argv[0];
#ifdef DEBUG
    printf("%s: Starting...\n", argv[0]);
#endif
    if (argc < 3)
    {   fprintf(stderr, "Usage: %s hostname port\n", Program1);
	exit(10);
    }
    Server = gethostbyname(argv[1]);
    if (Server == NULL)
    {   fprintf(stderr, "%s: no such host named '%s'\n", Program1, argv[1]);
        exit(10);
    }
    PortNo = atoi(argv[2]);
    if (PortNo <= 2000)
    {   fprintf(stderr, "%s: invalid port number %d, should be >2000\n",
		Program1, PortNo);
        exit(10);
    }
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(PortNo);
    ServerAddress.sin_addr = *(struct in_addr*)Server->h_addr_list[0];

    /* build the GUI */
    /* (adapted from https://en.wikipedia.org/wiki/GTK%2B#Example) */
    
    


    

    /* initialize the GTK libraries */
    gtk_init(&argc, &argv);
   
  


    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE); 
    gtk_window_set_title(GTK_WINDOW(window), "Go Fish Poker");
    gtk_container_set_border_width (GTK_CONTAINER(window), WINDOW_BORDER);
    gtk_widget_set_size_request(window, WINDOW_WIDTH, WINDOW_HEIGHT);\

    gdk_color_parse("#43d8e8", &color);
    gtk_widget_modify_bg(window, GTK_STATE_NORMAL, &color);
    table = gtk_table_new(WINDOW_HEIGHT, WINDOW_WIDTH, TRUE);
   
    gametable = gtk_table_new(13 , 10, TRUE);

   
    turncolor = gtk_image_new_from_file(RedBox);
    gtk_table_attach(GTK_TABLE(table), turncolor, 20, 22, 4, 6, GTK_FILL, GTK_FILL, 0, 0);

    gdk_color_parse("green", &color);
    gtk_widget_modify_bg(turncolor, GTK_STATE_NORMAL, &color);

    // start button
    startbtn = gtk_button_new_with_label("Start");
    gtk_table_attach(GTK_TABLE(table), startbtn, 17, 19, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
    g_signal_connect(startbtn, "clicked", G_CALLBACK(startbtnclicked), NULL);


    // menu button
    menubtn = gtk_button_new_with_label("Rules");
    
    gtk_table_attach(GTK_TABLE(table), menubtn, 21, 23, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
    g_signal_connect(menubtn, "clicked", G_CALLBACK(PokerRuleManual), NULL);
    
     // Setting button
    Settingbtn = gtk_button_new_with_label("Setting");
    gtk_table_attach(GTK_TABLE(table), Settingbtn, 19, 21, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
    g_signal_connect(Settingbtn, "clicked", G_CALLBACK(Settingbtnclicked), NULL);

    Player1 = gtk_label_new("Player 1");
    //gtk_widget_set_size_request(Player1, 50, 50);
    gtk_table_attach(GTK_TABLE(table), Player1, 2, 4, 0, 1, GTK_FILL, GTK_FILL, 0, 0);

    Player2 = gtk_label_new("Player 2");
    //gtk_widget_set_size_request(Player2, 150,50);
    gtk_table_attach(GTK_TABLE(table), Player2, 6, 9, 0, 1, GTK_FILL, GTK_FILL, 0, 0);

    Player3 = gtk_label_new("Player 3");
    //gtk_widget_set_size_request(Player3, 50,50);
    gtk_table_attach(GTK_TABLE(table), Player3, 11, 13, 0, 1, GTK_FILL, GTK_FILL, 0, 0);

    Player4 = gtk_label_new("Player 4");
    //gtk_widget_set_size_request(Player4, 50,50);
    gtk_table_attach(GTK_TABLE(table), Player4, 2, 4, 15, 16, GTK_FILL, GTK_FILL, 0, 0);

    Player5 = gtk_label_new("Player 5");
    //gtk_widget_set_size_request(Player5, 50,50);
    gtk_table_attach(GTK_TABLE(table), Player5, 6, 9, 15, 16, GTK_FILL, GTK_FILL, 0, 0);

    Player6 = gtk_label_new("Player 6");
    //gtk_widget_set_size_request(Player6, 50,50);
    gtk_table_attach(GTK_TABLE(table), Player6, 11, 13, 15, 16, GTK_FILL, GTK_FILL, 0, 0);

   
    // Random button for testing
    Randombtn = gtk_button_new_with_label("Update");
    gtk_widget_set_size_request(Randombtn, 10, 10);
    gtk_table_attach(GTK_TABLE(table), Randombtn, 21, 23, 1, 2, GTK_FILL, GTK_FILL, 0,0);
    g_signal_connect(Randombtn, "clicked", G_CALLBACK(Randombtnclicked), NULL);

    // Fold button
    Foldbtn = gtk_button_new_with_label("Fold");
    //gtk_widget_set_size_request(Foldbtn, 50, 50);
    gtk_table_attach(GTK_TABLE(table), Foldbtn, 15, 17, 15, 16, GTK_FILL, GTK_FILL, 0, 0);
    g_signal_connect(Foldbtn, "clicked", G_CALLBACK(Foldbtnclicked), NULL);

    // Raise button
    Raisebtn = gtk_button_new_with_label("Raise");
    //gtk_widget_set_size_request(Raisebtn, 50, 50);
    gtk_table_attach(GTK_TABLE(table), Raisebtn, 19, 21, 15, 16, GTK_FILL, GTK_FILL, 0, 0);
    adjustment3 = gtk_adjustment_new(1, 1, 9999, 1, 1, 0);
    spin3 = gtk_spin_button_new(GTK_ADJUSTMENT(adjustment3), 1, 0);
    g_signal_connect(spin3, "value-changed", G_CALLBACK(Raiseamounttyped), spin3);
    
    gtk_table_attach(GTK_TABLE(table), spin3, 19, 21, 14, 15, GTK_FILL, GTK_FILL, 0, 0);
    g_signal_connect(Raisebtn, "clicked", G_CALLBACK(Raisebtnclicked), spin3);
    raiseamount = gtk_label_new("Enter amount");
    gtk_table_attach(GTK_TABLE(table), raiseamount, 19,21, 13,14, GTK_FILL, GTK_FILL, 0, 0);


    //Check button
    Checkbtn = gtk_button_new_with_label("Check");
    //gtk_widget_set_size_request(Checkbtn, 50, 50);
    gtk_table_attach(GTK_TABLE(table), Checkbtn, 21, 23, 15, 16, GTK_FILL, GTK_FILL, 0, 0);
    g_signal_connect(Checkbtn, "clicked", G_CALLBACK(Checkbtnclicked), NULL);

    //Call button
    Callbtn = gtk_button_new_with_label("Call");
    gtk_table_attach(GTK_TABLE(table), Callbtn, 17, 19, 15, 16, GTK_FILL, GTK_FILL, 0, 0);
    g_signal_connect(Callbtn, "clicked", G_CALLBACK(Callbtnclicked), NULL);

    // Send button
    Sendbtn = gtk_button_new_with_label("Send");
    gtk_table_attach(GTK_TABLE(table), Sendbtn, 21, 23, 14, 15 , GTK_FILL, GTK_FILL, 0 ,0);
    g_signal_connect(Sendbtn, "clicked", G_CALLBACK(Sendbtnclicked), NULL);


    // Redo button
    Redobtn = gtk_button_new_with_label("Redo");
    gtk_table_attach(GTK_TABLE(table), Redobtn, 21, 23, 13, 14, GTK_FILL, GTK_FILL, 0, 0);
    g_signal_connect(Redobtn, "clicked", G_CALLBACK(Redobtnclicked), NULL);
    
    // Player Money Label
    PlayerOnePot = gtk_label_new("0");
    PlayerTwoPot = gtk_label_new("0");
    PlayerThreePot = gtk_label_new("0");
    PlayerFourPot = gtk_label_new("0");
    PlayerFivePot = gtk_label_new("0");
    PlayerSixPot = gtk_label_new("0");

    gtk_widget_set_size_request(PlayerOnePot, 10 ,10);
    gtk_widget_set_size_request(PlayerTwoPot, 10 ,10);
    gtk_widget_set_size_request(PlayerThreePot, 10 ,10);
    gtk_widget_set_size_request(PlayerFourPot, 10 ,10);
    gtk_widget_set_size_request(PlayerFivePot, 10 ,10);
    gtk_widget_set_size_request(PlayerSixPot, 10 ,10);


    gtk_table_attach(GTK_TABLE(table), PlayerOnePot, 3, 5, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), PlayerTwoPot, 8, 10, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), PlayerThreePot, 13, 15, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), PlayerFourPot, 3, 5, 15, 16, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), PlayerFivePot, 8, 10, 15, 16, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), PlayerSixPot, 13, 15, 15, 16, GTK_FILL, GTK_FILL, 0, 0);

    // Pot size label
    PotSize = gtk_label_new("Pot Size: ");
    gtk_table_attach(GTK_TABLE(table), PotSize, 18, 20, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
    
    ////////////////// DEFAULT PLAYER CARDS///////////////////
    // Player 1
    PlayerOneCardoneTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    PlayerOneCardoneBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    gtk_table_attach(GTK_TABLE(gametable), PlayerOneCardoneTop, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(gametable), PlayerOneCardoneBot, 1, 2, 2, 3, GTK_FILL, GTK_FILL, 0, 0);

    PlayerOneCardtwoTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    PlayerOneCardtwoBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    gtk_table_attach(GTK_TABLE(gametable), PlayerOneCardtwoTop, 2, 3, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(gametable), PlayerOneCardtwoBot, 2, 3, 2, 3, GTK_FILL, GTK_FILL, 0, 0);

    // Player 2
    PlayerTwoCardoneTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    PlayerTwoCardoneBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    gtk_table_attach(GTK_TABLE(gametable), PlayerTwoCardoneTop, 5, 6, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(gametable), PlayerTwoCardoneBot, 5, 6, 2, 3, GTK_FILL, GTK_FILL, 0, 0); 
   
    PlayerTwoCardtwoTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    PlayerTwoCardtwoBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    gtk_table_attach(GTK_TABLE(gametable), PlayerTwoCardtwoTop, 7, 8, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(gametable), PlayerTwoCardtwoBot, 7, 8, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
   
    // Player 3
    PlayerThreeCardoneTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    PlayerThreeCardoneBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    gtk_table_attach(GTK_TABLE(gametable), PlayerThreeCardoneTop, 10, 11, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(gametable), PlayerThreeCardoneBot, 10, 11, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
    
    PlayerThreeCardtwoTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    PlayerThreeCardtwoBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    gtk_table_attach(GTK_TABLE(gametable), PlayerThreeCardtwoTop, 11, 12, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(gametable), PlayerThreeCardtwoBot, 11, 12, 2, 3, GTK_FILL, GTK_FILL, 0, 0);

    //Player 4
    PlayerFourCardoneTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    PlayerFourCardoneBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    gtk_table_attach(GTK_TABLE(gametable), PlayerFourCardoneTop, 1, 2, 7, 8, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(gametable), PlayerFourCardoneBot, 1, 2, 8, 9, GTK_FILL, GTK_FILL, 0, 0);

    PlayerFourCardtwoTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    PlayerFourCardtwoBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    gtk_table_attach(GTK_TABLE(gametable), PlayerFourCardtwoTop, 2, 3, 7, 8, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(gametable), PlayerFourCardtwoBot, 2, 3, 8, 9, GTK_FILL, GTK_FILL, 0, 0);

    //Player 5
    PlayerFiveCardoneTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    PlayerFiveCardoneBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    gtk_table_attach(GTK_TABLE(gametable), PlayerFiveCardoneTop, 5, 6, 7, 8, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(gametable), PlayerFiveCardoneBot, 5, 6, 8, 9, GTK_FILL, GTK_FILL, 0, 0);

    PlayerFiveCardtwoTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    PlayerFiveCardtwoBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    gtk_table_attach(GTK_TABLE(gametable), PlayerFiveCardtwoTop, 7, 8, 7, 8, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(gametable), PlayerFiveCardtwoBot, 7, 8, 8, 9, GTK_FILL, GTK_FILL, 0, 0);

    //Player 6
    PlayerSixCardoneTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    PlayerSixCardoneBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    gtk_table_attach(GTK_TABLE(gametable), PlayerSixCardoneTop, 10, 11, 7, 8, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(gametable), PlayerSixCardoneBot, 10, 11, 8, 9, GTK_FILL, GTK_FILL, 0, 0);

    PlayerSixCardtwoTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    PlayerSixCardtwoBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    gtk_table_attach(GTK_TABLE(gametable), PlayerSixCardtwoTop, 11, 12, 7, 8, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(gametable), PlayerSixCardtwoBot, 11, 12, 8, 9, GTK_FILL, GTK_FILL, 0, 0);


    ///////////////////////// AVATARS /////////////////////////////
    //Player 1
    PlayerOneAvatar =  gtk_image_new_from_file("./src/assets/goldfishavatar/bigbrainfish.png");
    gtk_widget_set_size_request((GtkWidget *)PlayerOneAvatar, 100, 100);
    gtk_table_attach(GTK_TABLE(table), PlayerOneAvatar, 2, 4, 1, 3, GTK_FILL, GTK_FILL, 0, 0);

    //Player 2
    PlayerTwoAvatar =  gtk_image_new_from_file("./src/assets/goldfishavatar/blackgoldfish.png");
    gtk_widget_set_size_request((GtkWidget *)PlayerTwoAvatar, 100, 100);
    gtk_table_attach(GTK_TABLE(table), PlayerTwoAvatar, 6, 9, 1, 3, GTK_FILL, GTK_FILL, 0, 0);

    //Player 3
    PlayerThreeAvatar =  gtk_image_new_from_file("./src/assets/goldfishavatar/goldfishbloop.png");
    gtk_widget_set_size_request((GtkWidget *)PlayerThreeAvatar, 100, 100);
    gtk_table_attach(GTK_TABLE(table), PlayerThreeAvatar,  11, 13, 1, 3, GTK_FILL, GTK_FILL, 0, 0);

    //Player 4
    PlayerFourAvatar =  gtk_image_new_from_file("./src/assets/goldfishavatar/pixelfish.png");
    gtk_widget_set_size_request((GtkWidget *)PlayerFourAvatar, 100, 100);
    gtk_table_attach(GTK_TABLE(table), PlayerFourAvatar,  2, 4, 13, 15, GTK_FILL, GTK_FILL, 0, 0);

    //Player 5
    PlayerFiveAvatar =  gtk_image_new_from_file("./src/assets/goldfishavatar/susfish.png");
    gtk_widget_set_size_request((GtkWidget *)PlayerFiveAvatar, 100, 100);
    gtk_table_attach(GTK_TABLE(table), PlayerFiveAvatar,  6, 9, 13, 15, GTK_FILL, GTK_FILL, 0, 0);

    //Player 6
    PlayerSixAvatar =  gtk_image_new_from_file("./src/assets/goldfishavatar/drawfish.png");
    gtk_widget_set_size_request((GtkWidget *)PlayerSixAvatar, 100, 100);
    gtk_table_attach(GTK_TABLE(table), PlayerSixAvatar,  11, 13, 13, 15, GTK_FILL, GTK_FILL, 0, 0);

    //////////////////// Community Cards//////////////
    CommunityCardOneTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    CommunityCardOneBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    gtk_table_attach(GTK_TABLE(gametable), CommunityCardOneTop, 4, 5, 4, 5, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(gametable), CommunityCardOneBot, 4, 5, 5, 6, GTK_FILL, GTK_FILL, 0, 0);

    CommunityCardTwoTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    CommunityCardTwoBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    gtk_table_attach(GTK_TABLE(gametable), CommunityCardTwoTop, 5, 6, 4, 5, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(gametable), CommunityCardTwoBot, 5, 6, 5, 6, GTK_FILL, GTK_FILL, 0, 0);

    CommunityCardThreeTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    CommunityCardThreeBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    gtk_table_attach(GTK_TABLE(gametable), CommunityCardThreeTop, 6, 7, 4, 5, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(gametable), CommunityCardThreeBot, 6, 7, 5, 6, GTK_FILL, GTK_FILL, 0, 0);

    CommunityCardFourTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    CommunityCardFourBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    gtk_table_attach(GTK_TABLE(gametable), CommunityCardFourTop, 7, 8, 4, 5, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(gametable), CommunityCardFourBot, 7, 8, 5, 6, GTK_FILL, GTK_FILL, 0, 0);

    CommunityCardFiveTop = gtk_image_new_from_file("./src/assets/cardsicon/backtop.png");
    CommunityCardFiveBot = gtk_image_new_from_file("./src/assets/cardsicon/backbot.png");
    gtk_table_attach(GTK_TABLE(gametable), CommunityCardFiveTop, 8, 9, 4, 5, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(gametable), CommunityCardFiveBot, 8, 9, 5, 6, GTK_FILL, GTK_FILL, 0, 0);

    // update




    //Creating Bubbles
    bubblesimgone = gtk_image_new_from_file("./src/assets/goldfishavatar/bubbles.png");
    bubblesimgtwo = gtk_image_new_from_file("./src/assets/goldfishavatar/bubbles.png");
    bubblesimgthree = gtk_image_new_from_file("./src/assets/goldfishavatar/bubbles.png");
    bubblesimgfour = gtk_image_new_from_file("./src/assets/goldfishavatar/bubbles.png");
    bubblesimgfive = gtk_image_new_from_file("./src/assets/goldfishavatar/bubbles.png");
    bubblesimgsix = gtk_image_new_from_file("./src/assets/goldfishavatar/bubbles.png");
    gtk_widget_set_size_request((GtkWidget *)bubblesimgone, 50, 50);
    gtk_table_attach(GTK_TABLE(table), bubblesimgone,  0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), bubblesimgtwo,  5, 6, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), bubblesimgthree,  4, 5, 13, 14, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), bubblesimgfour,  10, 11, 15, 16, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), bubblesimgfive,  14, 15, 6, 7, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), bubblesimgsix,  16, 17, 9, 10, GTK_FILL, GTK_FILL, 0, 0);
    
    //End of bubbles

    //Create Gold
   // GoldBars = gtk_image_new_from_file("./src/assets/goldfishavatar/goldbar.png");
    //gtk_widget_set_size_request((GtkWidget *)GoldBars, 250, 150);
   // gtk_table_attach(GTK_TABLE(table), GoldBars,  17, 22, 1, 4, GTK_FILL, GTK_FILL, 0, 0);


    //Creating table background
    TableTop = gtk_image_new_from_file("./src/assets/table/topleft.png");
    gtk_widget_set_size_request((GtkWidget *)TableTop, 650, 50);
    gtk_table_attach(GTK_TABLE(gametable), TableTop,  0, 13, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
    TableBot = gtk_image_new_from_file("./src/assets/table/botleft.png");
    gtk_widget_set_size_request((GtkWidget *)TableBot, 650, 50);
    gtk_table_attach(GTK_TABLE(gametable), TableBot,  0, 13, 9, 10, GTK_FILL, GTK_FILL, 0, 0);
    TableLeft = gtk_image_new_from_file("./src/assets/table/leftside.png");
    gtk_widget_set_size_request((GtkWidget *)TableLeft, 50, 400);
    gtk_table_attach(GTK_TABLE(gametable), TableLeft,  0, 1, 1, 9, GTK_FILL, GTK_FILL, 0, 0);
    TableRight = gtk_image_new_from_file("./src/assets/table/rightside.png");
    gtk_widget_set_size_request((GtkWidget *)TableRight, 50, 400);
    gtk_table_attach(GTK_TABLE(gametable), TableRight,  12, 13, 1, 9, GTK_FILL, GTK_FILL, 0, 0);

    //Inner
    InnerRightFourTwo = gtk_image_new_from_file("./src/assets/table/2by4square.png");
    gtk_widget_set_size_request((GtkWidget *)InnerRightFourTwo, 100, 200);
    InnerLeftFourTwo = gtk_image_new_from_file("./src/assets/table/2by4square.png");
    gtk_widget_set_size_request((GtkWidget *)InnerLeftFourTwo, 100, 200);
    TopLeftTwoTwo = gtk_image_new_from_file("./src/assets/table/2by2square.png");
    gtk_widget_set_size_request((GtkWidget *)TopLeftTwoTwo, 100, 100);
    TopRightTwoTwo = gtk_image_new_from_file("./src/assets/table/2by2square.png");
    gtk_widget_set_size_request((GtkWidget *)TopRightTwoTwo, 100, 100);
    TopMiddle = gtk_image_new_from_file("./src/assets/table/1by2square.png");
    gtk_widget_set_size_request((GtkWidget *)TopMiddle, 50, 100);
    BotLeftTwoTwo = gtk_image_new_from_file("./src/assets/table/2by2square.png");
    gtk_widget_set_size_request((GtkWidget *)BotLeftTwoTwo, 100, 100);
    BotRightTwoTwo = gtk_image_new_from_file("./src/assets/table/2by2square.png");
    gtk_widget_set_size_request((GtkWidget *)BotRightTwoTwo, 100, 100);
    BotMiddle = gtk_image_new_from_file("./src/assets/table/1by2square.png");
    gtk_widget_set_size_request((GtkWidget *)BotMiddle, 50, 100);
    gtk_table_attach(GTK_TABLE(gametable), InnerRightFourTwo,  10, 12, 3, 7, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(gametable), InnerLeftFourTwo,  1, 3, 3, 7, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(gametable), TopLeftTwoTwo,  3, 5, 1, 3, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(gametable), TopRightTwoTwo,  8, 10, 1, 3, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(gametable), TopMiddle,  6, 7, 1, 3, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(gametable), BotLeftTwoTwo,  3, 5, 7, 9, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(gametable), BotRightTwoTwo,  8, 10, 7, 9, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(gametable), BotMiddle,  6, 7, 7, 9, GTK_FILL, GTK_FILL, 0, 0);
    //MiniBox
    MiniTop = gtk_image_new_from_file("./src/assets/table/5by1square.png");
    MiniBot = gtk_image_new_from_file("./src/assets/table/5by1square.png");
    MiniLeft = gtk_image_new_from_file("./src/assets/table/1by4square.png");
    MiniRight =  gtk_image_new_from_file("./src/assets/table/1by4square.png");
    gtk_widget_set_size_request((GtkWidget *)MiniTop, 250, 50);
    gtk_widget_set_size_request((GtkWidget *)MiniBot, 250, 50);
    gtk_widget_set_size_request((GtkWidget *)MiniLeft, 50, 200);
    gtk_widget_set_size_request((GtkWidget *)MiniRight, 50, 200);
    gtk_table_attach(GTK_TABLE(gametable), MiniTop,  4, 9, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(gametable), MiniBot,  4, 9, 6, 7, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(gametable), MiniLeft,  3, 4, 3, 7, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(gametable), MiniRight,  9, 10, 3, 7, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(table), gametable, 1, 14, 3, 20, GTK_FILL, GTK_FILL, 0,0);
    gtk_container_add(GTK_CONTAINER(window), table);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), G_OBJECT(window));

    /* make sure that everything becomes visible */
    gtk_widget_show_all(window);
    gtk_widget_hide(Callbtn);
    gtk_widget_hide(Raisebtn);
    gtk_widget_hide(Foldbtn);
    gtk_widget_hide(Checkbtn);
    gtk_widget_hide(spin3);
    /* connect request button with function asking server for time */



 

    /* start the main loop, handle all registered events */
    gtk_main();

#ifdef DEBUG
    printf("%s: Exiting...\n", Program);
#endif
    return 0;
} /* end of main */

// 

/* EOF GTK_ClockClient.c */

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

// start button signal
void startbtnclicked(GtkWidget *widget, gpointer data)
{
    if((UserPlayerSet == 1) & (UserPositionSet == 1)){
    gtk_widget_hide(startbtn);
    gtk_widget_hide(Settingbtn);
    GameStart();
    UpdateCards();
    }
    UpdateCards(); 

    gtk_image_set_from_file((GtkImage *)turncolor, GreenBox);
}




void GameStart(){
    
       while(c == 1)
    {
        /* send request for information */
        for (int i = 0; i < BUFFSIZE; i++) {RecvBuf[i] = '\0';}
        printf("talking to server...\n");
        Response = Talk2Server(requestMessage, RecvBuf);
        printf("received reponse:\n%s\n", Response);

        /* parse response */
        if (0 == strncmp(Response, "GAMESTATE", 9)) {
            //update gamestate
            table1 = SetTable(table1, personalNum, Response, 0);
            //display current gamestate
        }
        if (0 == strncmp(Response, "USER_INPUT_REQUEST", 18)) {
            //Show the buttons

            gtk_image_set_from_file((GtkImage *)turncolor, GreenBox);
            gtk_widget_show(Callbtn);
            gtk_widget_show(Checkbtn);
            gtk_widget_show(Foldbtn);
            gtk_widget_show(Raisebtn);
            gtk_widget_show(spin3);
            c = 0; //Ends the loop
            UpdateCards(); 
        }

        printf("starting delay\n");
        /* adding a delay */
        clock_t start_time = clock();
        while (clock() < start_time + usec) {}
        printf("ending delay\n");
    } 


}

// Setting button signal
void Settingbtnclicked(GtkWidget *widget, gpointer data)
{
    window2 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(window2, 250, 250);
    gtk_window_set_title(GTK_WINDOW(window2), "Settings");
    settingtable = gtk_table_new(6, 6, TRUE);
    adjustment = gtk_adjustment_new(1, 1, 6, 1, 1, 0);
    spin = gtk_spin_button_new(GTK_ADJUSTMENT(adjustment), 1, 0);
    gtk_table_attach(GTK_TABLE(settingtable), spin, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0); 
    g_signal_connect(spin, "value-changed", G_CALLBACK(Playernumber), spin);

    adjustment2 = gtk_adjustment_new(0, 0, 6, 1, 1, 0);
    spin2 =gtk_spin_button_new(GTK_ADJUSTMENT(adjustment2), 1, 0);
    gtk_table_attach(GTK_TABLE(settingtable), spin2, 0, 1, 4, 5, GTK_FILL, GTK_FILL, 0, 0); 
    g_signal_connect(spin2, "value-changed", G_CALLBACK(PlayerPosition), spin2);
  
    Playernmbr = gtk_label_new("Enter the amount of people (Press the enter Key)");
    Playerposi = gtk_label_new("Enter your table position (Press the enter Key)");
    gtk_table_attach(GTK_TABLE(settingtable), Playernmbr, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingtable), Playerposi, 0, 1, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
    gtk_container_add(GTK_CONTAINER(window2), settingtable);
    gtk_widget_show_all(window2);
    
}

// Raise button signal
void Raisebtnclicked(GtkWidget *widget, gpointer data)
{
   g_print("%s\n", gtk_entry_get_text(GTK_ENTRY(data)));
   gtk_entry_set_text(GTK_ENTRY(data), "");

        gtk_widget_hide(Checkbtn);
        gtk_widget_hide(Foldbtn);
        gtk_widget_hide(Raisebtn);
        gtk_widget_hide(Callbtn);
        gtk_widget_hide(spin3);    

        gtk_image_set_from_file((GtkImage *)turncolor, RedBox);

        userMove.choice = 3;
        userMove.raiseAmount = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin3));

        printf("\n\n\n\n\n\n\n Raised Amount = %d \n\n\n\n\n\n", userMove.raiseAmount);
       
    


    
}

// Fold button signal
void Foldbtnclicked(GtkWidget *widget, gpointer data)
{
        gtk_widget_hide(Checkbtn);
        gtk_widget_hide(Foldbtn);
        gtk_widget_hide(Raisebtn);
        gtk_widget_hide(Callbtn);
        gtk_widget_hide(spin3);    

        gtk_image_set_from_file((GtkImage *)turncolor, RedBox);

        userMove.choice = 4;
        userMove.raiseAmount = 0;
       

}

// check button signal
void Checkbtnclicked(GtkWidget *widget, gpointer data)
{
    //Make buttons disappear to avoid inputs
    gtk_widget_hide(Checkbtn);
    gtk_widget_hide(Foldbtn);
    gtk_widget_hide(Raisebtn);
    gtk_widget_hide(Callbtn);
    gtk_widget_hide(spin3);
    
    gtk_image_set_from_file((GtkImage *)turncolor, RedBox);


        //1: Check  2: Call  3: Raise  4: Fold
        //These Changes
        userMove.choice = 1;
        userMove.raiseAmount = 0;
       


}

// Call button signal
void Callbtnclicked(GtkWidget *widget, gpointer data)
{
            //Make buttons disappear to avoid inputs
        gtk_widget_hide(Checkbtn);
        gtk_widget_hide(Foldbtn);
        gtk_widget_hide(Raisebtn);
        gtk_widget_hide(Callbtn);
        gtk_widget_hide(spin3);


        gtk_image_set_from_file((GtkImage *)turncolor, RedBox);
    
        userMove.choice = 2;
        userMove.raiseAmount = 0;
        



}

// Entry Signal
void Enterbtnclicked(GtkWidget *widget, gpointer data)
{
    g_print("%s\n", gtk_entry_get_text(GTK_ENTRY(data)));
    gtk_entry_set_text(GTK_ENTRY(data), "");
}


// Send button signal
void Sendbtnclicked(GtkWidget *widget, gpointer data)
{
    //Send to Server
        strcpy(userInputChoice, "");
        strcpy(userRaiseAmount, "");
        strcpy(userInput, "");
        sprintf(userInputChoice, "%d", userMove.choice);
        sprintf(userRaiseAmount, "%d", userMove.raiseAmount);

    

        strcat(userInput, "PLAYER_NUM ");
        strcat(userInput, user_num_in_text);
        strcat(userInput, " CHOICE ");
        strcat(userInput, userInputChoice);
        strcat(userInput, " RAISE_AMOUNT ");
        strcat(userInput, userRaiseAmount);
        strcat(userInput, " ");


        strcpy(RecvBuf, "");
        Response = Talk2Server(userInput, RecvBuf);

        //Loops until it is your turn
        c = 1;
        while(c == 1)
        {
        /* send request for information */
        for (int i = 0; i < BUFFSIZE; i++) {RecvBuf[i] = '\0';}
        printf("talking to server...\n");
        Response = Talk2Server(requestMessage, RecvBuf);
        printf("received reponse:\n%s\n", Response);

        /* parse response */
        if (0 == strncmp(Response, "GAMESTATE", 9)) {
            //update gamestate
            table1 = SetTable(table1, personalNum, Response, 0);
            //display current gamestate
        }
        if (0 == strncmp(Response, "USER_INPUT_REQUEST", 18)) {
            //Show the buttons

            gtk_widget_show(Callbtn);
            gtk_widget_show(Checkbtn);
            gtk_widget_show(Foldbtn);
            gtk_widget_show(Raisebtn);
            gtk_widget_show(spin3);
            
            gtk_image_set_from_file((GtkImage *)turncolor, GreenBox);
            c = 0; //Ends the loop
            UpdateCards(); 
        }

        printf("starting delay\n");
        /* adding a delay */
        clock_t start_time = clock();
        while (clock() < start_time + usec) {}
        printf("ending delay\n");
        } 

}

void Redobtnclicked(GtkWidget *widget, gpointer data)
{
    gtk_widget_show(Callbtn);
    gtk_widget_show(Checkbtn);
    gtk_widget_show(Foldbtn);
    gtk_widget_show(Raisebtn);
    gtk_widget_show(spin3);
}