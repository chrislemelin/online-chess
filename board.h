#ifndef BOARD
#define BOARD

#include "piece.h"

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
}board;

//int simCheck(struct board * b, struct piece * p,struct pos * move, struct piece * k);

struct piece * getSpace(struct board * b, int x , int y);
int checkSpace(struct board * b, int x , int y);
int updateAllMoves(struct board * b);
struct board * copyBoard(struct board * b);
int deleteBoard(struct board * b);





#endif
