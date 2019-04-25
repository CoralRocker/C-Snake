#include <ncurses.h>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include "menu.hpp"
#include "SnakeScore.hpp"
#include <cstring>
#include <signal.h>
#include <string>
#include <chrono>

/* Global Vars */
bool DEATH_WALLS = false;

/* Method to add memory to snake array in order to grow */
void snakeIncrement(int *input[2], int *size)
{
	/* Make Temporary Snake Array to copy values to */
	int *temp[2] = {
		(int*)calloc(*size, sizeof(int) * *size),
		(int*)calloc(*size, sizeof(int) * *size)
	};

	/* Copy Values From Actual Snake Array To Temporary One */
	for(int i = 0; i < *size; i++)
	{
		temp[0][i] = input[0][i];
		temp[1][i] = input[1][i];
	}

	/* Clear And Deallocate Snake Array */
	free(input[0]);
	free(input[1]);

	/* Increment Size pointer */
	size[0]++;

	/* Reallocate memory for snake array */
	input[0] = (int*)calloc(size[0], sizeof(int) * size[0]);
	input[1] = (int*)calloc(size[0], sizeof(int) * size[0]);

	/* Copy temp values into snake array */
	for(int i = 0; i < *size -1; i++)
	{
		input[0][i] = temp[0][i];
		input[1][i] = temp[1][i];
	}

	/* Free memory from temporary array */
	free(temp[0]);
	free(temp[1]);
}

/* Gets a random integer within the exclusive range given. Used for food calculations */
int getRandInRange(int min, int max)
{
	return (static_cast<int> (rand() % (max - min + 1) + min));
}

/* Method to draw snake */
void drawSnake(int *input[2], int size)
{
	for(int i = 0; i < size; i++)
	{
		/* Draw head a different color than the body and a different symbol */
		if(i == 0)
		{
			attron(COLOR_PAIR(1));
			mvaddstr(input[1][i], input[0][i], "0");
			attroff(COLOR_PAIR(1));
		}
		else
		/* Draw Snake Body */
			mvaddch(input[1][i], input[0][i], '#');

	}
}

/* Method to erase snake */
void eraseSnake(int *snake[2], int size)
{
	/* Loop over snake and write empty spaces where it is */
	for(int i = 0; i < size; i++)
		mvaddch(snake[1][i], snake[0][i], ' ');
}

/* Method to calculate new position of snake's head, then shift snake */
void moveSnake(int *snake[2], int direction, int size, int height, int width, int oldMove)
{
	/* Clear snake before changing it */
	eraseSnake(snake, size);
	/* Shift all values in snake array back one, except for the head */
	for(int i = size - 1; i >= 1; i--)
	{
		snake[0][i] = snake[0][i-1];
		snake[1][i] = snake[1][i-1];
	}

	/* Calculate new position for head */
	switch(direction)
	{	
		case KEY_DOWN:
			/* Check if it has hit a wall */
			if(snake[1][0] + 1 == height - 1)
				snake[1][0] = 1;
			
			/* Exit if the movement would make snake back up onto itself */
			if(snake[1][0] + 1 == snake[1][1])
				break;
			/* Increment the position of the snake */
			snake[1][0]++;
			break;
		case KEY_UP:
			/* Check if it has hit a wall */
			if(snake[1][0] == 1)
				snake[1][0] = height;

			/* Exit if the movement would make snake back up onto itself */
			if(snake[1][0] -1 == snake[1][1])
				break;

			/* Increment the position of the snake */
			snake[1][0]--;
			break;
		case KEY_RIGHT:
			/* Check if it has hit a wall */
			if(snake[0][0] + 1 == width - 1)
				snake[0][0] = 0;

			/* Exit if the movement would make snake back up onto itself */
			if(snake[0][0] + 1 == snake[0][1])
				break;

			/* Increment the position of the snake */
			snake[0][0]++;
			break;
		case KEY_LEFT:
			/* Check if it has hit a wall */
			if(snake[0][0] - 1 == 0)
				snake[0][0] = width;

			/* Exit if the movement would make snake back up onto itself */
			if(snake[0][0] - 1 == snake[0][1])
				break;

			/* Increment the position of the snake */
			snake[0][0]--;
			break;
	}	
	
	/* Draw the modified snake */
	drawSnake(snake, size);
}

/* Places food */
void foodCalc(int f[][2], int h, int w, int row)
{
	/* Place food randomly */
	f[row][0] = getRandInRange(1, w - 2);
	f[row][1] = getRandInRange(1, h - 2);
}

/* Print Food */
void printFood(int f[][2], int rows)
{
	/* For every food item in the food array, draw a food piece */
	for(int i = 0; i < rows; i++)
	{
		attron(COLOR_PAIR(3));
		mvaddch(f[i][1], f[i][0], '@');
		attroff(COLOR_PAIR(3));
	}
}

/* Remove food */
void eraseFood(int f[][2], int row)
{
	/* Remove the specified food piece */
	//mvaddch(f[row][1], f[row][0], ' ');
}

/* Detects collisions between snakes, walls, and other fun things */
/* returns 'q' if the snake has hit something and died, else -1 */
int collisionDetect(int *snake[2], int size, int h, int w, char* &msg)
{
	/* Check if snake has hit itself (in confusion) */
	for( int i = 1; i < size ; i++)
	{
		/* if the head of the snake is in the same position as anywhere else in the body, exit */
		if(snake[0][0] == snake[0][i] && snake[1][0] == snake[1][i])
		{	
			/* Sets the death message and exits with 'q' */
			msg = (char*)"Snake Collided with itself";
			return 'q';
		}
	}
	
	/* If the snake dies upon hitting one of the walls */
	if(DEATH_WALLS)
	{
		/* If snake hit left or right walls */
		if(snake[0][0] == 1 || snake[0][0] == w - 1)
		{
			msg = (char*)"Snake collided with vertical walls";
			return 'q';
		}
		/* If snake hit top or bottom walls */
		else if(snake[1][0] == 0 || snake[1][0] == h - 1)
		{
			snprintf(msg, 37, "Snake collided with horizontal walls");
			return 'q';
		}
	}
	
	/* Exit without Error */
	return -1;

}

/* Draws border for screen */
void beerMeABorder(WINDOW *win, char symbol)
{
	attron(COLOR_PAIR(2));
	wborder(win, symbol, symbol, symbol,symbol, symbol, symbol, symbol, symbol);
	attroff(COLOR_PAIR(2));
}

/* Tests Speed */
int msOffset(void)
{
	/* Initialize Tester */
	int tsnakeSize = 3;
	int *tsnake[2] = {
		(int*)calloc(tsnakeSize, sizeof(int) * tsnakeSize),
		(int*)calloc(tsnakeSize, sizeof(int) * tsnakeSize)
	};
	for(int i = 0; i < tsnakeSize; i++)
	{
		tsnake[0][i] = 10 + i;
		tsnake[1][i] = 10;
	}
	
	long long start = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

	snakeIncrement(tsnake, &tsnakeSize);

	long long end = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

	free(tsnake[0]);
	free(tsnake[1]);
	printf("%ld\n", end-start);
	//printf("Time elapsed for one Snake Increment: %ld\n", end-start);
	//printf("Start: %ld\nEnd: %ld\n", start, end);
	return -1;	

}

/* Runs the game */
int main(int argc, char** argv)
{
//	if(msOffset() != 0)
//		exit(-1);

	int GROWTH_HORMONE = 1;
	int rows = 5;
	if(argc >= 2)
		GROWTH_HORMONE = atoi(argv[1]);

	/* snake vars */
	int snakeSize = 3;
	int *snake[2] = {
		(int*)calloc(snakeSize, sizeof(int) * snakeSize),
		(int*)calloc(snakeSize, sizeof(int) * snakeSize)
	};
	snake[0][0] = 10;
	snake[0][1] = 11;
	snake[0][2] = 12;
	snake[1][0] = 10;
	snake[1][1] = 10;
	snake[1][2] = 10;
	unsigned char pts = 0;

	/* food vars */
	int food[5][2] = {
		{10, 10},
		{20, 20},
		{30, 30},
		{40, 40},
		{50, 50}
	};
	
	/* NCURSES INITIALIZATION */
	initscr();
	nodelay(stdscr, TRUE);
	keypad(stdscr, TRUE);
	noecho();
	cbreak();
	curs_set(0);
	timeout(100);

	beerMeABorder(stdscr, 'X');
	
	start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_RED);
	init_pair(3, COLOR_RED, COLOR_GREEN);

	/* World Vars */
	int hi, wid;
	getmaxyx(stdscr, hi, wid);
	srand(time(NULL));
	int input = -1, oldMove = KEY_DOWN;
	char* strBuf = (char*)calloc(32, sizeof(char) * 32);
	char* memBuf = (char*)calloc(64, sizeof(char) * 64);
	char* name = new char[32];
	Score *board = new Score(5, "snake.bin");

	/* Menu */
	menu *Menu = new menu(stdscr);
	Menu->setOpts(9, "EASY", "MEDIUM", "HARD", "GOD", "QUIT", "FOR MITHUL (RETARD MODE)", "TURN ON DEATH WALLS", "TURN OFF DEATH WALLS", "Scoreboard");
	MENURUN: 
	clear();
	if(DEATH_WALLS)
		Menu->addStrings(1, "Deathwalls are on");
	else
		Menu->addStrings(1, "Deathwalls are off");
	switch(Menu->runMenu())
	{
		case 0:
			timeout(150);
			break;
		case 1:
			timeout(100);
			break;
		case 2:
			timeout(50);
			break;
		case 3:
			timeout(0);
			break;
		case -1:
		case 4:
			goto ENDPROG;
			break;
		case 5:
			timeout(5000);
			break;
		case 6:
			DEATH_WALLS = true;
			goto MENURUN;
		case 7:
			DEATH_WALLS = false;
			goto MENURUN;
		case 8:
			Score *b = new Score(5, "snake.bin");
			b->loadVector();
			clear();
			for(int i = 1; i <=  5; i++)
			{
				std::string str = b->getPair(i).second;
				str += ": ";
				str += std::to_string(b->getPair(i).first);
				mvaddstr(hi/2 + i, wid/2 - str.length()/2, str.c_str());
			}
			nodelay(stdscr, FALSE);
			getch();
			nodelay(stdscr, TRUE);
			delete(b);
			goto MENURUN;

	}	
	delete(Menu);
	clear();
	/* World Loop */
	while(input != 'q')
	{
		getmaxyx(stdscr, hi, wid);
		beerMeABorder(stdscr, 'X');	
		
		/* Screen Prints */
		snprintf(strBuf, 12, "Points: %03d", pts);
		attron(COLOR_PAIR(3));
		mvaddstr(0, 1, strBuf);
		attroff(COLOR_PAIR(3));
		printFood(food, rows);
		refresh();

		/* Movement */
		input = getch();
		if(input != -1)
		{
			if(input == KEY_DOWN && oldMove != KEY_UP)
				oldMove = input;
			if(input == KEY_UP && oldMove != KEY_DOWN)
				oldMove = input;
			if(input == KEY_RIGHT && oldMove != KEY_LEFT)
				oldMove = input;
			if(input == KEY_LEFT && oldMove != KEY_RIGHT)
				oldMove = input;
			if(input == 'q' || input == 'Q')
				break;
		}
		moveSnake((int**)&snake, oldMove, snakeSize, hi, wid, oldMove);
		input = collisionDetect(snake, snakeSize, hi, wid,memBuf); 
		
		/* Food and Collision Detection */
		int collideFood = 0;
		int row = -0;
		for(int i = 0; i < rows; i++)
		{
			if(food[i][0] == snake[0][0] && food[i][1] == snake[1][0])
			{	collideFood = 1; row = i; }
		}
		if(collideFood)
		{
			pts++;
			//eraseFood(food, rows);
			foodCalc((int (*)[2])food, hi, wid, row);
			for(int i = 0; i < GROWTH_HORMONE; i++)
				snakeIncrement((int**)&snake, &snakeSize);
		}
		collideFood = 0;

	}
		
	/* Score Window */
	clear();
	mvaddstr(hi/2 + 1, wid/2 - strlen(strBuf)/2, strBuf);
	mvaddstr(hi/2, wid/2 - strlen(memBuf)/2, memBuf);
	refresh();
	nodelay(stdscr, FALSE);
	getch();
	
	clear();
	board->loadVector();
	if(board->rankScore(pts) > 0)
	{
		nodelay(stdscr, 0);
		echo();
		nocbreak();
		clear();
		mvaddstr(hi/2, wid/2 - 9, "Enter your name: ");
		refresh();
		getstr(name);
		//strlen(name);
		std::string s(name, strlen(name));
		board->putScore(pts, s, board->rankScore(pts));
	}
	//delete(name);
	board->storeVector();
	clear();
        for(int i = 1; i <  board->size; i++)
        {
	        std::string str = board->getPair(i).second;
        	str += ": ";
        	str += std::to_string(board->getPair(i).first);
                mvaddstr(hi/2 + i, wid/2 - str.length()/2, str.c_str());
        }
	nodelay(stdscr, FALSE);
	getch();
	delete(board);
	
	//getch();

	/* End Window */
	endwin();
	printf("%s\n", name);
	delete(name);
ENDPROG:
	if(stdscr != NULL)
		endwin();
	free(strBuf);
	free(memBuf);
	free(snake[0]);
	free(snake[1]);
	printf("Dev Branch\n");
}	
