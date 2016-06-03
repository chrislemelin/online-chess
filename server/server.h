#ifndef SERVER_H
#define SERVER_H


int disconnect(int * list, int a,int * s_players );
int sendMessage(int fd, char type, char * message);
int sendMessageAll(struct player ** players, int s_players, char type, char * message);





#endif
