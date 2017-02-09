#ifndef SERVER_H
#define SERVER_H


int disconnect(int * list, int a,int * s_players );
int sendMessage(int fd, char type, char * message);
int sendMessageAll(struct player ** players, int s_players, char type, char * message);
void newSocket(int newsockfd, int sockfd,socklen_t clilen,
	int* s_players, int* a_players,struct player** players);

void processMessage(struct player* c_player, int* s_players, int* a_players, int* a,
  struct player ** players);

#endif
