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
	int fd0 = -5;
	int fd1 = -5;


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

	//struct board * b = malloc(sizeof(struct board));
	//initBoard(b);
	//updateAllMoves(b);


	while(1)
	{
		//drawBoard(b);

		printf("looking for keyboard\n");
		num_readable = select(sockfd+1+a_players , &readfds, NULL,NULL, NULL);
		printf("%d\n",sockfd+1+a_players);
		if (num_readable == -1)
			error("ERROR select failed");
		if(FD_ISSET(sockfd,&readfds))
		{
			newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
			if (newsockfd < 0)
				error("ERROR on accept");
			printf("connected to player %d: \n",s_players);

			sleep(1);

			bzero(buffer,256);

			if(addPlayer(players,&s_players,MAXP, newsockfd) > 0)
			{
				a_players++;
				printf("fd:%d\n", players[s_players-1]->fd);
				fflush(stdout);
				//players[s_players-1]->name = "test";
			}
			sendMessage(players[s_players-1]->fd,'m', "Enter Your User Name");

//			write(newsockfd,"connection esablished",30);

			if(fd0 <0)
			{
				fd0 = newsockfd;
			}
			else if(fd1 <0)
			{
				fd1 = newsockfd;
			}
		}
		if(FD_ISSET( fileno(stdin),&readfds))
		{
			printf("looking for keys");
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

			if(FD_ISSET(players[a]->fd, &readfds))
			{
				bzero(buffer,256);
				n = read(players[a]->fd,buffer,255);
				if (n < 0)
					error("ERROR reading from socket");
				if( strcmp(buffer,"quit\n") == 0)
				{
					removePlayer(players,&s_players,a);
					printf("closed connection on player %d\n",a);
					a--;
					continue;
				}
				if(players[a]->name == NULL)
				{
					char * tempS = malloc(20);
					strncpy(tempS,buffer,20);
					if(strcmp(tempS,"") !=0)
					{
						printf("name is %s size: %d\n",tempS,strlen(buffer));
						players[a]->name = tempS;
						char * temppy1 =playerListToString(players, s_players);
						printf("sent message :%s:\n",temppy1 );
						//sendMessage(players[a]->fd,'l',temppy1);
						sendMessageAll(players,s_players,'l',temppy1);
					}
					else
					{
						sendMessage(players[a]->fd,'m',"not a valid username");
					}
					continue;
				}

				if(players[a]->game == NULL)
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
							char temp[50] = "";
							strcat(temp,players[a]->name);
							strcat(temp," has challenege you, Y/N");
							if(players[a]->challengee != NULL)
							{
								players[a]->challengee->challenger = NULL;
								sendMessage(players[a]->challengee->fd,'m',"opponent has canceled request");
							}
							players[a]->challengee = opp;
							opp->challenger = players[a];
							printf("challenge %s to %s",players[a]->name, opp->name);
							sendMessage(opp->fd,'m',temp);
						}
						else
						{
							sendMessage(players[a]->fd,'m',"not a valid player");
						}
					}
					else if((strcasecmp(token,"Y") == 0 || strcasecmp(token,"yes") == 0)
						&& players[a]->challenger != NULL)
						{
							printf("making game\n" );
						 	startGame(players[a],players[a]->challenger);
							continue;
						}
					else if((strcasecmp(token,"N") == 0 || strcasecmp(token,"no") == 0)
						&& players[a]->challenger != 0)
						{
							sendMessage(players[a]->challenger->fd,'m',"play request denied");
							players[a]->challenger->challengee = NULL;
							players[a]->challenger = NULL;
						}


				}
				if(players[a]->game == NULL)
					continue;

				if(players[a]->game->currentPlayer != players[a]->playerGameId)
				{
				 sendMessage(players[a]->fd, 'm', "not your turn!!");
					continue;
				}

				if(players[a]->game->promotionPawn != NULL)
				{
					int action = promote(players[a]->game,buffer[0]);
					if(action)
					{
						players[a]->game->currentPlayer = (players[a]->game->currentPlayer+1)%2;
						bzero(buffer,256);
						char * temp = boardToString(players[a]->game);

						sendMessage(players[a]->fd, 'b', temp);
						if(players[a]->opp != NULL)
						{
								sendMessage(players[a]->opp->fd, 'b', temp);
						}
					}
					else
					{
						sendMessage(players[a]->fd, 'm', "not a valid promotion");
					}
					continue;
				}


				char *end;
				int x1 = strtol(buffer ,&end,10);
				int y1 = strtol(end ,&end,10);
				int x2 = strtol(end ,&end,10);
				int y2 = strtol(end ,&end,10);
				int t = 10;

				drawBoard(players[a]->game);

				bzero(buffer,256);

				t = tryMove(players[a]->game,x1,y1,x2,y2,players[a]->playerGameId);
	//			int t = tryMove (b,x1,y1,x2,y2,0);
				printf("Here is ta message from player %d-%d-%d-%d %d: \n",x1,y1,x2,y2,t);
				//sleep(5);

				bzero(buffer,256);
				if(t== 1 || t ==2)
				{
					char * temp = boardToString(players[a]->game);
	/*				strcat(buffer,"b");
					strcat(buffer,temp);
					strcat(buffer,"*");
					n = write(players[a]->fd,buffer,256);
*/
					sendMessage(players[a]->fd,'b', temp);

					if(players[a]->opp != NULL)
					{
						sendMessage(players[a]->fd, 'b', temp);

					}

					if(t==2)
					{
						sendMessage(players[a]->fd, 'm', "promote!!");
					}

					if (n < 0)
						error("ERROR writing to socket");
					bzero(buffer,256);

					int winner  = whoWon(players[a]->game);
					if(winner == -1)
					{
						if(players[a]->opp != NULL)
						{
								sendMessage(players[a]->opp->fd, 'm', "your turn!!");
						}
					}

					else if(winner == 2)
					{
						sendMessage(players[a]->fd, 'm', "Tie!!!");
						if(players[a]->opp != NULL)
							sendMessage(players[a]->opp->fd, 'm', "Tie!!!");
					}
					else if(winner == players[a]->playerGameId)
					{
						sendMessage(players[a]->fd, 'm', "You Win!!!");
						if(players[a]->opp != NULL)
						{
							sendMessage(players[a]->opp->fd, 'm', "You Lose!!!");
						}
					}
					else
					{
						sendMessage(players[a]->fd, 'm', "You Lose!!!");
						if(players[a]->opp != NULL)
						{
							sendMessage(players[a]->opp->fd, 'm', "You Win!!!");
						}
					}


				}
				if(t== -1)
				{
					sendMessage(players[a]->fd, 'm', "Invalid First Coordinate");
				}
				if(t== -2)
				{
					sendMessage(players[a]->fd, 'm', "Invalid Second Coordinate");

				}
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
/*
int disconnect(int * list, int a,int * s_players )
{
	write(list[a],"0*",2);
	close(list[a]);
	for(int c = a+1; c< *s_players ;c++)
	{
		printf("c:%d \n",list[c]);
		list[c-1] = list[c];
	}
	*s_players = *s_players-1;
}
*/

int sendMessage(int fd, char type, char * message)
{
	char buffer[256];
	buffer[0] = type;
	buffer[1] = '\0';
	strcat(buffer,message);
	int temp = strlen(buffer);
	buffer[temp] = EOM;
	buffer[temp+1] = '\0';
	write(fd,buffer,256);
}

int sendMessageAll(struct player ** players, int s_players, char type, char * message)
{
	for(int a = 0; a< s_players;a++)
	{
		sendMessage(players[a]->fd,type,message);
	}
}
