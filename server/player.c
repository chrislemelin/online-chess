#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "chess/board.h"
#include "player.h"
#include "server.h"

#define EON (char)11

/*  returns -1 if list is full
 *  returns 1 if successful
 *
 */
int addPlayer(struct player ** players,int  * s_players, int max_players, int fd)
{
  if(*s_players == max_players)
  {
    return -1;
  }
  struct player * temp = malloc(sizeof(struct player));
  temp->fd = fd;
  temp->game = NULL;
  temp->opp = (struct player *)NULL;
  temp->playerGameId = -1;
  temp->name = NULL;
  /*
  for(int a = 0;a < *s_players;a++)
  {
    if(players[a]->opp == NULL)
    {
      temp->opp = players[a];
      players[a]->opp = temp;
      temp->game = players[a]->game;
      temp->playerGameId = (players[a]->playerGameId +1)%2;
      break;
    }
  }
  if(temp->game == NULL)
  {
    struct board * b = malloc(sizeof(struct board));
    initBoard(b);
    temp->game = b;
    temp->playerGameId = 0;
  }
  */
  players[*s_players] = temp;
  *s_players = *s_players+1;
  return 1;
}

int removePlayer(struct player ** players, int * s_players, int a)
{
	write(players[a]->fd,"0*",2);
	close(players[a]->fd);
  if(players[a]->opp == NULL)
  {
    free(players[a]->game);
  }
  else
  {
    players[a]->opp->opp = NULL;
  }
  free(players[a]);
	for(int c = a+1; c< *s_players ;c++)
	{
		players[c-1] = players[c];
	}
	*s_players = *s_players-1;
}

char* playerListToString(struct player ** players, int s_players)
{
  char * tempS = calloc(25*s_players,1);
  for (int a= 0 ; a < s_players; a++)
  {
    char t1 [2];
    t1[1] = '\0';
    if(players[a]->game == NULL)
    {
      t1[0] = '0';
    }
    else
      t1[0] = '1';
    strcat(tempS,t1);

    if(players[a]->name != NULL)
    {
      strcat(tempS, players[a]->name);
      printf("players coppied:%s %d\n ", players[a]->name,players[a]->fd);
    }
    printf("current message : %s\n",tempS );
    int insert = strlen(tempS);
    tempS[insert] = EON;
    tempS[insert+1] = '\0';
  }
  return tempS;
}

struct player * findPlayer(struct player ** players, int s_players,char *name )
{
  struct player * temp = NULL;
  for(int a ; a< s_players; a++)
  {
    if(strcmp(name, players[a]->name)==0)
    {
      temp = players[a];
    }
  }
  return temp;
}

void startGame(struct player * p1 , struct player * p2)
{
  struct board * b = malloc(sizeof(struct board));
  initBoard(b);
  p1->game = b;
  p1->playerGameId = 0;
  p2->game = b;
  p2->playerGameId = 1;
  p1->opp = p2;
  p2->opp = p1;
  sendMessage(p1->fd,'g',"");
  sendMessage(p2->fd,'g',"");
  char * temp = boardToString(b);
  sendMessage(p1->fd,'b',temp);
  sendMessage(p2->fd,'b',temp);
  sendMessage(p1->fd,'m',"your turn!!");
}
