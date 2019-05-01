#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include "Snake.h"
#include "Score.h"
#include "Food.h"
#include <sys/types.h>
#include <sys/time.h>

WINDOW *initCurses()
{
	initscr();
	nodelay(stdscr, true);
	keypad(stdscr, true);
	noecho();
	cbreak();
	curs_set(0);
	start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_RED);
	init_pair(3, COLOR_RED, COLOR_GREEN);
	return stdscr;
}

int main()
{
	WINDOW* stdscr = initCurses();
	Snake snake = initSnake(stdscr, 3);
	setSegment(&snake, 0, 10, 3);
	setSegment(&snake, 1, 10, 2);
	setSegment(&snake, 2, 10, 1);
	const uint8_t MPS = 20;
	const uint8_t FPMS = 1000/MPS;
	struct timeval tv;
	uint64_t start, end;
	int16_t move = KEY_DOWN;
	uint16_t score = 0;
	Food food = initFood(stdscr, 3);
	for(int i = 0; i < 3; i++)
		setFoodRand(&food, i);
	gettimeofday(&tv, NULL);
	start = tv.tv_sec * 1000 + tv.tv_usec/1000;
	while(1)
	{
		gettimeofday(&tv, NULL);
		end = tv.tv_sec * 1000 + tv.tv_usec/1000;
		if(end - start >= FPMS)
		{
			int16_t c = getch();
			if(c == 'q')
				break;
			if(c == '1')
				incrementSnake(&snake);
			if(c != ERR)
				switch(c)
				{
					case KEY_DOWN:
						move = (move != KEY_UP) ? KEY_DOWN : KEY_UP;
						break;
					case KEY_UP:
						move = (move != KEY_DOWN) ? KEY_UP : KEY_DOWN;
						break;
					case KEY_LEFT:
						move = (move != KEY_RIGHT) ? KEY_LEFT : KEY_RIGHT;
						break;
					case KEY_RIGHT:
						move = (move != KEY_LEFT) ? KEY_RIGHT : KEY_LEFT;
						break;
				}
			moveSnake(&snake, move);
			drawSnake(&snake);
			eraseFood(&food);
			drawFood(&food);
			int16_t foodCollision = isIn(&food, snake.body[0].x, snake.body[0].y);
			if(foodCollision != -1)
			{
				setFoodRand(&food, foodCollision);
				incrementSnake(&snake);
				score++;
			}
			if(collideSnake(&snake, 0))
			{
				break;
			}
			start = end;
		}
	}
	freeSnake(&snake);
	endwin();
}
