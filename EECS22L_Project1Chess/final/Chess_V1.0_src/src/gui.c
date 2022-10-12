
#include <gtk/gtk.h>
#include "gui.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include "Board.h"
#include "chess.h"
#include "aiZach.h"


static gboolean chess_board_pressed (GtkWidget *square, GdkEventButton *event, GtkLabel *originboard[8][8]);
/*Global Variables */

GtkWidget *window ;  
GtkWidget *fixed ;
GtkWidget *eventbox;

GtkWidget *chess_icon ; 
GtkWidget *table;//chess

GtkWidget *btn;

GtkWidget *grid;
GtkWidget *fixedbtn;
GtkWidget *bigtable;
GtkLabel *filelabel;
GtkLabel *ranklabel;
GtkLabel *piecelabel; //Used in drawboard
GtkLabel *turnLabel;

GtkWidget *Playerbtn;
GtkWidget *aibtn;
GtkWidget *whitebtn;
GtkWidget *blackbtn;
GtkWidget *Settingbtn;

GtkTextIter textiter;

GtkWidget *text;
GtkWidget *scroll;
GtkTextBuffer *buffer;


//temp
GtkLabel *label;
//translation
char *numbertorankfile ( int position[2]);
char translated[5];
//movecount
int movecount = 0;


//For chssbox click event
int clickedon = 0;
GtkWidget *prevchessbox;
int originposition[2];
int newposition[2];

//Update GUI
int updateGUI(GtkLabel *piecelabel[8][8], Board *changedboard);
int valid;
Board *activeboard; //remember to free board

//Turn Order
Color currentTurn;
Color checkturn;
char turnlabelcolorW[] = "White Turn to move";
char turnlabelcolorB[] = "Black Turn to move";

typedef enum mode{
  human = 0,
  ai = 1
} mode;


typedef struct Player{
  Color color;
  mode Mode;
} Player;

Player PlayerOne;
Player PlayerTwo;

//Ai

Move suggestedMove;


int PieceBoard[8][8];

enum GRID BoardWhiteBlack[8][8];
//temp enum

//being used
void ResetBoard()
{
	int x,y;
  //Build/Color the board
  for(y = 0; y < 8 ; y++)
    {
      if(y % 2 == 0)
        {
          for(x = 0; x < 8 ; x++)
            {
              if(x % 2 == 0)
              {
                BoardWhiteBlack[x][y] = WHITE;
              }

              if(x % 2 == 1)
              {
                BoardWhiteBlack[x][y] = BLACK;
              }

            }
        }



      if(y % 2 == 1)
        {
          for(x = 0; x < 8 ; x++)
            {
              if(x % 2 == 0)
              {
                BoardWhiteBlack[x][y] = BLACK;
              }

              if(x % 2 == 1)
              {
                BoardWhiteBlack[x][y] = WHITE;
              }

            }
        }

    }
}






void CoordToGrid(int c_x, int c_y, int *g_x, int *g_y)
{
	*g_x = (c_x - BOARD_BORDER) / SQUARE_SIZE;
	*g_y = (c_y - BOARD_BORDER) / SQUARE_SIZE;
}

void CreateRank()
  {
    int x;
    char RankChar[20];
    sprintf(RankChar, "%s", "A");
    for (x = 1; x < 9; x++)
    { 
      ranklabel = (GtkLabel *) gtk_label_new(RankChar);
      gtk_widget_set_size_request((GtkWidget *) ranklabel, 25, 25);
      gtk_table_attach(GTK_TABLE(bigtable), (GtkWidget *) ranklabel, x, x+1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
      RankChar[0]++;
    }
  }

void CreateFile()
  {
    char FileChar[20];
    int y;
    sprintf(FileChar, "%s", "8");
    for(y = 1; y < 9; y++)
    {
      filelabel = (GtkLabel *) gtk_label_new(FileChar);
      gtk_widget_set_size_request((GtkWidget *) filelabel, 25,25);
      gtk_table_attach(GTK_TABLE(bigtable), (GtkWidget *) filelabel, 0, 1, y,y+1, GTK_FILL, GTK_FILL, 0,0);
      FileChar[0]--;
    }
  }

/* This is a callback function. The data arguments are ignored
 * in this example. More on callbacks below.
 */


//Close the chess game
static gboolean on_delete_event (GtkWidget *widget, GdkEvent  *event, gpointer data)
{
  /* If you return FALSE in the "delete_event" signal handler,
   * GTK will emit the "destroy" signal. Returning TRUE means
   * you don't want the window to be destroyed.
   *
   * This is useful for popping up 'are you sure you want to quit?'
   * type dialogs.
   */

  g_print ("delete event occurred\n"); 
  gtk_main_quit();
  return TRUE;
}






// Pvp button 
void PVP_button_clicked(GtkWidget *widget, gpointer data)
{
    g_print("PVP button clicked\n");
    gtk_widget_hide(Playerbtn);
    gtk_widget_hide(aibtn);
 


    PlayerOne.color = white;
    PlayerOne.Mode  = human;
    PlayerTwo.color = black;
    PlayerTwo.Mode = human;
    
}

// ai button
void ai_button_clicked(GtkWidget *widget, gpointer data)
{
    g_print("ai button clicked\n");
    gtk_widget_hide(aibtn);
    gtk_widget_hide(Playerbtn);
    gtk_widget_show(blackbtn);
    gtk_widget_show(whitebtn);
    
}

// white button
void white_button_clicked(GtkWidget *widget, gpointer data)
{
    g_print("white button clicked\n");
    gtk_widget_hide(blackbtn);
    gtk_widget_hide(whitebtn);

    PlayerOne.color = white;
    PlayerOne.Mode  = human;
    PlayerTwo.color = black;
    PlayerTwo.Mode = ai;


}

// black button
void black_button_clicked(GtkWidget *widget, gpointer data)
{
    g_print("black button clicked\n");
    gtk_widget_hide(blackbtn);
    gtk_widget_hide(whitebtn);

    PlayerOne.color = white;
    PlayerOne.Mode  = ai;
    PlayerTwo.color = black;
    PlayerTwo.Mode = human;


}

// setting button
void SETTING_button_clicked(GtkWidget *widget, gpointer data)
{
  g_print("Setting button clicked\n");
  gtk_widget_show(Playerbtn);
  gtk_widget_show(aibtn);
}

int main (int   argc, char *argv[])
{
  //char str[MAX_MSGLEN]; 
  //InitBoard(PieceBoard);
  ResetBoard();
  /*create a new window */
  gtk_init(&argc, &argv) ;
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL) ;
  gtk_widget_set_size_request(window, WINDOW_WIDTH, WINDOW_HEIGHT) ; 
  gtk_container_set_border_width (GTK_CONTAINER(window), WINDOW_BORDER) ; 
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER) ; 
  gtk_window_set_title(GTK_WINDOW(window), "Checkers but Chess") ; 
  gtk_window_set_resizable(GTK_WINDOW(window), FALSE) ; 

  bigtable  = gtk_table_new(WINDOW_HEIGHT, WINDOW_WIDTH, TRUE);
  /*create a table and draw the board*/
  table = gtk_table_new (8, 8, TRUE) ; 
  gtk_widget_set_size_request (table, BOARD_WIDTH, BOARD_HEIGHT);


    //Default PVP
    PlayerOne.color = white;
    PlayerOne.Mode  = human;
    PlayerTwo.color = black;
    PlayerTwo.Mode = human;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Initializing the board
  //DrawBoard();
  GtkLabel *originboard[8][8];
  GtkWidget *chessbox;
  GdkColor bgcolor;
//Initialize the board struct

  
activeboard = StartingBoard();
currentTurn = white; 

  char *asciipieces[64] = {
        "♜", "♞", "♝","♛","⛃","♝","♞","♜",
        "♟", "♟", "♟","♟","♟","♟","♟","♟",
        "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "",
        "♙", "♙", "♙","♙","♙","♙","♙","♙",
        "♖", "♘", "♗","♕","⛁","♗","♘","♖"
  };




  int counterpiecearray = 0;

	int i, j;
    for(j = 0; j< 8; j ++)
	{
		for(i = 0; i < 8; i ++)
		{
      piecelabel = (GtkLabel *) gtk_label_new(asciipieces[counterpiecearray]);
      gtk_widget_set_size_request((GtkWidget *) piecelabel, 50, 50); //size of ascii to be 35,35 to fit into 50,50 square
      originboard[i][j] = piecelabel;
      chessbox = gtk_event_box_new();

			switch(BoardWhiteBlack[i][j])
			{
				case BLACK:
          gdk_color_parse("#37b04d", &bgcolor);
          gtk_widget_modify_bg((GtkWidget *)chessbox, GTK_STATE_NORMAL, &bgcolor);


					break;
				case WHITE:
          gdk_color_parse("#94e1a2", &bgcolor);
          gtk_widget_modify_bg((GtkWidget *)chessbox, GTK_STATE_NORMAL, &bgcolor);
					break;
				default:
					break;
				
			}

      gtk_event_box_set_above_child(GTK_EVENT_BOX(chessbox), FALSE);
      //Put the chess unicode into the chessbox
      gtk_container_add(GTK_CONTAINER(chessbox), (GtkWidget *) piecelabel);
      //Add the chessbox inot the table
			gtk_table_attach(GTK_TABLE(table), (GtkWidget *) chessbox, i, i + 1, j, j + 1, GTK_FILL, GTK_FILL, 0, 0) ;


      //Connects signal when the individual squares are pressed
      g_signal_connect(G_OBJECT(chessbox), "button_press_event", G_CALLBACK (chess_board_pressed), (gpointer) originboard);

      gtk_widget_set_events(chessbox, GDK_BUTTON_PRESS_MASK);



      counterpiecearray++;//Increment to access the next char
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //Creates the A-H , 1-8
  CreateRank();
  CreateFile();
  gtk_table_attach(GTK_TABLE(bigtable), table, 1, 9, 1, 9, GTK_FILL, GTK_FILL,0,0);

  //Turn Label
  
  turnLabel = (GtkLabel *) gtk_label_new(turnlabelcolorW);
  gtk_table_attach(GTK_TABLE(bigtable), (GtkWidget *) turnLabel, 9, 14, 4, 5, GTK_FILL, GTK_FILL, 0,0);
  

  // pvp button
  Playerbtn = gtk_button_new_with_label("PvP");
  gtk_widget_set_size_request(Playerbtn, OPTION_BTN_WIDTH, OPTION_BTN_HEIGHT);
  gtk_table_attach(GTK_TABLE(bigtable), Playerbtn, 10, 12, 1, 2, GTK_FILL,GTK_FILL, 0, 0);
  g_signal_connect(Playerbtn, "clicked", G_CALLBACK(PVP_button_clicked), NULL);

  // ai button
  aibtn = gtk_button_new_with_label("AI");
  gtk_widget_set_size_request(aibtn, OPTION_BTN_WIDTH, OPTION_BTN_HEIGHT);
  gtk_table_attach(GTK_TABLE(bigtable), aibtn, 12, 14, 1, 2, GTK_FILL,GTK_FILL, 0, 0);
  g_signal_connect(aibtn, "clicked", G_CALLBACK(ai_button_clicked), NULL);
  
  // white button
  whitebtn = gtk_button_new_with_label("White");
  gtk_widget_set_size_request(whitebtn, OPTION_BTN_WIDTH, OPTION_BTN_HEIGHT);
  gtk_table_attach(GTK_TABLE(bigtable), whitebtn, 12, 14, 2, 3, GTK_FILL,GTK_FILL, 0, 0);
  gtk_widget_hide(whitebtn);
  g_signal_connect(whitebtn, "clicked", G_CALLBACK(white_button_clicked), NULL);

  // black button
  blackbtn = gtk_button_new_with_label("Black");
  gtk_widget_set_size_request(blackbtn, OPTION_BTN_WIDTH, OPTION_BTN_HEIGHT);
  gtk_table_attach(GTK_TABLE(bigtable), blackbtn, 10, 12, 2, 3, GTK_FILL,GTK_FILL, 0, 0);
  gtk_widget_hide(blackbtn);
  g_signal_connect(blackbtn, "clicked", G_CALLBACK(black_button_clicked), NULL);
 
  
  // settings button
  Settingbtn = gtk_button_new_with_label("Setting");
  gtk_widget_set_size_request(Settingbtn, SETTING_BTN_WIDTH, SETTING_BTN_HEIGHT);
  gtk_table_attach(GTK_TABLE(bigtable), Settingbtn, 10, 14, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
  g_signal_connect(Settingbtn, "clicked", G_CALLBACK(SETTING_button_clicked), NULL);

// text
  text = gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(text), FALSE);
  gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW(text), FALSE);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW(text), GTK_WRAP_WORD);
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text));
  gtk_text_view_set_left_margin(GTK_TEXT_VIEW(text), 10);
  gtk_text_view_set_right_margin(GTK_TEXT_VIEW(text), 10);

  scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_set_size_request(scroll, 10, 10);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
  gtk_container_add(GTK_CONTAINER (scroll), text);
  gtk_table_attach(GTK_TABLE(bigtable), scroll, 9, 14, 5, 9, GTK_FILL, GTK_FILL, 0, 0);

  /*register event handlers*/
  g_signal_connect(window, "delete_event", G_CALLBACK(on_delete_event), NULL) ; 
  gtk_widget_set_events((GtkWidget *) table, GDK_BUTTON_PRESS_MASK) ; 

  //g_signal_connect((GtkWidget *) table, "button_press_event", G_CALLBACK(area_click), NULL) ; 
  
  


  

 // fixed = gtk_fixed_new() ; 
  //gtk_fixed_put(GTK_FIXED(fixed), bigtable, 0, 0) ; 
  gtk_container_add(GTK_CONTAINER(window), bigtable) ; 


  /*show the window*/
  gtk_widget_show_all(window) ; 
  gtk_main() ; 

  return 0 ;
}







//When the chess squares are clicked upon
static gboolean chess_board_pressed (GtkWidget *square, GdkEventButton *event, GtkLabel *originboard[8][8])
{
    GdkColor clickcolor;
    unsigned int left,top;
    char *chesslango;
    g_print("Chess Board clicked Test\n");
    if(event->type == GDK_BUTTON_PRESS)
      {
        if(!clickedon)
        {
          gdk_color_parse("#F3F34D", &clickcolor);
          gtk_widget_modify_bg((GtkWidget *) square, GTK_STATE_NORMAL, &clickcolor);

          gtk_container_child_get(GTK_CONTAINER(table), square, "left-attach", &left, "top-attach",&top, NULL);
          originposition[0] = abs(left);
          originposition[1] = abs(top-7);

          g_print("%d %d square was clicked\n", originposition[0], originposition[1]);

          prevchessbox = square;

          //testing purposes
          chesslango = numbertorankfile(originposition);
          //g_print("area pressed %c%c\n", chesslango[0],chesslango[1]);
          g_print("area pressed %c%c\n", *chesslango, *(chesslango + 1));
          clickedon = 1;

        }else {
          
          gtk_container_child_get(GTK_CONTAINER(table), square, "left-attach", &left, "top-attach",&top, NULL);
          newposition[0] = abs(left);
          newposition[1] = abs(top-7);

          //newposition[left,top]

          //change back the previous box color to original
          if((originposition[0]+originposition[1])&1)
            {
              gdk_color_parse("#94e1a2", &clickcolor);
              gtk_widget_modify_bg((GtkWidget *) prevchessbox, GTK_STATE_NORMAL, &clickcolor);

            }else{
              gdk_color_parse("#37b04d", &clickcolor);
              gtk_widget_modify_bg((GtkWidget *) prevchessbox, GTK_STATE_NORMAL, &clickcolor);
            }
          /////////////////////////////////////////////////

          //tessting purposes
          chesslango = numbertorankfile(newposition);
          //g_print("area pressed %c%c\n", chesslango[0],chesslango[1]);
          g_print("area pressed %c%c\n", *chesslango, *(chesslango + 1));
          //numbertorankfile(originposition);
          
          
          if(currentTurn == activeboard->grid[originposition[0]][originposition[1]].color)
          {//start of currentturn

            if(((PlayerOne.color == currentTurn) && (PlayerOne.Mode == human)) || ((PlayerTwo.color == currentTurn) && (PlayerTwo.Mode == human)) )
            {
              valid = MovePieces(originposition, newposition, activeboard);
            
              g_print("Passed valid\n");
            }else{
              //ai function
                printf("got to ai\n");
                suggestedMove = createBestMove(activeboard, 5, currentTurn);
                originposition[0] = suggestedMove.iPos.file;
                originposition[1] = abs(suggestedMove.iPos.rank);
                printf("\nPrinting initial pos\n");
                PrintFileAndRank(suggestedMove.iPos);
                printf("\n-------------------------\n");
                PrintFileAndRank(suggestedMove.fPos);
                printf("\nPrinted Final Position\n");
                newposition[0] = suggestedMove.fPos.file;
                newposition[1] = abs(suggestedMove.fPos.rank);

                valid = SmartMovePiece(suggestedMove.iPos, suggestedMove.fPos, activeboard, empty);
                //valid = MovePieces(originposition, newposition, activeboard);
                //g_print("Passed valid\n");
            }

          if(((PlayerOne.color == currentTurn) && (PlayerOne.Mode == human)) || ((PlayerTwo.color == currentTurn) && (PlayerTwo.Mode == human)) )
            {
            if(!valid)
              {
                //Update log
                buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (text));
                gtk_text_buffer_get_end_iter(buffer, &textiter);
                sprintf(chesslango, "%d", ++movecount);
                gtk_text_buffer_insert(buffer, &textiter, " ", -1);
                gtk_text_buffer_insert(buffer, &textiter, chesslango, -1);
                gtk_text_buffer_insert(buffer, &textiter, ".", -1);
                //Update Board
                updateGUI(originboard, activeboard);


              
                if(currentTurn == black){
                  //printf("chasdaaaaaaaaaaaaaaaaaa\n");
                  //turnLabel = (GtkLabel *) gtk_label_new(turnlabelcolorW);
                  //gtk_table_attach(GTK_TABLE(bigtable), (GtkWidget *) turnLabel, 9, 11, 4, 5, GTK_FILL, GTK_FILL, 0,0);
                  //gtk_widget_hide((GtkWidget *) turnLabel);

                  gtk_label_set_text(turnLabel, "White Player Turn");
                }else{
                  //printf("asdddddddddddddddddddddd");
                  gtk_label_set_text(turnLabel, "Black Player Turn");
                  //turnLabel = (GtkLabel *) gtk_label_new(turnlabelcolorB);
                  //gtk_table_attach(GTK_TABLE(bigtable), (GtkWidget *) turnLabel, 12, 14, 4, 5, GTK_FILL, GTK_FILL, 0,0);
                  //gtk_widget_hide((GtkWidget *) turnLabel);
                }
              


                // Update the game info
                chesslango = numbertorankfile(originposition);
                sprintf(translated, "%c%c to ", *chesslango, *(chesslango + 1));
                gtk_text_buffer_insert(buffer, &textiter, translated , -1);


                chesslango = numbertorankfile(newposition);
                sprintf(translated, "%c%c", *chesslango, *(chesslango + 1));
                gtk_text_buffer_insert(buffer, &textiter, translated , -1);


                if      (currentTurn == white) { checkturn = black; }
                else if (currentTurn == black) { checkturn = white; }

                


                int checkmatelog = CheckForCheckMate(LocateKing(checkturn, activeboard), activeboard);
                //printf("checklog is %d \n", checkmatelog);
                if (checkmatelog == 1)
                  {
                    
                    if(checkturn == black)
                    {
                      //gtk_text_buffer_insert(buffer, &textiter, " Black in CheckMate, White Wins", -1);
                      gtk_label_set_text(turnLabel, " Black in CheckMate, White Wins");
                    }else{
                      //gtk_text_buffer_insert(buffer, &textiter, " White in Check, Black Wins", -1);
                      gtk_label_set_text(turnLabel, " White in CheckMate, Black Wins ");
                    }

                  }else{
                      int checklog = CheckForCheck(LocateKing(checkturn, activeboard), activeboard);
                      //printf("checklog is %d \n", checklog);
                      if (checklog == 1)
                      {
                    
                        if(checkturn == black)
                          {
                            gtk_text_buffer_insert(buffer, &textiter, " Black in Check", -1);
                          }else{
                            gtk_text_buffer_insert(buffer, &textiter, " White in Check", -1);
                          }

                      }



                  }

              }
            }else{
              //ai print log
              if(!valid)
              {
                //Update log
                buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (text));
                gtk_text_buffer_get_end_iter(buffer, &textiter);
                sprintf(chesslango, "%d", ++movecount);
                gtk_text_buffer_insert(buffer, &textiter, " ", -1);
                gtk_text_buffer_insert(buffer, &textiter, chesslango, -1);
                gtk_text_buffer_insert(buffer, &textiter, ".", -1);
                //Update Board
                updateGUI(originboard, activeboard);


              
                if(currentTurn == black){
                  
                  gtk_label_set_text(turnLabel, "White Player Turn");
                }else{
                  gtk_label_set_text(turnLabel, "Black Player Turn");
                }
              


                // Update the game info
                chesslango = numbertorankfile(originposition);
                sprintf(translated, "%c%c to ", *chesslango, *(chesslango + 1));
                gtk_text_buffer_insert(buffer, &textiter, translated , -1);


                chesslango = numbertorankfile(newposition);
                sprintf(translated, "%c%c", *chesslango, *(chesslango + 1));
                gtk_text_buffer_insert(buffer, &textiter, translated , -1);


                if      (currentTurn == white) { checkturn = black; }
                else if (currentTurn == black) { checkturn = white; }

                


                int checkmatelog = CheckForCheckMate(LocateKing(checkturn, activeboard), activeboard);
                //printf("checklog is %d \n", checkmatelog);
                if (checkmatelog == 1)
                  {
                    
                    if(checkturn == black)
                    {
                      //gtk_text_buffer_insert(buffer, &textiter, " Black in CheckMate, White Wins", -1);
                      gtk_label_set_text(turnLabel, " Black in CheckMate, White Wins");
                    }else{
                      //gtk_text_buffer_insert(buffer, &textiter, " White in Check, Black Wins", -1);
                      gtk_label_set_text(turnLabel, " White in CheckMate, Black Wins ");
                    }

                  }else{
                      int checklog = CheckForCheck(LocateKing(checkturn, activeboard), activeboard);
                      //printf("checklog is %d \n", checklog);
                      if (checklog == 1)
                      {
                    
                        if(checkturn == black)
                          {
                            gtk_text_buffer_insert(buffer, &textiter, " Black in Check", -1);
                          }else{
                            gtk_text_buffer_insert(buffer, &textiter, " White in Check", -1);
                          }

                      }



                  }

              }


            }

            if(valid == 0){
              if      (currentTurn == white) { currentTurn = black; }
              else if (currentTurn == black) { currentTurn = white; }
            }
          }//end of currentturn





            clickedon = 0;//reset variable
        }



      }




  return FALSE;
}





char *numbertorankfile( int position[2])
  {

    // x = rank  y = file
    int rankpos, filepos;
    char *chessmove = malloc(sizeof(2));
    rankpos = position[0];
    filepos = position[1];
    //g_print("%d  %d testing \n", rankpos, filepos);
    //Switch x to rank
    switch(rankpos)
      {
        case 0 :
          chessmove[0] = 'A';
          break;
        case 1 :
          chessmove[0] = 'B';
          break;
        case 2 :
          chessmove[0] = 'C';
          break;
        case 3 :
          chessmove[0] = 'D';
          break;
        case 4 :
          chessmove[0] = 'E';
          break;
        case 5 :
          chessmove[0] = 'F';
          break;
        case 6 :
          chessmove[0] = 'G';
          break;
        case 7 :
          chessmove[0] = 'H';
          break;
        default :
          break;
      }

    //switch y to file
      switch(filepos)
      {
        case 0 :
          chessmove[1] = '1';
          break;
        case 1 :
          chessmove[1] = '2';
          break;
        case 2 :
          chessmove[1] = '3';
          break;
        case 3 :
          chessmove[1] = '4';
          break;
        case 4 :
          chessmove[1] = '5';
          break;
        case 5 :
          chessmove[1] = '6';
          break;
        case 6 :
          chessmove[1] = '7';
          break;
        case 7 :
          chessmove[1] = '8';
          break;
        default :
          break;
      }


    
    return chessmove;
  }




int updateGUI(GtkLabel *piecelabel[8][8], Board *changedboard)
{
  int rank, file;
  char *replacement;

  //0 0 A 1  0 0 A 8
  //7 7 h 8 7 7 h 1 

  for (rank = 0; rank <= eight; rank++) {
    for (file = a; file <= h; file++) {
      
      switch(changedboard->grid[file][rank].color)
      { 
        //If black
        case black:
          //assign black unicode pieces
          switch(changedboard->grid[file][rank].type)
            {
              case empty:
                replacement = "";
                break;
              case pawn:
                replacement = "♟";
                break;
              case king:
                replacement = "⛃";
                break;
              case queen:
                replacement = "♛";
                break;
              case bishop:
                replacement = "♝";
                break;
              case knight:
                replacement = "♞";
                break;
              case rook:
                replacement = "♜";
                break;
            }
          break;

        case white:
          switch(changedboard->grid[file][rank].type)
            {
              case empty:
                replacement = "";
                break;
              case pawn:
                replacement = "♙";
                break;
              case king:
                replacement = "⛁";
                break;
              case queen:
                replacement = "♕";
                break;
              case bishop:
                replacement = "♗";
                break;
              case knight:
                replacement = "♘";
                break;
              case rook:
                replacement = "♖";
                break;
            }
        
          break;

        case no_color:
          replacement = "";
          break;
        default:
          replacement = "";
          break;

      }


      gtk_label_set_text(piecelabel[file][abs(rank-7)], replacement);

    }
  }
  return 0;
}