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
/* #define DEBUG */	/* be verbose */
#define BUFFSIZE 256

/*** global variables ****************************************************/

const char *Program	/* program name for descriptive diagnostics */
	= NULL;
struct sockaddr_in
	ServerAddress;	/* server address we connect with */

/*** global functions ****************************************************/

void PokerRuleManual(GtkWidget *widget, gpointer data)
{
    //GtkWidget *scroll;
    GtkWidget *dialog;
    GdkPixbuf *pixbuf;
    //scroll = gtk_scrolled_window_new(NULL, NULL);
    //gtk_widget_set_size_request(scroll, 10, 10);
    //gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    pixbuf = gdk_pixbuf_new_from_file("./rulesicon/Rules1.jpg", NULL);
    dialog = gtk_about_dialog_new();
    gtk_widget_set_size_request(dialog, 500, 1000);
    gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "Rules");
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), "This is the Poker rules.");
    gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), pixbuf);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}









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

char *Talk2Server(		/* communicate with the server */
	const char *Message,
	char *RecvBuf)
{
    int n;
    int SocketFD;

    SocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (SocketFD < 0)
    {   FatalError("socket creation failed");
    }
#ifdef DEBUG
    printf("%s: Connecting to the server at port %d...\n",
		Program, ntohs(ServerAddress.sin_port));
#endif
    if (connect(SocketFD, (struct sockaddr*)&ServerAddress,
		sizeof(struct sockaddr_in)) < 0)
    {   FatalError("connecting to server failed");
    }
#ifdef DEBUG
    printf("%s: Sending message '%s'...\n", Program, Message);
#endif
    n = write(SocketFD, Message, strlen(Message));
    if (n < 0)
    {   FatalError("writing to socket failed");
    }
#ifdef DEBUG
    printf("%s: Waiting for response...\n", Program);
#endif
    n = read(SocketFD, RecvBuf, BUFFSIZE-1);
    if (n < 0) 
    {   FatalError("reading from socket failed");
    }
    RecvBuf[n] = 0;
#ifdef DEBUG
    printf("%s: Received response: %s\n", Program, RecvBuf);
    printf("%s: Closing the connection...\n", Program);
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
    //GtkWidget *Window;
    //GtkWidget *VBox, *RequestButton, *Frame, *Label;
    //GtkWidget *HButtons, *QuitButton, *ShutdownButton;

    GtkWidget *window;
    //GtkWidget *button;
    //GtkWidget *scale;
    //GtkObject *adjustment;
    GtkWidget *about;
    GtkWidget *table;
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





    Program = argv[0];
#ifdef DEBUG
    printf("%s: Starting...\n", argv[0]);
#endif
    if (argc < 3)
    {   fprintf(stderr, "Usage: %s hostname port\n", Program);
	exit(10);
    }
    Server = gethostbyname(argv[1]);
    if (Server == NULL)
    {   fprintf(stderr, "%s: no such host named '%s'\n", Program, argv[1]);
        exit(10);
    }
    PortNo = atoi(argv[2]);
    if (PortNo <= 2000)
    {   fprintf(stderr, "%s: invalid port number %d, should be >2000\n",
		Program, PortNo);
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
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE); 
    gtk_window_set_title(GTK_WINDOW(window), "Go Fish Poker");
    gtk_container_set_border_width (GTK_CONTAINER(window), WINDOW_BORDER);
    gtk_widget_set_size_request(window, WINDOW_WIDTH, WINDOW_HEIGHT);
    


    table = gtk_table_new(WINDOW_HEIGHT, WINDOW_WIDTH, TRUE);
    //gtk_table_set_col_spacings(GTK_TABLE(table), 1);
    //gtk_table_set_row_spacings(GTK_TABLE(table), 1);

    // menu button
    menubtn = gtk_button_new_with_label("Menu");
    gtk_widget_set_size_request(menubtn, 20, 10);
    gtk_table_attach(GTK_TABLE(table), menubtn, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0,0);
    g_signal_connect(menubtn, "clicked", G_CALLBACK(PokerRuleManual), window);

    Player1 = gtk_label_new("Player 1");
    gtk_widget_set_size_request(Player1, 1, 1);
    gtk_table_attach(GTK_TABLE(table), Player1, 4, 5, 2, 3, GTK_FILL, GTK_FILL, 0, 0);

    Player2 = gtk_label_new("Player 2");
    gtk_widget_set_size_request(Player2, 1,1);
    gtk_table_attach(GTK_TABLE(table), Player2, 12, 13, 2, 3, GTK_FILL, GTK_FILL, 0, 0);

    Player3 = gtk_label_new("Player 3");
    gtk_widget_set_size_request(Player3, 1,1);
    gtk_table_attach(GTK_TABLE(table), Player3, 16, 17, 10, 11, GTK_FILL, GTK_FILL, 0, 0);

    Player4 = gtk_label_new("Player 4");
    gtk_widget_set_size_request(Player4, 1,1);
    gtk_table_attach(GTK_TABLE(table), Player4, 12, 13, 18, 19, GTK_FILL, GTK_FILL, 0, 0);

    Player5 = gtk_label_new("Player 5");
    gtk_widget_set_size_request(Player5, 1,1);
    gtk_table_attach(GTK_TABLE(table), Player5, 4, 5, 18, 19, GTK_FILL, GTK_FILL, 0, 0);

    Player6 = gtk_label_new("Player 6");
    gtk_widget_set_size_request(Player6, 1,1);
    gtk_table_attach(GTK_TABLE(table), Player6, 0, 1, 10, 11, GTK_FILL, GTK_FILL, 0, 0);

    Ptable = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(Ptable), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(Ptable), FALSE);
    gtk_table_attach(GTK_TABLE(table), Ptable, 2, 15, 3, 15, GTK_FILL, GTK_FILL , 0, 0);
    
    // Fold button
    Foldbtn = gtk_button_new_with_label("Fold");
    gtk_widget_set_size_request(Foldbtn, 50, 30);
    gtk_table_attach(GTK_TABLE(table), Foldbtn, 3, 4, 22, 23, GTK_FILL, GTK_FILL, 0, 0);

    // Raise button
    Raisebtn = gtk_button_new_with_label("Raise");
    gtk_widget_set_size_request(Raisebtn, 50, 30);
    gtk_table_attach(GTK_TABLE(table), Raisebtn, 5, 6, 22, 23, GTK_FILL, GTK_FILL, 0, 0);

   //Check button
    Checkbtn = gtk_button_new_with_label("Check");
    gtk_widget_set_size_request(Checkbtn, 50, 30);
    gtk_table_attach(GTK_TABLE(table), Checkbtn, 4, 5, 22, 23, GTK_FILL, GTK_FILL, 0, 0);

    //Call button
    //Checkbtn = gtk_button_new_with_label("Call");
    //gtk_widget_set_size_request(Checkbtn, 50, 30);
    //gtk_table_attach(GTK_TABLE(table), Callbtn, 4, 5, 23, 24, GTK_FILL, GTK_FILL, 0, 0);

    gtk_container_add(GTK_CONTAINER(window), table);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), G_OBJECT(window));


    


 

    /* make sure that everything becomes visible */
    gtk_widget_show_all(window);

    /* connect request button with function asking server for time */
 

    /* start the main loop, handle all registered events */
    gtk_main();

#ifdef DEBUG
    printf("%s: Exiting...\n", Program);
#endif
    return 0;
} /* end of main */

/* EOF GTK_ClockClient.c */
