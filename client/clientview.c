#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "clientview.h"
#include "display.h"

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

	set_cur_pos(20,0);
	put('~');
}
