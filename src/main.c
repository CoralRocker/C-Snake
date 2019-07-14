#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include "Snake.h"
#include "Score.h"
#include "Food.h"
#include <sys/types.h>
#include <sys/time.h>
#include "Menu.h"
#include <string.h>

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

void endScreen(WINDOW* win, uint16_t score, char* fileName)
{
	/* Score Show */
	werase(win);
	uint16_t h, w;
	getmaxyx(win, h, w);
	mvwprintw(win, h/2, w/2 - 15, "Congratulations! You got %d points!", score);
	nodelay(win, FALSE);
	getch();
	werase(win);

	/* Scoreboard Show */
	ScoreArray scores = initScoreArray(fileName, 5);
	readScore(&scores);
	char *strBuf = (char*)malloc(sizeof(char)*256);
	for(uint8_t i = 0; i < scores.size; i++)
	{
		memset(strBuf, 0x00, 256);
		sprintf(strBuf, "%d: %s -- %dpts", i, scores.arr[i].name, scores.arr[i].score);
		mvwaddstr(win, h/2+i, w/2 - strlen(strBuf)/2, strBuf);
	}
	getch();
	werase(win);

	/* Add Score */
	uint8_t rank = rankScore(&scores, score);
	if(rank < scores.size)
	{
		sprintf(strBuf, "Congratulations! You got %d place!", rank + 1);
		mvwaddstr(win, h/2, w/2 - strlen(strBuf)/2, strBuf);
		refresh();
		mvwaddstr(win, h/2 + 1, w/2 - 8, "Enter Your Name: ");
		echo();
		memset(strBuf, 0x00, 256);
		getnstr(strBuf, 255);
		sprintf(strlen(strBuf) + strBuf, 0x00);
		werase(win);
		insertScore(&scores, rank, score, strBuf, strlen(strBuf));
		storeScore(&scores);
		//mvwaddstr(win, h/2-1, w/2 - strlen(strBuf), strBuf);
		for(uint8_t i = 0; i < scores.size; i++)
		{
			memset(strBuf, 0x00, 256);
			sprintf(strBuf, "%d: %s -- %dpts", i, scores.arr[i].name, scores.arr[i].score);
			mvwaddstr(win, h/2+i, w/2 - strlen(strBuf)/2, strBuf);
		}
		getch();
		werase(win);
	}
	freeScoreArray(&scores);
	free(strBuf);
}

int main(int argc, char** argv)
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
	uint8_t MPS = 20;
	uint8_t FPMS = 1000/MPS;
	struct timeval tv;
	uint64_t start, end;
	gettimeofday(&tv, NULL);
	start = tv.tv_sec * 1000 + tv.tv_usec/1000;
	char* scoreFileName = "Snake.bin";

	/* Game Movement and Scores */
	int16_t move = KEY_DOWN;
	uint16_t score = 0;
	int16_t c = 0x00;
	int16_t foodCollision = -1;	

	/*endScreen(stdscr, atoi(argv[1]), scoreFileName);
	endwin();
	return -1;
	*/

	/* Game Menu */
	Menu m = initMenu(stdscr, 3);
	menuAddOpts(&m, 4, "Easy", "Medium", "Hard", "Quit");
	uint16_t option = runMenu(&m);
	switch(option)
	{
		case 0:
			MPS = 10;
			FPMS = 1000/MPS;
			break;
		case 1:
			MPS = 20;
			FPMS = 1000/MPS;
			break;
		case 2:
			MPS = 30;
			FPMS = 1000/MPS;
			break;
		case 3:
			freeSnake(&snake);
			freeFood(&food);
			freeMenu(&m);
			endwin();
			return 0;
	}
	freeMenu(&m);

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
				if(score % 2 == 0)
				{
					MPS += 1;
					FPMS = 1000/MPS;
				}
			}
			if(collideSnake(&snake, 0))
			{
				break;
			}
			
			/* Reset Clock */
			start = end;
		}
	}

	/* End Screen */
	endScreen(stdscr, score, scoreFileName);

	/* Release Memory */
	freeSnake(&snake);
	freeFood(&food);
	endwin();
}
