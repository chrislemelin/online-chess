#ifndef PIECE
#define PIECE

#define PAWN 'P'
#define ROOK 'R'
#define KNIGHT 'K'
#define BISHOP 'B'
#define QUEEN 'Q'
#define KING '+'

#include "board.h"

struct pos
{
	int x,y;

	/*
	normal = 0
	capturing = 1
	covering = 2
	will put player in check = 3
	*/
	int type;
}pos;

struct piece
{
	struct pos *loc;
	char p;
	int player;
	struct pos * moves[64];
	int s_moves;
	int s_validmoves;
	int covered;
	int active;
	int notMoved;
}piece;


int printMoves(struct piece *p);
int addPiece(struct board *b, char p, int x, int y, int player);
int addMove(struct board * b , struct piece *p, int x , int y , int target );
int updateMoves(struct board *b ,struct piece *p);
int equalPos(struct pos * pos1, struct pos * pos2);
int addMoves(struct board * b, struct piece *p, int x , int y, int opp);
int removePiece(struct board * b, struct piece *p);
int specialKingMoveCheck(struct board *b, struct piece * k1, struct piece * k2);
int removeMove(struct piece *p, int pos);
int incheckCheck(struct board * b, struct piece * p, struct pos * m);
int clearMoves(struct piece * p);
struct pos * makeLoc(int x, int y);


#endif
