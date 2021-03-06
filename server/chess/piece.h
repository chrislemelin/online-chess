#ifndef PIECE_H
#define PIECE_H

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
	covering = -2
	will put player in check = -1

	normal = 0
	capturing = 1
	non-caturing move (pawn moving forward) = 3
	rooking = 4
	enpassant = 5
	double move = 6
	*/
	int type;
	struct piece * taken;
	struct piece * additionalP;
	struct pos * additionalM1;
	struct pos * additionalM2;

}pos;

struct piece
{
	struct pos * loc;
	struct pos * ghostLoc;
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
int addCastleing(struct board * b, struct piece * r);
int clearGhost(struct piece * p);
struct pos * validMoveForPiece(struct piece *p,struct pos * m);



#endif
