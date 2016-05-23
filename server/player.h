#ifndef PLAYER_H
#define PLAYER_H

struct player
{
	struct board * game;
	int fd;
	int playerGameId;
  struct player * opp;
}player;

int addPlayer(struct player ** players,int  * s_players, int max_players, int fd);



#endif
