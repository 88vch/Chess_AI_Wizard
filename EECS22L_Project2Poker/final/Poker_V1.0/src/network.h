#ifndef NETWORK_H
#define NETWORK_H

#include "table.h"

//gets back user input for the given player, also sends gamestate to all other players
Move ModifiedServerLoop(Table table, int playerNum, int argc, int portNum);

#endif