




GTKINC	= `PKG_CONFIG_PATH=/usr/share/pkgconfig pkg-config --cflags gtk+-2.0`
GTKLIBS	= `PKG_CONFIG_PATH=/usr/share/pkgconfig pkg-config --libs gtk+-2.0`

INC	= $(GTKINC)
LIBS	= $(GTKLIBS)

OPTS	= -g

PROJECT	= Checkers_But_Chess

CC = gcc
CFLAGS = -Wall -std=c11
OBJDIR = obj
SRCDIR = src
BINDIR = bin
SRCS    = $(wildcard $(SRCDIR)/*.c)
OBJS   = $(OBJDIR)/gui.o $(OBJDIR)/Board.o $(OBJDIR)/Piece.o $(OBJDIR)/Chess.o $(OBJDIR)/ai.o
OBJSUB = gui.o Board.o Piece.o Chess.o ai.o 
CHESSOBJ = $(OBJDIR)/gui.o $(OBJDIR)/Board.o $(OBJDIR)/Piece.o $(OBJDIR)/Chess.o $(OBJDIR)/ai.o
GAME = $(BINDIR)/$(PROJECT)

all: bin/$(PROJECT)

#gui.o: gui.c gui.h
#	$(CC) $(CFLAGS) -c gui.c $(INC) $(OPTS) -o gui.o

#gui: gui.o
#	gcc gui.o $(LIBS) $(OPTS) -o chess


gui.o: gui.c gui.h
	$(CC) $(CFLAGS) -c gui.c $(INC) $(OPTS) -o gui.o

gui: $(OBJSUB)
	gcc $(OBJSUB) $(LIBS) $(OPTS) -o gui

Board.o: Board.h Board.c
	$(CC) $(CFLAGS) -c Board.c -o Board.o

Piece.o: Piece.h Piece.c
	$(CC) $(CFLAGS) -c Piece.c -o Piece.o
Chess.o: chess.h chess.c
	$(CC) $(CFLAGS) -c chess.c -o Chess.o
ai.o: aiZach.h aiZach.c
	$(CC) $(CFLAGS) -c aiZach.c -o ai.o


minichess: basicgui.c basicgui.h aiZach.c aiZach.h
	gcc -Wall -std=c11 MiniChessGame.c Board.c Piece.c basicgui.c aiZach.c -o MiniChessGame


#For FInal Project
#make the obj directory
$(OBJS): | $(OBJDIR)
$(OBJDIR):
	mkdir -p $@

$(GAME): | $(BINDIR)
$(BINDIR):
	mkdir -p $@

#    Output $@      Input $<
$(OBJDIR)/gui.o: $(SRCDIR)/gui.c
	gcc -c $< $(INC) $(OPTS) -o  $@

bin/$(PROJECT) : $(OBJDIR)/gui.o $(OBJS)
	$(CC) $(CFLAGS) $(CHESSOBJ) $(LIBS) $(OPTS) -o $@

# Board 
$(OBJDIR)/Board.o: $(SRCDIR)/Board.c
	$(CC) $(CFLAGS) -c $< -o  $@

# Piece
$(OBJDIR)/Piece.o: $(SRCDIR)/Piece.c
	$(CC) $(CFLAGS) -c $< -o  $@

# Chess
$(OBJDIR)/Chess.o: $(SRCDIR)/chess.c
	$(CC) $(CFLAGS) -c $< -o  $@

# AI
$(OBJDIR)/ai.o: $(SRCDIR)/aiZach.c
	$(CC) $(CFLAGS) -c $< -o  $@


tarSource:
	gtar cvzf Chess_V1.0_src.tar.gz README.txt COPYRIGHT.txt INSTALL.txt Makefile doc src

tarUser:
	gtar cvzf Chess_V1.0.tar.gz README.txt COPYRIGHT.txt INSTALL.txt bin doc


clean:
	rm -rf *.o $(PROJECT) $(OBJDIR) $(BINDIR) gui