#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "display.h"
#include "piece.h"
#include "board.h"


int addPiece(struct board *b, char p, int x, int y, int player)
{
	struct piece *thingy = malloc(sizeof(struct piece));
	struct pos *loc = makeLoc(x,y);
	thingy->notMoved = 1;
	thingy->ghostLoc = NULL;
	thingy->s_moves = 0;
	thingy->s_validmoves = 0;
	thingy->loc = loc;
	thingy->p = p;
	thingy->player = player;
	thingy->active = 1;
	b->pieces[b->s_pieces] = thingy;
	b->s_pieces++;
}

/*@author
*/
int removePiece(struct board * b, struct piece *p)
{
	for(int a = 0; a < b->s_pieces;a++)
	{
		if(b->pieces[a] == p)
		{
			clearMoves(p);
			free(p->loc);
			free(p);
			for(int c = a+1; c< b->s_pieces;c++)
			{
				b->pieces[c-1] = b->pieces[c];
			}
			a = b->s_pieces;
		}
	}
	b->s_pieces--;
	return 0;
}

int removeMove(struct piece *p, int pos)
{
	/*
	free(p->moves[pos]);
	for(int c = pos+1; c< p->s_moves;c++)
	{
		p->moves[c-1] = p->moves[c];
	}
	p->s_moves--;
	*/
	if(p->moves[pos]->type >= 0)
	{
		p->moves[pos]->type = -1;
		p->s_validmoves--;
	}
}



/*
	returns the value of the checkspace
 */
int addMove(struct board * b , struct piece *p, int x , int y , int opp)
{
	int check;
	if (p->p == PAWN)
		check = checkSpace(b,x,y,1,p->player);
	else
		check = checkSpace(b,x,y,0,0);
	if(-1 == check)
	{
		struct pos *m = makeLoc(x,y);
		m->taken = NULL;
		p->moves[p->s_moves] = m;
		p->s_moves++;
		p->s_validmoves++;
		if(p->p == PAWN && abs(p->loc->y-y) == 2)
		{
			m->type = 6;
		}
		else
		{
			m->type = 0;
		}
	}

	else if(opp == check)
	{
		struct pos *m = makeLoc(x,y);
		m->type = 1;
		m->taken = getSpace(b,x,y);
		p->moves[p->s_moves] = m;
		p->s_moves++;
		p->s_validmoves++;
	}
	else if(p->player == check)
	{
		struct pos *m = makeLoc(x,y);
		m->taken = NULL;
		m->type = -2;
		p->moves[p->s_moves] = m;
		p->s_moves++;
	}

	else if(2 == check)
	{
		if(p->p == PAWN)
		{
			struct pos *m = makeLoc(x,y);
			m->type = 5;
			p->moves[p->s_moves] = m;
			p->s_moves++;
			if(p->player == 0)
			{
				m->taken = getSpace(b,x,y+1);
			}
			else
			{
				m->taken = getSpace(b,x,y-1);
			}
		}
	}

	return check;
}

int addMoves(struct board * b, struct piece *p, int x , int y, int opp)
{
	int tempx = p->loc->x;
	int tempy = p->loc->y;

	while(1)
	{
		tempx+=x;
		tempy+=y;

		int check = addMove(b,p,tempx,tempy,opp);
		if(check == -1)
		{
			continue;
		}
		break;
	}
}

int clearMoves(struct piece * p)
{
	for(int a = 0; a < p->s_moves;a++)
	{
		free(p->moves[a]);
	}
	p->s_moves = 0;
	p->s_validmoves = 0;
}

int clearGhost(struct piece * p)
{
	if(p->ghostLoc != NULL)
	{
		free(p->ghostLoc);
		p->ghostLoc = NULL;
	}
}

int addCastleing(struct board * b, struct piece * r)
{
	if(r->notMoved == 0)
	{
		return 0;
	}
	for(int a = 0 ; a< r->s_moves; a++)
	{
		if(r->moves[a]->type == -2 &&
			getSpace(b,r->moves[a]->x,r->moves[a]->y)->p == KING &&
			r->notMoved && getSpace(b,r->moves[a]->x,r->moves[a]->y)->notMoved)
		{
			struct pos *m = makeLoc(r->moves[a]->x,r->moves[a]->y);
			m->additionalM1 = makeLoc(r->moves[a]->x,r->moves[a]->y);
			m->type = 4;
			m->taken = NULL;
			m->additionalP = 	getSpace(b,r->moves[a]->x,r->moves[a]->y);
			//castle to the right
			if(r->moves[a]->x > r->loc->x)
			{
				m->additionalM1->x--;
				m->additionalM2 =  makeLoc(m->additionalP->loc->x-2,m->additionalP->loc->y);
			}
			//casle to the left
			else
			{
				m->additionalM1->x++;
				m->additionalM2 =  makeLoc(m->additionalP->loc->x+2,m->additionalP->loc->y);
			}
			r->moves[r->s_moves] = m;
			r->s_moves++;
			r->s_validmoves++;
		}
	}
}



/* return the number of moves that the piece can make
 *
 */
int updateMoves(struct board *b ,struct piece *p)
{
	clearMoves(p);
	int opp;
	if(p->player ==0)
		opp = 1;
	else
		opp = 0;

	if(p->p == PAWN)
	{
		int temp;
		if(p->player == 0)
		{
			temp = -1;
		}
		else
		{
			temp = 1;
		}

		if (checkSpace(b,p->loc->x,p->loc->y+temp,0,0) == -1)
			addMove(b,p,p->loc->x,p->loc->y+temp,opp);
		if (checkSpace(b,p->loc->x+1,p->loc->y+temp,1,p->player) != -1)
			addMove(b,p,p->loc->x+1,p->loc->y+temp,opp);
		if (checkSpace(b,p->loc->x-1,p->loc->y+temp,1,p->player) != -1)
			addMove(b,p,p->loc->x-1,p->loc->y+temp,opp);
		if (p->notMoved && checkSpace(b,p->loc->x,p->loc->y+temp,0,0) == -1
	&& checkSpace(b,p->loc->x,p->loc->y+temp*2,0,0) == -1)
		{
			addMove(b,p,p->loc->x,p->loc->y+ temp*2,opp);
		}
	}

	if(p->p == ROOK)
	{
		addMoves(b,p,1,0,opp);
		addMoves(b,p,-1,0,opp);
		addMoves(b,p,0,1,opp);
		addMoves(b,p,0,-1,opp);

	}
	if(p->p == KNIGHT)
	{
		addMove(b,p,p->loc->x+1,p->loc->y+2,opp);
		addMove(b,p,p->loc->x+1,p->loc->y-2,opp);
		addMove(b,p,p->loc->x-1,p->loc->y+2,opp);
		addMove(b,p,p->loc->x-1,p->loc->y-2,opp);
		// switch 1 and 2
		addMove(b,p,p->loc->x+2,p->loc->y+1,opp);
		addMove(b,p,p->loc->x+2,p->loc->y-1,opp);
		addMove(b,p,p->loc->x-2,p->loc->y+1,opp);
		addMove(b,p,p->loc->x-2,p->loc->y-1,opp);
	}
	if(p->p == BISHOP)
	{
		addMoves(b,p,1,1,opp);
		addMoves(b,p,-1,1,opp);
		addMoves(b,p,1,-1,opp);
		addMoves(b,p,-1,-1,opp);
	}
	if(p->p == QUEEN)
	{
		addMoves(b,p,1,1,opp);
		addMoves(b,p,-1,1,opp);
		addMoves(b,p,1,-1,opp);
		addMoves(b,p,-1,-1,opp);
		addMoves(b,p,1,0,opp);
		addMoves(b,p,-1,0,opp);
		addMoves(b,p,0,1,opp);
		addMoves(b,p,0,-1,opp);
	}

	if(p->p == KING)
	{
		addMove(b,p,p->loc->x+1,p->loc->y,opp);
		addMove(b,p,p->loc->x-1,p->loc->y,opp);
		addMove(b,p,p->loc->x,p->loc->y-1,opp);
		addMove(b,p,p->loc->x,p->loc->y+1,opp);
		addMove(b,p,p->loc->x+1,p->loc->y+1,opp);
		addMove(b,p,p->loc->x-1,p->loc->y+1,opp);
		addMove(b,p,p->loc->x-1,p->loc->y-1,opp);
		addMove(b,p,p->loc->x+1,p->loc->y-1,opp);
	}
	return p->s_moves;

}
int specialKingMoveCheck(struct board *b, struct piece * k1, struct piece * k2)
{
	if (k1 != NULL)
	{
		for(int z = 0 ; z<k1->s_moves; z++)
		{
			if(incheckCheck(b,k1,k1->moves[z]) == 1)
			{
				removeMove(k1,z);
			}
		}
	}
	if (k2 != NULL)
	{
		for(int z = 0 ; z<k2->s_moves; z++)
		{
			if(incheckCheck(b,k2,k2->moves[z]) == 1)
			{
				removeMove(k2,z);
			}
		}
	}

	return 0;

}

/*
 * return 0 if not in check
 * return 1 if in check
 * return 2 if in check by a king, special case
 *
 */
int incheckCheck(struct board * b, struct piece * p, struct pos * m)
{
	int toReturn = 0;
	for(int x = 0 ;x<b->s_pieces;x++)
	{
		if(b->pieces[x]->player == p->player || p->active == 0 )
			continue;

		for(int c = 0; c<b->pieces[x]->s_moves;c++)
		{
			if(b->pieces[x]->moves[c]->type == 3)
				continue;
			if(equalPos(m, b->pieces[x]->moves[c]) == 1)
			{
				if(b->pieces[x]->p == KING)
					return 2;
				else
					toReturn = 1;
			}
		}
	}
	return toReturn;
}

int printMoves(struct piece *p)
{
	if(p->player == 0)
		printf(BLUE);
	else
		printf(RED);
	printf("%c : ",p->p);
	printf(RESET);
	printf("loc : (%d,%d),valid moves :%d, ",p->loc->x, p->loc->y,p->s_validmoves);
	for(int a = 0 ; a < p->s_moves;a++)
	{
		printf("type :%d (%d,%d), ",p->moves[a]->type, p->moves[a]->x, p->moves[a]->y);
		if(p->moves[a]->type == 4)
		{
			printf("%c  (%d,%d) ( %d,%d)",p->moves[a]->additionalP->p,
			 p->moves[a]->additionalM1->x, p->moves[a]->additionalM1->y,
			 p->moves[a]->additionalM2->x, p->moves[a]->additionalM2->y);
		}
	}
	printf("\n");
}


//returns 1 if equal, else returns 0
int equalPos(struct pos * pos1, struct pos * pos2)
{
	if(pos1->x == pos2->x && pos1->y == pos2->y)
		return 1;
	else
		return 0;
}

struct pos * makeLoc(int x, int y)
{
	struct pos * l = malloc(sizeof(struct pos));
	int xnew = x;
	int ynew = y;
	l->x = xnew;
	l->y = ynew;
	return l;
}


/*	returns NULL if not valid
 *	returns pointer to move if valid
 */
struct pos * validMoveForPiece(struct piece *p,struct pos * m)
{
		for(int a = 0; a < p->s_moves;a++)
		{
			if(p->moves[a]->type >= 0)
			{
				if(p->moves[a]->x == m->x && p->moves[a]->y == m->y)
				{
					return p->moves[a];
				}
			}
		}
		return NULL;
}
