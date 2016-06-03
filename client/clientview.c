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

#define EON (char)11

#define BOARD_LENGTH 8
#define BOARD_WIDTH 8
#define DISPLAY_X 2
#define DISPLAY_Y 2

#define BOARD_HOR '_'
#define BOARD_VERT '|'
#define BOARD_CORN '+'

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

int drawBoard(char * data)
{
	clear();
	set_cur_pos(0,0);
	for (int x = 0 ; x < BOARD_WIDTH; x++)
	{
		for(int y = 0 ; y < BOARD_LENGTH;y++)
		{
			set_cur_pos(y*2+DISPLAY_Y+2, 4*x+DISPLAY_X);
			put(BOARD_VERT);
			set_cur_pos(y*2+DISPLAY_Y+1, 4*x+DISPLAY_X);
			put(BOARD_VERT);
			set_cur_pos(y*2+DISPLAY_Y, 4*x+DISPLAY_X);
			put(BOARD_VERT);
			set_cur_pos(y*2+DISPLAY_Y, 4*x+DISPLAY_X+1);
			put(BOARD_HOR);
			set_cur_pos(y*2+DISPLAY_Y, 4*x+DISPLAY_X+2);
			put(BOARD_HOR);
			set_cur_pos(y*2+DISPLAY_Y, 4*x+DISPLAY_X+3);
			put(BOARD_HOR);

		}
		set_cur_pos(BOARD_LENGTH*2+DISPLAY_Y,4*x+DISPLAY_X);
		put(BOARD_HOR);
		set_cur_pos(BOARD_LENGTH*2+DISPLAY_Y,4*x+DISPLAY_X+1);
		put(BOARD_HOR);
		set_cur_pos(BOARD_LENGTH*2+DISPLAY_Y,4*x+DISPLAY_X+2);
		put(BOARD_HOR);
		set_cur_pos(BOARD_LENGTH*2+DISPLAY_Y,4*x+DISPLAY_X+3);
		put(BOARD_HOR);
		set_cur_pos(DISPLAY_Y,4*x+DISPLAY_X);
		put(BOARD_HOR);

		set_cur_pos(x*2+DISPLAY_Y,BOARD_WIDTH*4+DISPLAY_X);
		put(BOARD_VERT);
		set_cur_pos(x*2+1+DISPLAY_Y,BOARD_WIDTH*4+DISPLAY_X);
		put(BOARD_VERT);

		set_cur_pos(DISPLAY_Y-1,4*x+DISPLAY_X+2);
		put(x+'0');
		set_cur_pos(x*2+1+DISPLAY_Y,DISPLAY_X+1+BOARD_WIDTH*4);
		put(x+'0');


		set_cur_pos(DISPLAY_Y,DISPLAY_X);
		put(BOARD_CORN);
		set_cur_pos(DISPLAY_Y+BOARD_LENGTH*2,DISPLAY_X);
		put(BOARD_CORN);
		set_cur_pos(DISPLAY_Y,DISPLAY_X+BOARD_WIDTH*4);
		put(BOARD_CORN);
		set_cur_pos(DISPLAY_Y+BOARD_LENGTH*2,DISPLAY_X+BOARD_WIDTH*4);
		put(BOARD_CORN);
	}
	int a = 0;
	while(1)
	{
		int x = data[a*4+2] - '0';
		int y = data[a*4+3] - '0';
		int player = data[a*4+1];

		set_cur_pos(y*2+DISPLAY_Y+1,x*4 +DISPLAY_X+1);
		if(player == '0')
			printf(BLUE);
		else
			printf(RED);
		put(data[a*4]);
		printf(RESET);
		if(data[a*4+4]=='\0')
			break;
		a++;
	}
}

int printMessage(char * message)
{
	set_cur_pos(18+DISPLAY_Y,0);
	printf(CLEARLINE);
	set_cur_pos(18+DISPLAY_Y,0);
  printf("%s\n", message);
}
int clearInput()
{
	set_cur_pos(19+DISPLAY_Y,0);
	printf(CLEARLINE);
	set_cur_pos(19+DISPLAY_Y,0);
	put('~');
}

int drawLobby()
{
	clear();
	set_cur_pos(DISPLAY_Y,DISPLAY_X);
	printf("LOBBY:\n");

	char tempS[2];
	tempS[0] = EON;
	tempS[1] = '\0';

	char * delim = tempS;
	char * token;
	token = strtok(lobby,delim);
	int a = 0;
	while(token)
	{

		if(token[0] == '0')
			printf(BLUE);
		else
			printf(RED);
		memmove(token, token+1, strlen(token));
		set_cur_pos(a+DISPLAY_Y+1, DISPLAY_X);
		if(strlen(token) !=0)
			printf("%s\n",token);
		token = strtok(0,delim);
		a++;
	}
	printf(RESET);
}

int updateLobby(char * nLobby)
{
  lobby = nLobby;
}
int updateBoard(char * nBoard)
{
	board = nBoard;
}
