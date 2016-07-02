#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>


#include "clientview.h"
#include "display.h"

const char EOM = (char)10;
int quit = 0;

void signalHandler(int input)
{
    printf("%d\n",input );
    quit = 1;
}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int run(int sockfd, fd_set readfds, int * inGame)
{
  char buffer[256];
  int n;

  int num_readable = select(sockfd+1, &readfds, NULL,NULL, NULL);
  if(FD_ISSET(fileno(stdin),&readfds))
  {
  if(quit)
  {
    n = write(sockfd,&"quit",6);
    printf("closed connection\n");
    return -1;

  }
  else
  {
    bzero(buffer,256);
    fgets(buffer,256,stdin);
    buffer[strlen(buffer)-1] = '\0';
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0)
      error("ERROR writing to socket");
  }
  }
  if(FD_ISSET(sockfd,&readfds))  //    printf("Please enter the message: ");

  {
  bzero(buffer,256);
  n = read(sockfd,buffer,256);

  char tempS[2];
  tempS[0] = EOM;
  tempS[1] = '\0';

  char * delim = tempS;
  char * token;
  token = strtok(buffer,delim);
  while(token)
  {
    //printf("%s\n",token );
    if (token[0] == 'b')
    {
      memmove(token, token+1, strlen(token));
      updateBoard(token);
      if(* inGame)
      {
        drawBoard();
      }
    }
    else if (token[0] == 'm')
    {
      memmove(token, token+1, strlen(token));
      printMessage(token);
    }
    else if (token[0] == 'l')
    {
      memmove(token, token+1, strlen(token));
      updateLobby(token);
      //drawLobby();
      if(! (*inGame))
      {
        drawLobby();
      }

    }
    else if (token[0] == '0')
    {
      close(sockfd);
      printMessage("connection closed");
      return -1;
    }
    else if (token[0] == 'g')
    {
      * inGame = 1;
    }
    else if (token[0] == 'd')
    {
      * inGame = 0;
    }
    token = strtok(0,delim);
  }
  }
  clearInput();
  FD_SET(fileno(stdin),&readfds);
  FD_SET(sockfd,&readfds);
  return 0;
}


int main(int argc, char *argv[])
{

    signal(SIGINT, signalHandler);
    signal(SIGTSTP, signalHandler);
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    /*
    n = read(sockfd,buffer,256);
    printf("--%s\n",buffer);
    */

    fd_set readfds;
    FD_SET(fileno(stdin),&readfds);
    FD_SET(sockfd,&readfds);

    int inGame = 0;

    while(1)
  	{
      int a = run(sockfd, readfds,&inGame);
      if(a)
        return a;
     }

}
