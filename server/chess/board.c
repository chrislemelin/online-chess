#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "piece.h"
#include "display.h"
#include "board.h"

#define BOARD_HEIGHT 8
#define BOARD_WIDTH 8
#define DISPLAY_X 10
#define DISPLAY_Y 35

#define BOARD_HOR '_'
#define BOARD_VERT '|'
#define BOARD_CORN '+'

int movePiece(struct board *, struct piece *, struct pos *);
int simCheck(struct board * b, struct piece * p,struct pos * move, struct piece * k);


/*
int main(int argc, char * argv[])
{
	clear();
	struct board *boardy =  malloc(sizeof(struct board));
	initBoard(boardy);
//	initBoard(board
	//printf("%d\n", boardy->s_pieces);
//	addPiece(boardy,KING,1,1,0);
//	addPiece(boardy,ROOK,1,2,0);

//	addPiece(boardy,ROOK,1,5,1);
//	addPiece(boardy,KING,1,3,1);
//	addPiece(boardy,ROOK,4,2,1);
//	addPiece(boardy,KING,5,2,1);

	updateAllMoves(boardy);

//	tryMove(boardy,4,2,3,2);
//	tryMove(boardy,3,2,3,4);
//	struct board * b = copyBoard(boardy);
//	deleteBoard(b);

//	addPiece(boardy,BISHOP,1,0,1);
//	addPiece(PAWN,3,1,0,boardy);
//	addPiece(PAWN,4,1,0,boardy);
//	addPiece(PAWN,2,0,1,boardy);

//	removePiece(boardy->pieces[1], boardy);
//	removePiece(boardy->pieces[1], boardy);

//	updateMoves(boardy->pieces[0],boardy);

//	removeMove(boardy->pieces[0],0);

//	updateAllMoves(boardy);
//	updateAllMoves(b);


//	printMoves(b->pieces[1]);
	while(1)
	{
		int quit = 0;
		drawBoard(boardy);
		set_cur_pos(55,0);
		char * input;
		char *end;
		while(1)
		{
			gets(input);
			printf("%s\n",input );
			if(strcmp(input,&"quit")==0)
			{
				quit = 1;
				break;
			}
			int x1 = strtol(input ,&end,10);
			int y1 = strtol(end ,&end,10);
			int x2 = strtol(end ,&end,10);
			int y2 = strtol(end ,&end,10);
			int t = tryMove (boardy,x1,y1,x2,y2,boardy->currentPlayer);
			printf("%d %d %d %d %d\n", x1,y1,x2,y2,t);
			if(t == 1)
			{
				printf("sucess\n");
				break;
			}
		}
		if (quit)
		{
			break;
		}
	}
	deleteBoard(boardy);
}
*/

char * boardToString(struct board * b)
{
	char * tempT = calloc(b->s_pieces*5,1);
	char * temp = malloc(5);
	for(int a = 0 ; a < b->s_pieces ; a++)
	{
		temp[0] = b->pieces[a]->p;
		temp[1] = b->pieces[a]->player+ '0';
		temp[2] = b->pieces[a]->loc->x + '0';
		temp[3] = b->pieces[a]->loc->y + '0';
		temp[4] = '\0';
;		tempT = strcat(tempT,temp);
	}
	return tempT;
}

int printAllMoves(struct board * b)
{
	set_cur_pos(0,0);
	printf("player : %d" ,b->currentPlayer);
	for (int a = 0 ; a< b->s_pieces;a++)
	{
		printMoves(b->pieces[a]);
	}
}

/* returns 2 if the move caused a promotion
 * returns 1 if valid
 * returns -1 if invalid m1
 * returns -2 if invalid m2
 * returns -3 if not your turn
 */
int tryMove(struct board* b,int x1,int y1,int x2,int y2, int player)
{
	if(player != b->currentPlayer)
	{
		return -3;
	}

	struct pos * m1 = makeLoc(x1,y1);
	struct pos * m2 = makeLoc(x2,y2);

	struct piece * mover = getSpace(b,x1,y1);
	if(mover == NULL)
	{
				return -1;
	}
	if(mover->player != b->currentPlayer)
		return -1;

	struct pos * m3 = validMoveForPiece(mover,m2);

	free(m1);
	free(m2);

	if(m3 != NULL)
	{
		movePiece(b, mover, m3);
		updateAllMoves(b);

		if(checkPromotion(b))
		{
			b->currentPlayer = (b->currentPlayer+1)%2;
			return 2;
		}
		return 1;
	}
	else
	{
		return -2;
	}
}

int checkPromotion(struct board * b)
{
	for(int a = 0; a< b->s_pieces;a++)
	{
		if(b->pieces[a]->p == PAWN && b->pieces[a]->player == 0
			 && b->pieces[a]->loc->y == 0)
		{
			b->promotionPawn = b->pieces[a];
			return 1;
		}
		if(b->pieces[a]->p == PAWN && b->pieces[a]->player == 1
			 && b->pieces[a]->loc->y== BOARD_HEIGHT-1)
		{
			b->promotionPawn = b->pieces[a];
			return 1;
		}
	}
	b->promotionPawn = NULL;
	return 0;
}

int promote(struct board * b,char c)
{
	if(c == QUEEN || c == BISHOP || c == KNIGHT || c == ROOK)
	{
		int x = b->promotionPawn->loc->x;
		int y = b->promotionPawn->loc->y;
		removePiece(b,b->promotionPawn);
		b->promotionPawn = NULL;
		addPiece(b, c, x, y, b->currentPlayer);
		return 1;
	}
	return 0;

}

int drawBoard(struct board *b)
{
	clear();
	printAllMoves(b);
//	return 0;
	set_cur_pos(0,20);
	printf("pieces=%d",b->s_pieces);
	set_cur_pos(0,0);

	for (int x = 0 ; x < BOARD_WIDTH; x++)
	{
		for(int y = 0 ; y < BOARD_HEIGHT;y++)
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
		set_cur_pos(BOARD_HEIGHT*2+DISPLAY_Y,4*x+DISPLAY_X);
		put(BOARD_HOR);
		set_cur_pos(BOARD_HEIGHT*2+DISPLAY_Y,4*x+DISPLAY_X+1);
		put(BOARD_HOR);
		set_cur_pos(BOARD_HEIGHT*2+DISPLAY_Y,4*x+DISPLAY_X+2);
		put(BOARD_HOR);
		set_cur_pos(BOARD_HEIGHT*2+DISPLAY_Y,4*x+DISPLAY_X+3);
		put(BOARD_HOR);
		set_cur_pos(DISPLAY_Y,4*x+DISPLAY_X);
		put(BOARD_HOR);

		set_cur_pos(x*2+DISPLAY_Y,BOARD_WIDTH*4+DISPLAY_X);
		put(BOARD_VERT);
		set_cur_pos(x*2+1+DISPLAY_Y,BOARD_WIDTH*4+DISPLAY_X);
		put(BOARD_VERT);

		set_cur_pos(DISPLAY_Y-1,4*x+DISPLAY_X+2);
		put(x+'0');
		set_cur_pos(x*2+1+DISPLAY_Y,DISPLAY_X+1+BOARD_WIDTH*4);
		put(x+'0');


		set_cur_pos(DISPLAY_Y,DISPLAY_X);
		put(BOARD_CORN);
		set_cur_pos(DISPLAY_Y+BOARD_HEIGHT*2,DISPLAY_X);
		put(BOARD_CORN);
		set_cur_pos(DISPLAY_Y,DISPLAY_X+BOARD_WIDTH*4);
		put(BOARD_CORN);
		set_cur_pos(DISPLAY_Y+BOARD_HEIGHT*2,DISPLAY_X+BOARD_WIDTH*4);
		put(BOARD_CORN);
	}
	for(int a = 0; a < b->s_pieces;a++)
	{
		set_cur_pos(b->pieces[a]->loc->y*2+DISPLAY_Y+1,
			b->pieces[a]->loc->x*4 +DISPLAY_X+1);
		if(b->pieces[a]->player == 0)
			printf(BLUE);
		else
			printf(RED);
		put(b->pieces[a]->p);
		printf(RESET);
	}
	set_cur_pos(55,0);

}

struct board * copyBoard(struct board * b)
{
	struct board * nBoard = malloc(sizeof(struct board));
	nBoard->s_pieces = 0;

	for(int a = 0; a < b->s_pieces;a++)
	{
		int x = b->pieces[a]->loc->x;
		int y = b->pieces[a]->loc->y;

		addPiece(nBoard,b->pieces[a]->p,x,
			y,b->pieces[a]->player);

		int notM = b->pieces[a]->notMoved;

		nBoard->pieces[a]->notMoved = notM;
	}
	return nBoard;
}

int deleteBoard(struct board * b)
{
	while(b->s_pieces != 0)
		removePiece(b,b->pieces[0]);
	free(b);
	return 0;
}

int initBoard(struct board *b)
{
	b->s_pieces = 0;
	b->currentPlayer = 0;
	b->promotionPawn = NULL;
	for(int a = 0; a < BOARD_WIDTH;a++)
	{
		//printf("%d\n",a);
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

	updateAllMoves(b);
	return 0;
}

/* returns number of player of the piece at the position x,y
 * returns -1 if no piece is at the position
 * returns -2 if coordinate is not on the board
 * return 2 if valid enzzZ
 */
int checkSpace(struct board * b, int x , int y, int g, int player)
{
	if(x < 0 || x >= BOARD_WIDTH)
		return -2;
	if(y < 0 || y >= BOARD_HEIGHT)
		return -2;

	for(int a = 0; a < b->s_pieces;a++)
	{
		if(b->pieces[a]->loc->x == x && b->pieces[a]->loc->y ==y)
			return b->pieces[a]->player;
		if(g && b->pieces[a]->ghostLoc != NULL && b->pieces[a]->ghostLoc->x == x
			 && b->pieces[a]->ghostLoc->y ==y && b->pieces[a]->player != player)
			 return 2;
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
	return NULL;
}

int getOrder(struct board * b, struct piece * p)
{
	for(int a = 0; a < b->s_pieces;a++)
	{
		if(b->pieces[a] == p)
		{
			return a;
		}
	}
	return 0;
}

/* return -1 if no winner yet
 * returns 0 if player 0 wins
 * returns 1 if player 1 wins
 * returns 2 if a tie
 */
int whoWon(struct board * b)
{
	int moves0 = 1;
	int moves1= 1;
	struct piece * k0 = NULL;
	struct piece * k1 = NULL;

	for(int a = 0; a < b->s_pieces; a++)
	{
		if(b->pieces[a]->p == KING)
		{
			if(b->pieces[a]->player == 0)
				k0 = b->pieces[a];
			if(b->pieces[a]->player == 1)
				k1 = b->pieces[a];
		}
		if(b->pieces[a]->player == 0 && b->pieces[a]->s_validmoves > 0)
		{
			moves0 = 0;
			continue;
		}
	}

	for(int a = 0; a < b->s_pieces; a++)
	{
		if(b->pieces[a]->player == 1 && b->pieces[a]->s_validmoves > 0)
		{
			moves1 = 0;
			break;
		}
	}

	int check0 = incheckCheck(b,k0,k0->loc);
	int check1 = incheckCheck(b,k1,k1->loc);

	if(moves0 && check0 == 1)
		return 1;
	if(moves1 && check1 == 1)
		return 0;
	if(moves0 || moves1)
		return 2;
	return -1;

}

/* returns 0 if not in check
 * returns 1 if in check
 */
int updateAllMovesSim(struct board * b)
{
	for(int a = 0 ; a< b->s_pieces; a++)
	{
		if(b->pieces[a]->p == KING)
		{
			continue;
		}
		updateMoves(b,b->pieces[a]);
	}
}

int updateAllMoves(struct board * b)
{
	struct piece * k0 = NULL;
	struct piece * k1 = NULL;
	for(int a = 0 ; a< b->s_pieces; a++)
	{
		// saves kings for later, all other pieces must
		// be updated BEFORE the king
		if(b->pieces[a]->p == KING)
		{
			if(b->pieces[a]->player == 0)
				k0 = b->pieces[a];
			else
				k1 = b->pieces[a];
			continue;
		}
		updateMoves(b,b->pieces[a]);
	}
	for(int a = 0 ; a< b->s_pieces; a++)
	{
		if(b->pieces[a]->p == ROOK)
		{
			addCastleing(b,b->pieces[a]);
		}
	}
	if(k0 != NULL)
	{
		for(int a = 0 ; a< b->s_pieces; a++)
		{
			if(b->pieces[a]->player == 1)
				continue;
			for(int c = 0 ; c < b->pieces[a]->s_moves;c++)
			{
				if(b->pieces[a]->moves[c]-> type == 2 ||b->pieces[a]->moves[c]-> type == 4)
				{
					continue;
				}
				if(simCheck(b,b->pieces[a],b->pieces[a]->moves[c],k0) == 1)
				{
					removeMove(b->pieces[a],c);
				}
			}
		}
	}
	if(k1 != NULL)
	{
		for(int a = 0 ; a< b->s_pieces; a++)
		{
			if(b->pieces[a]->player == 0)
				continue;
			for(int c = 0 ; c < b->pieces[a]->s_moves;c++)
			{
				if(simCheck(b,b->pieces[a],b->pieces[a]->moves[c],k1) == 1)
				{
					removeMove(b->pieces[a],c);
				}
			}
		}
	}

	if(k0 != NULL)
	{
		updateMoves(b,k0);
	}
	if(k1 != NULL)
	{
		updateMoves(b,k1);
	}
	specialKingMoveCheck(b,k0, k1);

}

int movePiece(struct board * b, struct piece * p, struct pos* move)
{

	for(int a = 0 ; a < b->s_pieces; a++)
	{
		clearGhost(b->pieces[a]);
	}

	if(move->taken != NULL)
	{
		removePiece(b,move->taken);
	}

	p->notMoved = 0;


	if(move->type != 4)
	{
		free(p->loc);
		p->loc = makeLoc(move->x,move->y);
	}

	if(move->type == 4)
	{
		free(p->loc);
		p->loc = 	makeLoc(move->additionalM1->x,move->additionalM1->y);
		//free(move->additionalP->loc);
		move->additionalP->loc = 	makeLoc(move->additionalM2->x,move->additionalM2->y);
	}
	if(move->type == 6)
	{
		if(p->player ==0)
		{
			p->ghostLoc= makeLoc(move->x, move->y +1);	//specialKingMoveCheck(b,k1,k2);
		}
		else
		{
			p->ghostLoc= makeLoc(move->x, move->y -1);
			//printf("made ghosty at %d,%d ",move->x, move->y +1);
		}
	}
	else
	{
		p->ghostLoc = NULL;
	}
	if(b->currentPlayer == 0)
	{
		b->currentPlayer = 1;
	}
	else
		b->currentPlayer = 0;
}

// returns 0 if NOT in check after the move
// return 1 if in check after the move
int simCheck(struct board * b, struct piece * p, struct pos * move, struct piece * k )
{
	struct piece * inactivePiece = NULL;
	struct pos * tempLoc;
	struct board * nBoard = copyBoard(b);

	struct pos * nMove = makeLoc(move->x, move->y);
	nMove->type = move->type;
	if(move->type ==4)
	{
		nMove->additionalM1 = move->additionalM1;
		nMove->additionalM2 = move->additionalM2;
		nMove->additionalP = getSpace(nBoard, move->x, move->y);
	}
 	int temp = getOrder(b, p);
	int temp1;
	if(move->taken != NULL)
	{
		temp1 = getOrder(b,move->taken);
		nMove->taken = nBoard->pieces[temp1];
	}
	else{
		nMove->taken = NULL;
	}

//	printAllMoves(nBoard);
	movePiece(nBoard,nBoard->pieces[temp],nMove);
	updateAllMovesSim(nBoard);
	free(nMove);
	if(incheckCheck(nBoard,k,k->loc) == 1)
	{
		deleteBoard(nBoard);
		return 1;
	}
	deleteBoard(nBoard);
	return 0;
}
