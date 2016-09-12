#ifndef PLAYER_H
#define PLAYER_H

#define EON (char)11

struct player
{
	char * name;
	struct board * game;
	int fd;
	int playerGameId;
  struct player * opp;
	struct player * challenger;
	struct player * challengee;
}player;

int addPlayer(struct player ** players,int  * s_players, int max_players, int fd);
int removePlayer(struct player ** players, int * s_players, int a);
struct player * findPlayer(struct player ** players, int s_players,char *name );
void startGame(struct player * p1 , struct player * p2);
int validName (struct player ** players, int s_players,char *name);
char* playerListToString(struct player ** players, int s_players);


#endif
