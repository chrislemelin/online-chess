#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "clientview.h"
#include "display.h"


void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
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

    while(1)
  	{
  //    printf("Please enter the message: ");
      int num_readable = select(sockfd+1, &readfds, NULL,NULL, NULL);
      if(FD_ISSET(fileno(stdin),&readfds))
      {
        printf("write from keyboard");
        bzero(buffer,256);
        fgets(buffer,256,stdin);
    		n = write(sockfd,buffer,strlen(buffer));
        if (n < 0)
    			error("ERROR writing to socket");
      }
      if(FD_ISSET(sockfd,&readfds))
      {
    		bzero(buffer,256);
        n = read(sockfd,buffer,256);
        if (buffer[0] == 'b')
        {
          memmove(buffer, buffer+1, strlen(buffer));
          drawBoard(buffer);
        //  printf("\033[%d;%dH", 0,0);
          //break;
        }
        if (buffer[0] == 'm')
        {
          memmove(buffer, buffer+1, strlen(buffer));
          printf("%s\n",buffer);
        }

        if (buffer[0] == '0')
        {
          close(sockfd);
          printf("connection severed\n");
          break;
        }

       }
       FD_SET(fileno(stdin),&readfds);
       FD_SET(sockfd,&readfds);
     }
}
