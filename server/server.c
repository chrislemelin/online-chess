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

#define _POSIX_C_SOURCE 1

const int WAIT = 50;
const int MAXP = 10;

int disconnect(int * list, int a,int * c_connections );


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
	int c_connections = 0;
	int a_connections = 0;

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
		num_readable = select(sockfd+1+a_connections , &readfds, NULL,NULL, NULL);
		printf("%d\n",sockfd+1+a_connections);
		if (num_readable == -1)
			error("ERROR select failed");
		if(FD_ISSET(sockfd,&readfds))
		{
			newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
			if (newsockfd < 0)
				error("ERROR on accept");
			printf("connected to player %d: \n",c_connections);

			sleep(1);

			bzero(buffer,256);

			if(addPlayer(players,&c_connections,MAXP, newsockfd) > 0)
			{
				a_connections++;
				printf("fd:%d\n", players[c_connections-1]->fd);
				fflush(stdout);
			}
			char * temp = boardToString(players[c_connections-1]->game);
			strcat(buffer,"b");
			strcat(buffer,temp);
			strcat(buffer,"*");
			write(newsockfd,buffer,256);

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
				for(int a = 0; a<c_connections;a++)
				{
					n = write(players[a]->fd,"0*",2);
				}
				close(sockfd);
				return 0;
			}
			printf("not valid command enter q to exit\n%s_%d\n",buffer,fd_stdin);
			fflush(stdout);
		}
		for(int a = 0; a<c_connections;a++)
		{

			if(FD_ISSET(players[a]->fd, &readfds))
			{
				bzero(buffer,256);
				n = read(players[a]->fd,buffer,255);
				if (n < 0)
					error("ERROR reading from socket");
				if( strcmp(buffer,"quit\n") == 0)
				{
					//disconnect(connections, a,&c_connections);
					printf("closed connection on player %d",a);
					sleep(5);
					a--;
					continue;
				}
				if(players[a]->game->currentPlayer != players[a]->playerGameId)
				{
					bzero(buffer,256);
					strcat(buffer,"m");
					strcat(buffer,"not your turn!!*");
					n = write(players[a]->fd,buffer,256);
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
						strcat(buffer,"b");
						strcat(buffer,temp);
						strcat(buffer,"*");

						n = write(players[a]->fd,buffer,256);
						if(players[a]->opp != NULL)
						{
								n = write(players[a]->opp->fd,buffer,256);
						}
					}
					else
					{
						printf("not a valid promotion*\n");
						bzero(buffer,256);
						strcat(buffer,"m");
						strcat(buffer,"not a valid promotion*");
						n = write(players[a]->fd,buffer,256);
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
					strcat(buffer,"b");
					strcat(buffer,temp);
					strcat(buffer,"*");

					n = write(players[a]->fd,buffer,256);
					if(players[a]->opp != NULL)
					{
						n = write(players[a]->opp->fd,buffer,256);
					}

					if(t==2)
					{
						bzero(buffer,256);
						strcat(buffer,"m");
						strcat(buffer,"promote!!*");
						n = write(players[a]->fd,buffer,256);
					}

					if (n < 0)
						error("ERROR writing to socket");
					bzero(buffer,256);

					int winner  = whoWon(players[a]->game);
					if(winner == -1)
					{
						if(players[a]->opp != NULL)
						{
								strcat(buffer,"m");
								strcat(buffer,"your turn!!*");
								n = write(players[a]->opp->fd,buffer,256);
						}
					}

					else if(winner == 2)
					{
						strcat(buffer,"m");
						strcat(buffer,"tie!!*");
						n = write(players[a]->fd,buffer,256);
						if(players[a]->opp != NULL)
							n = write(players[a]->opp->fd,buffer,256);
					}
					else if(winner == players[a]->playerGameId)
					{
						strcat(buffer,"m");
						strcat(buffer,"you win!!*");
						n = write(players[a]->fd,buffer,256);
						bzero(buffer,256);
						if(players[a]->opp != NULL)
						{
							strcat(buffer,"m");
							strcat(buffer,"you lose!!*");
							n = write(players[a]->opp->fd,buffer,256);
						}
					}
					else
					{
						strcat(buffer,"m");
						strcat(buffer,"you lose!!*");
						n = write(players[a]->fd,buffer,256);
						bzero(buffer,256);
						if(players[a]->opp != NULL)
						{
							strcat(buffer,"m");
							strcat(buffer,"you win!!*");
							n = write(players[a]->opp->fd,buffer,256);
						}
					}


				}
				if(t== -1)
				{
					strcat(buffer,"m");
					strcat(buffer,"invalid first coordinate*");
					n = write(players[a]->fd,buffer,256);
				}
				if(t== -2)
				{
					strcat(buffer,"m");
					strcat(buffer,"invalid second coordinate*");
					n = write(players[a]->fd,buffer,256);
				}
				if(t== -3)
				{
					strcat(buffer,"m");
					strcat(buffer,"not your turn*");
					n = write(players[a]->fd,buffer,256);
				}
			}
		}
		FD_ZERO(&readfds);
		FD_SET(fileno(stdin),&readfds);
		FD_SET(sockfd,&readfds);
		for(int a = 0 ; a<c_connections;a++)
		{
			printf("a :%d ",players[a]->fd);
			fflush(stdout);
			FD_SET(players[a]->fd, &readfds);
		}
	}
}

int disconnect(int * list, int a,int * c_connections )
{
	write(list[a],"0*",2);
	close(list[a]);
	for(int c = a+1; c< *c_connections ;c++)
	{
		printf("c:%d \n",list[c]);
		list[c-1] = list[c];
	}
	*c_connections = *c_connections-1;
}
