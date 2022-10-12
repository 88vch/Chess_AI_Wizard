#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <assert.h>
#include <math.h>
#include "network.h"
#include "com.h"
//#include <gtk/gtk.h>

//#define DEBUG

/*** type definitions ****************************************************/

typedef void (*TimeoutHandler)(void);

typedef Move (*ClientHandler_V2)(int DataSocketFD, int playerNum, int *quasi_shutdown, Table table);

/*** global variables ****************************************************/

const char *Program	/* program name for descriptive diagnostics */
	= "Goldfish Server";
int Shutdown		/* keep running until Shutdown == 1 */
	= 0;
char ClockBuffer[26]	/* current time in printable format */
	= "";



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
    if (listen(ServSocketFD, 6) < 0)	/* max 5 clients in backlog */
    {   FatalError("listening on socket failed");
    }
    return ServSocketFD;
} /* end of MakeServerSocket */

//custom function
void DisplayUpdatedTable(void);

char str[500];

Move ProcessRequest_V2(int DataSocketFD, int playerNum, int *quasi_shutdown, Table table) {
    int  l, n;
    char RecvBuf[10000];	/* message buffer for receiving a message */
    char SendBuf[10000];	/* message buffer for sending a response */

    Move userInput;
    char currentPlayerNum[3];
    int currentPlayer = 0; //default value to get no warning of uninitialized var
    char playerChoiceNum[4];
    char playerChoiceRaiseAmount[4];
    char* ptr;

    n = read(DataSocketFD, RecvBuf, sizeof(RecvBuf)-1);
    if (n < 0) 
    {   FatalError("reading from data socket failed");
    }
    RecvBuf[n] = 0;
#ifdef DEBUG
    printf("%s: Received message: %s\n", Program, RecvBuf);
#endif

    if (0 == strncmp(RecvBuf, "REQUEST_MESSAGE", 15))
    {
        //get player number
        ptr = findWord(RecvBuf, "REQUEST_MESSAGE", 1);
        strcpy(currentPlayerNum, ptr);
        free(ptr);
        currentPlayer = atoi(currentPlayerNum);
    #ifdef DEBUG
        printf("%s", currentPlayer);
    #endif
              
        //if player number does not match 
        //send back appropritate info for given player
        //update size 6 array aswell for specific player
        if (currentPlayer != playerNum) {
            //need send gamestate
            SendGameState(table, currentPlayer, SendBuf);
            quasi_shutdown[currentPlayer] = 1;
        }
        //if player number does match
        //send back 
        else if (currentPlayer == playerNum) {
            //send back user input request
            strcpy(SendBuf, "USER_INPUT_REQUEST");
        }   
    }
    else if (0 == strncmp(RecvBuf, "PLAYER_NUM", 10)) {
        //optional change: could check is user input is from correct player

        //is a user input response msg
        //parse data
        ptr = findWord(RecvBuf, "CHOICE", 1);
        strcpy(playerChoiceNum, ptr);
        free(ptr);
        ptr = findWord(RecvBuf, "RAISE_AMOUNT", 1);
        strcpy(playerChoiceRaiseAmount, ptr);
        free(ptr);
        //update userMove
        userInput.choice = atoi(playerChoiceNum);
        userInput.raiseAmount = atoi(playerChoiceRaiseAmount);
        //send back gamestate for that specific player (sending the old info) (might change for later)
        SendGameState(table, currentPlayer, SendBuf);
        //update size 6 array for player number
        quasi_shutdown[currentPlayer] = 1;
    }

    //strcat(SendBuf, str);

    l = strlen(SendBuf);


#ifdef DEBUG
    printf("%s: Sending response: %s.\n", Program, SendBuf);
#endif
    n = write(DataSocketFD, SendBuf, l);
    if (n < 0)
    {   FatalError("writing to data socket failed");
    }

    return userInput;
}



//helper
int checkSentInforToAllPlayer(int *ptr) {
    int answer;
    answer = 1;
    //assumed size of 6 players
    for (int i = 0; i < 6; i++) {
        if (ptr[i] == 0) {answer = 0;}
    }
    return answer;
}

Move ModifiedServerMainLoop(int ServSocketFD,		/* server socket to wait on */
	ClientHandler_V2 HandleClient,	/* client handler to call */
	/*TimeoutHandler HandleTimeout,*/	/* timeout handler to call */
	int Timeout, int playerNum, Table table) 
{
    int DataSocketFD;	/* socket for a new client */
    socklen_t ClientLen;
    struct sockaddr_in
	ClientAddress;	/* client address we connect with */
    fd_set ActiveFDs;	/* socket file descriptors to select from */
    fd_set ReadFDs;	/* socket file descriptors ready to read from */
    struct timeval TimeVal;
    int res, i;

    Move userMove;
    int haveSentInfoToAllPlayers[6] = {0,0,0,0,0,0};

    FD_ZERO(&ActiveFDs);		/* set of active sockets */
    FD_SET(ServSocketFD, &ActiveFDs);	/* server socket is active */
    while(!checkSentInforToAllPlayer(haveSentInfoToAllPlayers))
    {
	//UpdateWindow();	/* update the GUI window */
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
	    //HandleTimeout();
        //remove:
        //printf("timeout occured\n");
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
			userMove = HandleClient(i, playerNum, haveSentInfoToAllPlayers, table);
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
    return userMove;
}   

//gets back user input for the given player, also sends gamestate to all other players
Move ModifiedServerLoop(Table table, int playerNum, int argc, int portNum) {
    /* launch loop */
    int ServSocketFD;	/* socket file descriptor for service */
    int PortNo;		/* port number */
    //Program = argv[0];	/* publish program name (for diagnostics) */

    Move userMove;

#ifdef DEBUG
    printf("%s: Starting...\n", Program);
#endif
    PortNo = portNum;
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
   
#ifdef DEBUG
    printf("%s: Providing current time at port %d...\n", Program, PortNo);
#endif
    userMove = ModifiedServerMainLoop(ServSocketFD, ProcessRequest_V2,
			/*DisplayUpdatedTable,*/ 250000, playerNum, table);//DisplayUpdateCards Updates GUI
#ifdef DEBUG
    printf("\n%s: Shutting down.\n", Program);
#endif
    close(ServSocketFD);

    return userMove;


}
