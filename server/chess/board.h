#ifndef BOARD_H
#define BOARD_H

#include "../player.h"


#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"


struct board
{
	struct piece *pieces[32];
	int s_pieces;
	int currentPlayer;
	struct piece *promotionPawn;
	int players;
	struct player *p1;
	struct player* p2;

}board;


struct piece * getSpace(struct board * b, int x , int y);
int checkSpace(struct board * b, int x , int y, int g, int player);
int updateAllMoves(struct board * b);
struct board * copyBoard(struct board * b);
int deleteBoard(struct board * b);
int tryMove(struct board* b,int x1,int y1,int x2,int y2, int player);
int drawBoard(struct board *b);
int initBoard(struct board *b);
char * boardToString(struct board * b);
int whoWon(struct board * b);
int checkPromotion(struct board * b);
int promote(struct board * b,char c);
int tryMoveAdapter(struct board * b, char * s, int player);






#endif
