GTKINC	= `pkg-config --cflags gtk+-2.0`
GTKLIBS	= `pkg-config --libs gtk+-2.0`

INC	 = $(GTKINC)
LIBS = $(GTKLIBS)


DEBUG	= -O2
CFLAGS	= -Wall -std=c11 $(DEBUG) $(GTKINC) -c
LFLAGS	= -Wall -std=c11 $(DEBUG) $(GTKLIBS) -lm
CC = gcc
PROJECT = GoldfishClient GoldfishServer
OBJDIR = obj
SRCDIR = src
BINDIR = bin
SRCS    = $(wildcard $(SRCDIR)/*.c)
OBJS   = $(OBJDIR)/GUIServer.o $(OBJDIR)/GUIClient.o $(OBJDIR)/table.o $(OBJDIR)/deck.o $(OBJDIR)/com.o
OBJSUB = GUIServer.o GUIClient.o
GAME = $(BINDIR)/$(PROJECT)


all: bin/GoldfishClient bin/GoldfishServer 

testserver: GUIServer GUIServer.c GUIServer.o
	./GUIServer 10113
	
test:
	@echo "To run the examples, first start the server in one terminal,"
	@echo "then start one (or multiple) client(s) in another."
	@echo
	@echo "For example:"
	@echo "crystalcove% ./bin/GoldfishServer 10111"
	@echo "crystalcove% ./bin/GoldfishClient crystalcove 10111"


GUIServer.o: GUIServer.c  
	gcc GUIServer.c $(CFLAGS) -o GUIServer.o

GUIServer: GUIServer.o
	gcc GUIServer.o $(LFLAGS) -o GUIServer

GUIClient.o: GUIClient.c GUIClient.h
	$(CC) $(CFLAGS) -c GUIClient.c $(INC) $(OPTS) -o GUIClient.o

GUIClient: $(OBJSUB)
	gcc -Wall -std=c11 deck.c com.c table.c GUIClient.o $(LIBS) $(OPTS) -o GUIClient
	./GUIClient bondi 10112

Poker: poker.c table.c deck.c
	gcc -Wall -std=c11 poker.c table.c deck.c -o Poker


#For Submission Purposes
#Make Folders
$(OBJS): | $(OBJDIR)
$(OBJDIR):
	mkdir -p $@

$(GAME): | $(BINDIR)
$(BINDIR):
	mkdir -p $@



bin/GoldfishClient : $(OBJS)
	$(CC) $(OBJDIR)/GUIClient.o $(OBJDIR)/table.o $(OBJDIR)/deck.o $(OBJDIR)/com.o $(LIBS) $(OPTS) -o $@

bin/GoldfishServer : $(OBJS)
	gcc -Wall -std=c11 $(OBJDIR)/table.o $(OBJDIR)/deck.o $(OBJDIR)/com.o $(OBJDIR)/GUIServer.o $(LIBS) $(OPTS) -o $@


#	$(CC) $(OBJDIR)/GUIServer.o $(LIBS) $(OPTS) -o $@




#    Output $@      Input $<
$(OBJDIR)/GUIClient.o: $(SRCDIR)/GUIClient.c
	$(CC) $(CFLAGS) -c $< $(INC) $(OPTS) -o  $@

$(OBJDIR)/GUIServer.o: $(SRCDIR)/GUIServer.c
	$(CC) $(CFLAGS) -c $< $(INC) $(OPTS) -o  $@

$(OBJDIR)/table.o: $(SRCDIR)/table.c
	$(CC) $(CFLAGS) -c $< $(INC) $(OPTS) -o  $@

$(OBJDIR)/deck.o: $(SRCDIR)/deck.c
	$(CC) $(CFLAGS) -c $< $(INC) $(OPTS) -o  $@

$(OBJDIR)/com.o: $(SRCDIR)/com.c
	$(CC) $(CFLAGS) -c $< $(INC) $(OPTS) -o  $@


test-Servergui: bin/GoldfishServer
	./bin/GoldfishServer 10112

test-Client: bin/GoldfishClient
	./bin/GoldfishClient bondi 10112


tarSource:
	gtar cvzf Poker_V1.0_src.tar.gz README.txt COPYRIGHT.txt INSTALL.txt Makefile doc src bin
tarUser:
	gtar cvzf Poker_V1.0.tar.gz README.txt COPYRIGHT.txt INSTALL.txt bin doc src



clean:
	rm -f *.o 
	rm bin/GoldfishClient bin/GoldfishServer
	rm -f GUIServer