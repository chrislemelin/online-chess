#ifndef CLIENTVIEW_H
#define CLIENTVIEW_H

#define CLEARLINE "%c[2K",27

char * lobby;
char * board;

int updateLobby(char * nLobby);
int updateBoard(char * nBoard);
int drawBoard();
int clearInput();
int printMessage(char * message);
int drawLobby();
void printHelpMessage(int * inGame);

#endif
