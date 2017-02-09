#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/select.h>

#include "chess/piece.h"
#include "chess/display.h"
#include "chess/board.h"
#include "player.h"
#include "server.h"

#define _POSIX_C_SOURCE 1

const int WAIT = 50;
const int MAXP = 10;

#define EOM (char)10


void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{

	struct player ** players = malloc (sizeof(struct player) *MAXP);
	int s_players = 0;
	int a_players = 0;

	int num_readable;
	int fd_stdin = fileno(stdin);

	fd_set readfds;
	FD_SET(fileno(stdin),&readfds);

	int sockfd, newsockfd, portno;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	      error("ERROR on binding");
	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	FD_SET(sockfd,&readfds);

	//main while loop
	while(1)
	{
		printf("started server\n");
		num_readable = select(sockfd+1+a_players , &readfds, NULL,NULL, NULL);
		if (num_readable == -1)
			error("ERROR select failed");

		//new connection
		if(FD_ISSET(sockfd,&readfds))
		{
			newSocket(sockfd, sockfd,clilen, &s_players, &a_players,
			players);
		}

		//read input from keyboard
		if(FD_ISSET( fileno(stdin),&readfds))
		{
			fflush(stdout);
			bzero((char*) &buffer, 255);
			n = read(fd_stdin,buffer,255);
			if (strcmp(buffer,"q\n") == 0)
			{
				printf("Exited Correctly\n");
				for(int a = 0; a<s_players;a++)
				{
					sendMessage(players[a]->fd, '0', "");
				}
				close(sockfd);
				return 0;
			}
			printf("not valid command enter q to exit\n%s_%d\n",buffer,fd_stdin);
			fflush(stdout);
		}
		for(int a = 0; a<s_players;a++)
		{
			struct player * c_player = players[a];
			if(FD_ISSET(c_player->fd, &readfds))
			{
				processMessage(c_player, &s_players,&a_players,&a,players);

				/*
				bzero(buffer,256);
				n = read(c_player->fd,buffer,255);
				if (n < 0)
					error("ERROR reading from socket");
				if( strcmp(buffer,"quit") == 0)
				{
					removePlayer(players,&s_players,a);
					printf("closed connection on player %d\n",a);
					a--;
					continue;
				}
				if(strcmp(c_player->name, "") == 0)
				{
					char * tempS = malloc(20);
					strncpy(tempS,buffer,20);
					//printf("tempS: %s\n",tempS);
					fflush(stdout);
					if(strcmp(tempS,"") !=0 && validName(players , s_players, tempS))
					{
						printf("name is %s size: %ld\n",tempS,strlen(buffer));

						c_player->name = tempS;
						char * temppy1 =playerListToString(players, s_players);
						//printf("sent message :%s:\n",temppy1 );
						sendMessage(c_player->fd,'a',"");
						sendMessageAll(players,s_players,'l',temppy1);
					}
					else
					{
						sendMessage(c_player->fd,'m',"not a valid username");
					}
					continue;
				}

				if(c_player->game == NULL)
				{
					char tempS[2];
					tempS[0] = ' ';
					tempS[1] = '\0';
					char * token;
					token = strtok(buffer,tempS);
					if(strcmp(token,"play")==0)
					{
						token = strtok(0,tempS);
						struct player * opp = findPlayer(players,s_players,token);
						if(opp!=NULL)
						{
							if(opp == c_player)
							{
								sendMessage(c_player->fd,'m',"you cannot challenege yourself");
								continue;
							}
							if(opp->game != NULL)
							{
								sendMessage(c_player->fd,'m',"that player is busy");
								continue;
							}
							if(c_player->challenger != NULL)
							{
								sendMessage(c_player->fd,'m',"You already have a challenge pending");
								continue;
							}

							char temp[50] = "";
							strcat(temp,c_player->name);
							strcat(temp," has challenege you, Y/N");
							if(c_player->challengee != NULL)
							{
								c_player->challengee->challenger = NULL;
								sendMessage(c_player->challengee->fd,'m',"opponent has canceled request");
							}
							c_player->challengee = opp;
							opp->challenger = c_player;
							printf("challenge %s to %s",c_player->name, opp->name);
							sendMessage(opp->fd,'m',temp);
						}
						else
						{
							sendMessage(c_player->fd,'m',"not a valid player");
						}
					}
					else if((strcasecmp(token,"Y") == 0 || strcasecmp(token,"yes") == 0)
						&& c_player->challenger != NULL)
						{
							printf("making game\n" );
						 	startGame(c_player,c_player->challenger);
							sendMessageAll(players,s_players,'l',playerListToString(players,s_players));
							continue;
						}
					else if((strcasecmp(token,"N") == 0 || strcasecmp(token,"no") == 0)
						&& c_player->challenger != 0)
						{
							sendMessage(c_player->challenger->fd,'m',"play request denied");
							c_player->challenger->challengee = NULL;
							c_player->challenger = NULL;
						}
				}
				if(c_player->game == NULL)
					continue;

				if(c_player->game->currentPlayer != c_player->playerGameId)
				{
				 sendMessage(c_player->fd, 'm', "not your turn!!");
					continue;
				}

				if(c_player->game->promotionPawn != NULL)
				{
					int action = promote(c_player->game,buffer[0]);
					if(action)
					{
						c_player->game->currentPlayer = (c_player->game->currentPlayer+1)%2;
						bzero(buffer,256);
						char * temp = boardToString(c_player->game);

						sendMessage(c_player->fd, 'b', temp);
						if(c_player->opp != NULL)
						{
								sendMessage(c_player->opp->fd, 'b', temp);
						}
					}
					else
					{
						sendMessage(c_player->fd, 'm', "not a valid promotion");
					}
					continue;
				}

				char *end;
				int x1 = strtol(buffer ,&end,10);
				int y1 = strtol(end ,&end,10);
				int x2 = strtol(end ,&end,10);
				int y2 = strtol(end ,&end,10);
				int t = 10;

				drawBoard(c_player->game);

				t = tryMoveAdapter(c_player->game,buffer,c_player->playerGameId);
				printf("Here is ta message from player %d-%d-%d-%d %d: \n",x1,y1,x2,y2,t);

				bzero(buffer,256);
				if(t== 1 || t ==2)
				{
					char * temp = boardToString(c_player->game);
					sendMessage(c_player->fd,'b', temp);
					if(c_player->opp != NULL)
					{
						sendMessage(c_player->opp->fd, 'b', temp);
					}
					if(t==2)
					{
						sendMessage(c_player->fd, 'm', "promote!!");
					}
					if (n < 0)
						error("ERROR writing to socket");
					bzero(buffer,256);

					int winner  = whoWon(c_player->game);
					if(winner == -1)
					{
						if(c_player->opp != NULL)
						{
								sendMessage(c_player->opp->fd, 'm', "your turn!!");
						}
					}
					else if(winner == 2)
					{
						sendMessage(c_player->fd, 'm', "Tie!!!");
						if(c_player->opp != NULL)
							sendMessage(c_player->opp->fd, 'm', "Tie!!!");
					}
					else if(winner == c_player->playerGameId)
					{
						sendMessage(c_player->fd, 'm', "You Win!!!");
						if(c_player->opp != NULL)
						{
							sendMessage(c_player->opp->fd, 'm', "You Lose!!!");
						}
					}
					else
					{
						sendMessage(c_player->fd, 'm', "You Lose!!!");
						if(c_player->opp != NULL)
						{
							sendMessage(c_player->opp->fd, 'm', "You Win!!!");
						}
					}

				}
				if(t== -1)
				{
					sendMessage(c_player->fd, 'm', "Invalid First Coordinate");
				}
				if(t== -2)
				{
					sendMessage(c_player->fd, 'm', "Invalid Second Coordinate");

				}
				*/
			}
		}
		FD_ZERO(&readfds);
		FD_SET(fileno(stdin),&readfds);
		FD_SET(sockfd,&readfds);
		for(int a = 0 ; a<s_players;a++)
		{
			printf("a :%d ",players[a]->fd);
			fflush(stdout);
			FD_SET(players[a]->fd, &readfds);
		}
	}
}

void newSocket(int newsockfd, int sockfd,socklen_t clilen,
	 int* s_players, int* a_players,struct player** players)
{
	struct sockaddr_in serv_addr, cli_addr;
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0)
		error("ERROR on accept");
	printf("connected to player %d: \n",*s_players);

	sleep(1);

	if(addPlayer(players,s_players,MAXP, newsockfd) > 0)
	{
		*a_players = *a_players+1;
		printf("fd:%d\n", players[*s_players-1]->fd);
		fflush(stdout);
	}
	sendMessage(players[*s_players-1]->fd,'m', "Enter Your User Name");

}

void processMessage(struct player* c_player, int* s_players, int* a_players, int* a,
	struct player ** players)
{
	char buffer[256];
	bzero(buffer,256);
	int n = read(c_player->fd,buffer,255);
	if (n < 0)
		error("ERROR reading from socket");
	if( strcmp(buffer,"quit") == 0)
	{
		removePlayer(players,s_players,*a);
		printf("closed connection on player %d\n",*a);
		*a = *a-1;
		return;
	}
	if(strcmp(c_player->name, "") == 0)
	{
		char * tempS = malloc(20);
		strncpy(tempS,buffer,20);
		//printf("tempS: %s\n",tempS);
		fflush(stdout);
		if(strcmp(tempS,"") !=0 && validName(players , *s_players, tempS))
		{
			printf("name is %s size: %ld\n",tempS,strlen(buffer));

			c_player->name = tempS;
			char * temppy1 =playerListToString(players, *s_players);
			//printf("sent message :%s:\n",temppy1 );
			sendMessage(c_player->fd,'a',"");
			sendMessageAll(players,*s_players,'l',temppy1);
		}
		else
		{
			sendMessage(c_player->fd,'m',"not a valid username");
		}
		return;
	}

	if(c_player->game == NULL)
	{
		char tempS[2];
		tempS[0] = ' ';
		tempS[1] = '\0';
		char * token;
		token = strtok(buffer,tempS);
		if(strcmp(token,"play")==0)
		{
			token = strtok(0,tempS);
			struct player * opp = findPlayer(players,*s_players,token);
			if(opp!=NULL)
			{
				if(opp == c_player)
				{
					sendMessage(c_player->fd,'m',"you cannot challenege yourself");
					return;
				}
				if(opp->game != NULL)
				{
					sendMessage(c_player->fd,'m',"that player is busy");
					return;
				}
				if(c_player->challenger != NULL)
				{
					sendMessage(c_player->fd,'m',"You already have a challenge pending");
					return;
				}

				char temp[50] = "";
				strcat(temp,c_player->name);
				strcat(temp," has challenege you, Y/N");
				if(c_player->challengee != NULL)
				{
					c_player->challengee->challenger = NULL;
					sendMessage(c_player->challengee->fd,'m',"opponent has canceled request");
				}
				c_player->challengee = opp;
				opp->challenger = c_player;
				printf("challenge %s to %s",c_player->name, opp->name);
				sendMessage(opp->fd,'m',temp);
			}
			else
			{
				sendMessage(c_player->fd,'m',"not a valid player");
			}
		}
		else if((strcasecmp(token,"Y") == 0 || strcasecmp(token,"yes") == 0)
			&& c_player->challenger != NULL)
			{
				printf("making game\n" );
				startGame(c_player,c_player->challenger);
				sendMessageAll(players,*s_players,'l',playerListToString(players,*s_players));
				return;
			}
		else if((strcasecmp(token,"N") == 0 || strcasecmp(token,"no") == 0)
			&& c_player->challenger != 0)
			{
				sendMessage(c_player->challenger->fd,'m',"play request denied");
				c_player->challenger->challengee = NULL;
				c_player->challenger = NULL;
			}
	}
	if(c_player->game == NULL)
		return;

	if(c_player->game->currentPlayer != c_player->playerGameId)
	{
	 sendMessage(c_player->fd, 'm', "not your turn!!");
		return;
	}

	if(c_player->game->promotionPawn != NULL)
	{
		int action = promote(c_player->game,buffer[0]);
		if(action)
		{
			c_player->game->currentPlayer = (c_player->game->currentPlayer+1)%2;
			bzero(buffer,256);
			char * temp = boardToString(c_player->game);

			sendMessage(c_player->fd, 'b', temp);
			if(c_player->opp != NULL)
			{
					sendMessage(c_player->opp->fd, 'b', temp);
			}
		}
		else
		{
			sendMessage(c_player->fd, 'm', "not a valid promotion");
		}
		return;
	}

	char *end;
	int x1 = strtol(buffer ,&end,10);
	int y1 = strtol(end ,&end,10);
	int x2 = strtol(end ,&end,10);
	int y2 = strtol(end ,&end,10);
	int t = 10;

	drawBoard(c_player->game);

	t = tryMoveAdapter(c_player->game,buffer,c_player->playerGameId);
	printf("Here is ta message from player %d-%d-%d-%d %d: \n",x1,y1,x2,y2,t);

	bzero(buffer,256);
	if(t== 1 || t ==2)
	{
		char * temp = boardToString(c_player->game);
		sendMessage(c_player->fd,'b', temp);
		if(c_player->opp != NULL)
		{
			sendMessage(c_player->opp->fd, 'b', temp);
		}
		if(t==2)
		{
			sendMessage(c_player->fd, 'm', "promote!!");
		}
		if (n < 0)
			error("ERROR writing to socket");
		bzero(buffer,256);

		int winner  = whoWon(c_player->game);
		if(winner == -1)
		{
			if(c_player->opp != NULL)
			{
					sendMessage(c_player->opp->fd, 'm', "your turn!!");
			}
		}
		else if(winner == 2)
		{
			sendMessage(c_player->fd, 'm', "Tie!!!");
			if(c_player->opp != NULL)
				sendMessage(c_player->opp->fd, 'm', "Tie!!!");
		}
		else if(winner == c_player->playerGameId)
		{
			sendMessage(c_player->fd, 'm', "You Win!!!");
			if(c_player->opp != NULL)
			{
				sendMessage(c_player->opp->fd, 'm', "You Lose!!!");
			}
		}
		else
		{
			sendMessage(c_player->fd, 'm', "You Lose!!!");
			if(c_player->opp != NULL)
			{
				sendMessage(c_player->opp->fd, 'm', "You Win!!!");
			}
		}

	}
	if(t== -1)
	{
		sendMessage(c_player->fd, 'm', "Invalid First Coordinate");
	}
	if(t== -2)
	{
		sendMessage(c_player->fd, 'm', "Invalid Second Coordinate");

	}
}

int sendMessage(int fd, char type, char * message)
{
	char buffer[256];
	buffer[0] = type;
	buffer[1] = '\0';
	strcat(buffer,message);
	int temp = strlen(buffer);
	buffer[temp] = EOM;
	buffer[temp+1] = '\0';
	int n =write(fd,buffer,256);
	printf("n : %d message : %s",n,message);
	if(n <= 0)
	{
		printf("dissconedted??\n");
	}
	return 0;
}

int sendMessageAll(struct player ** players, int s_players, char type, char * message)
{
	for(int a = 0; a< s_players;a++)
	{
		sendMessage(players[a]->fd,type,message);
	}
	return 0;
}
