#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include "Snake.h"
#include "Score.h"
#include "Food.h"
#include <sys/types.h>
#include <sys/time.h>

/* Method to initialize NCurses for the program */
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
	/* Init NCurses */
	WINDOW* stdscr = initCurses();
	
	/* Init Snake */
	Snake snake = initSnake(stdscr, 3);
	setSegment(&snake, 0, 10, 3);
	setSegment(&snake, 1, 10, 2);
	setSegment(&snake, 2, 10, 1);
	
	/* Init Food */
	Food food = initFood(stdscr, 3);
	for(uint8_t i = 0; i < 3; i++)
		setFoodRand(&food, i);

	/* Game Speed Values */
	const uint8_t MPS = 20;
	const uint8_t FPMS = 1000/MPS;
	struct timeval tv;
	uint64_t start, end;
	gettimeofday(&tv, NULL);
	start = tv.tv_sec * 1000 + tv.tv_usec/1000;

	/* Game Movement and Scores */
	int16_t move = KEY_DOWN;
	uint16_t score = 0;
	int16_t c = 0x00;
	int16_t foodCollision = -1;

	/* Game Loop */
	while(1)
	{
		/* Calculate current time */
		gettimeofday(&tv, NULL);
		end = tv.tv_sec * 1000 + tv.tv_usec/1000;

		/* Check if it is time to rerun game loop */
		if(end - start >= FPMS)
		{
			/* Movement Cases */
			c = getch(); //Get key press
			if(c == 'q' || c == 'Q') // Exit case 
				break;
			if(c != ERR) // Check movements
				switch(c) // Check for illegal moves
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

			/* Move snake, draw food, get collisions */
			moveSnake(&snake, move);
			drawSnake(&snake);
			eraseFood(&food);
			drawFood(&food);
			foodCollision = isIn(&food, snake.body[0].x, snake.body[0].y);
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
			
			/* Reset Clock */
			start = end;
		}
	}
	/* Release Memory */
	freeSnake(&snake);
	freeFood(&food);
	endwin();
}
