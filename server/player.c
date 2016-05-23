#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "chess/board.h"
#include "player.h"


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
    printf("making board\n");
    fflush(stdout);
    initBoard(b);
    printf("made board\n");
    fflush(stdout);
    temp->game = b;
    temp->playerGameId = 0;
  }
  players[*s_players] = temp;
  *s_players = *s_players+1;
  return 1;
}
