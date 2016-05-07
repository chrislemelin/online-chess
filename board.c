#include <stdio.h>
#include <stdlib.h>
#include "display.h"
#include "piece.h"

#define BOARD_LENGTH 8
#define BOARD_WIDTH 8
#define DISPLAY_X 10
#define DISPLAY_Y 20

#define BOARD_HOR '-'
#define BOARD_VERT '|'
#define BOARD_CORN '+'



int drawBoard(struct board *);
int initBoard(struct board *);
int occupied(struct board *, int, int);

int main(int argc, char * argv[])
{
	clear();
	struct board *boardy =  malloc(sizeof(struct board));
//	initBoard(board
	//printf("%d\n", boardy->s_pieces);
	addPiece(boardy,KING,1,1,0);
//	addPiece(boardy,ROOK,1,5,1);
//	addPiece(boardy,KING,1,3,1);
	addPiece(boardy,ROOK,1,2,1);
	addPiece(boardy,ROOK,1,5,1);
	addPiece(boardy,KING,3,2,1);

//	addPiece(boardy,BISHOP,1,0,1);
//	addPiece(PAWN,3,1,0,boardy);
//	addPiece(PAWN,4,1,0,boardy);
//	addPiece(PAWN,2,0,1,boardy);

//	removePiece(boardy->pieces[1], boardy);
//	removePiece(boardy->pieces[1], boardy);

//	updateMoves(boardy->pieces[0],boardy);

//	removeMove(boardy->pieces[0],0);

	updateAllMoves(boardy);

	set_cur_pos(5,1);
	printMoves(boardy->pieces[0]);
	printMoves(boardy->pieces[1]);
	drawBoard(boardy);

	set_cur_pos(50,0);
}

//
int drawBoard(struct board *b)
{
//	clear();

	for (int x = 0 ; x < BOARD_WIDTH; x++)
	{
		for(int y = 0 ; y < BOARD_LENGTH;y++)
		{

			set_cur_pos(y*2+DISPLAY_Y+2, 4*x+DISPLAY_X);
			put(BOARD_VERT);
			set_cur_pos(y*2+DISPLAY_Y+1, 4*x+DISPLAY_X);
			put(BOARD_VERT);
			set_cur_pos(y*2+DISPLAY_Y, 4*x+DISPLAY_X);
			put(BOARD_VERT);
			set_cur_pos(y*2+DISPLAY_Y, 4*x+DISPLAY_X+1);
			put(BOARD_HOR);
			set_cur_pos(y*2+DISPLAY_Y, 4*x+DISPLAY_X+2);
			put(BOARD_HOR);
			set_cur_pos(y*2+DISPLAY_Y, 4*x+DISPLAY_X+3);
			put(BOARD_HOR);

		}
		set_cur_pos(BOARD_LENGTH*2+DISPLAY_Y,4*x+DISPLAY_X);
		put(BOARD_HOR);
		set_cur_pos(BOARD_LENGTH*2+DISPLAY_Y,4*x+DISPLAY_X+1);
		put(BOARD_HOR);
		set_cur_pos(BOARD_LENGTH*2+DISPLAY_Y,4*x+DISPLAY_X+2);
		put(BOARD_HOR);
		set_cur_pos(BOARD_LENGTH*2+DISPLAY_Y,4*x+DISPLAY_X+3);
		put(BOARD_HOR);
		set_cur_pos(DISPLAY_Y,4*x+DISPLAY_X);
		put(BOARD_HOR);

		set_cur_pos(x*2+DISPLAY_Y,BOARD_WIDTH*4+DISPLAY_X);
		put(BOARD_VERT);
		set_cur_pos(x*2+1+DISPLAY_Y,BOARD_WIDTH*4+DISPLAY_X);
		put(BOARD_VERT);


		set_cur_pos(DISPLAY_Y,DISPLAY_X);
		put(BOARD_CORN);
		set_cur_pos(DISPLAY_Y+BOARD_LENGTH*2,DISPLAY_X);
		put(BOARD_CORN);
		set_cur_pos(DISPLAY_Y,DISPLAY_X+BOARD_WIDTH*4);
		put(BOARD_CORN);
		set_cur_pos(DISPLAY_Y+BOARD_LENGTH*2,DISPLAY_X+BOARD_WIDTH*4);
		put(BOARD_CORN);
	}
	for(int a = 0; a < b->s_pieces;a++)
	{
		set_cur_pos(b->pieces[a]->loc->y*2+DISPLAY_Y+1,b->pieces[a]->loc->x*4 +DISPLAY_X+1);
		if(b->pieces[a]->player == 0)
			printf(BLUE);
		else
			printf(RED);
		put(b->pieces[a]->p);
		printf(RESET);
	}

}

int initBoard(struct board *b)
{
	b->s_pieces = 0;
	for(int a = 0; a < BOARD_WIDTH;a++)
	{
		printf("%d\n",a);
		addPiece(b,PAWN,a,6,0);
		addPiece(b,PAWN,a,1,1);
	}
	addPiece(b,ROOK,0,0,1);
	addPiece(b,ROOK,7,0,1);
	addPiece(b,ROOK,7,7,0);
	addPiece(b,ROOK,0,7,0);

	addPiece(b,KNIGHT,1,0,1);
	addPiece(b,KNIGHT,6,0,1);
	addPiece(b,KNIGHT,1,7,0);
	addPiece(b,KNIGHT,6,7,0);

	addPiece(b,BISHOP,2,0,1);
	addPiece(b,BISHOP,5,0,1);
	addPiece(b,BISHOP,2,7,0);
	addPiece(b,BISHOP,5,7,0);

	addPiece(b,QUEEN,4,7,0);
	addPiece(b,QUEEN,4,0,1);

	addPiece(b,KING,3,7,0);
	addPiece(b,KING,3,0,1);
	return 0;
}


/* returns number of player of the piece at the position x,y
 * returns -1 if no piece is at the position
 * returns -2 if coordinate is not on the board
 */
int checkSpace(struct board * b, int x , int y)
{
	if(x < 0 || x >= BOARD_WIDTH)
		return -2;
	if(y < 0 || y >= BOARD_LENGTH)
		return -2;

	for(int a = 0; a < b->s_pieces;a++)
	{
		if(b->pieces[a]->loc->x == x && b->pieces[a]->loc->y ==y)
			return b->pieces[a]->player;
	}
	return -1;
}

struct piece * getSpace(struct board * b, int x , int y)
{
	for(int a = 0; a < b->s_pieces;a++)
	{
		if(b->pieces[a]->loc->x == x && b->pieces[a]->loc->y == y)
		{
			return b->pieces[a];
		}

	}
	return 0;
}
int updateAllMoves(struct board * b)
{
	struct piece * k1 = NULL;
	struct piece * k2 = NULL;
	for(int a = 0 ; a< b->s_pieces; a++)
	{
		// saves kings for later, all other pieces must
		// be updated BEFORE the king
		if(b->pieces[a]->p == KING)
		{
			if(k1 == NULL)
				k1 = b->pieces[a];
			else
				k2 = b->pieces[a];
			continue;
		}
		updateMoves(b,b->pieces[a]);
	}

	if(k1 != NULL)
	{
		updateMoves(b,k1);
	}
	if(k2 != NULL)
	{
		updateMoves(b,k2);
	}
	specialKingMoveCheck(b,k1,k2);

}

int movePiece(struct board * b, struct piece * p, struct pos * move)
{
	if(move->type == 2 || move->type == 3)
		return 0;



}

/*
int simCheck(struct board * b, struct piece * p, int a, struct piece * k )
{
	struct pos * move = p->moves[a];
	if(move->type = 1)
	{

	}


}
*/
