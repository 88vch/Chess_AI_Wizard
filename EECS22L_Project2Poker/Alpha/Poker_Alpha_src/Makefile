GTKINC	= `pkg-config --cflags gtk+-2.0`
GTKLIBS	= `pkg-config --libs gtk+-2.0`

INC	 = $(GTKINC)
LIBS = $(GTKLIBS)


DEBUG	= -O2
CFLAGS	= -Wall -std=c11 $(DEBUG) $(GTKINC) -c
LFLAGS	= -Wall -std=c11 $(DEBUG) $(GTKLIBS) -lm
CC = gcc
PROJECT = GUIClient GUIServer
OBJDIR = obj
SRCDIR = src
BINDIR = bin
SRCS    = $(wildcard $(SRCDIR)/*.c)
OBJS   = $(OBJDIR)/GUIServer.o $(OBJDIR)/GUIClient.o
OBJSUB = GUIServer.o GUIClient.o
GAME = $(BINDIR)/$(PROJECT)


all: bin/GUIClient bin/GUIServer bin/Poker



test:
	@echo "To run the examples, first start the server in one terminal,"
	@echo "then start one (or multiple) client(s) in another."
	@echo
	@echo "For example:"
	@echo "crystalcove% ./GUIServer 10111"
	@echo "crystalcove% ./GUIClient crystalcove 10111"


GUIServer.o: GUIServer.c  
	gcc GUIServer.c $(CFLAGS) -o GUIServer.o

GUIServer: GUIServer.o
	gcc GUIServer.o $(LFLAGS) -o GUIServer

GUIClient.o: GUIClient.c GUIClient.h
	$(CC) $(CFLAGS) -c GUIClient.c $(INC) $(OPTS) -o GUIClient.o

GUIClient: $(OBJSUB)
	gcc $(OBJSUB) $(LIBS) $(OPTS) -o GUIClient


#Make Folders
$(OBJS): | $(OBJDIR)
$(OBJDIR):
	mkdir -p $@

$(GAME): | $(BINDIR)
$(BINDIR):
	mkdir -p $@



bin/GUIClient : $(OBJS)
	$(CC) GUIClient.o $(LIBS) $(OPTS) -o $@

bin/GUIServer : $(OBJS)
	$(CC) GUIServer.o $(LIBS) $(OPTS) -o $@

bin/Poker: $(OBJS)
	gcc -Wall -std=c11 $(SRCDIR)/poker.c -o $@


#    Output $@      Input $<
$(OBJDIR)/GuiClient.o: $(SRCDIR)/GUIClient.c
	$(CC) $(CFLAGS) -c $< $(INC) $(OPTS) -o  $@

$(OBJDIR)/GUIServer.o: $(SRCDIR)/GUIServer.c
	$(CC) $(CFLAGS) -c $< $(INC) $(OPTS) -o  $@



test-Servergui: GUIServer
	./bin/GUIServer 10112

test-Client: GUIClient
	./bin/GUIClient bondi 10112

tar:
	gtar cvzf Poker_Alpha_src.tar.gz README.txt COPYRIGHT.txt INSTALL.txt Makefile doc src bin
	gtar cvzf Poker_Alpha.tar.gz README.txt COPYRIGHT.txt INSTALL.txt bin doc



clean:
	rm -f *.o
	rm /bin/GUIServer /bin/GUIClient /bin/Poker
	rm -f GUIServer