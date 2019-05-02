#pragma once

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>


/* Structs */
/* Struct for inidividual snake segment. Helps keep Snake struct orderly */
typedef struct SnakeSegment {
	uint16_t x, y; // X and Y coordinates of segment
} SnakeSegment;

/* Struct for snake itself. */
typedef struct Snake {
	SnakeSegment *body; // Array of SnakeSegments. 
	WINDOW* win; // Window to write to
	uint16_t size; // Size of snake, equal to amount of segments in body pointer
	uint16_t win_h, win_w; // Width and height of window
} Snake;

/* Method to create a basic snake. 
 * Returns a snake with given size, and no segments initialized 
 */
Snake initSnake(WINDOW *win, uint16_t size)
{
	/* Create Snake */
	Snake s = {
		(SnakeSegment*)malloc(sizeof(SnakeSegment)*size),
		win,
		size
		};
	
	/* Set window height and width appropriately */
	if(win == NULL)
	{
		s.win_h = 0;
		s.win_w = 0;
	}else
		getmaxyx(s.win, s.win_h, s.win_w);

	return s;
}

/* Since Snakes use malloc to allocate memory for itself, it must be freed */
void freeSnake(Snake* s)
{
	free(s->body);
}

/* Sets a given snake segment to a given set of x and y values */
int setSegment(Snake *s, uint16_t index, uint16_t x, uint16_t y)
{
	if(index < s->size && index >= 0)
	{
		s->body[index].x = x;
		s->body[index].y = y;
		return 0;
	}else{
		fprintf(stderr, "Index %d is out of bounds.\nBounds are 0 to %d\n", index, s->size);
		return -1;
	}
}

/* Increases the size of the snake by creating a temporary snake, copying actual snake into
 * it, then freeing and recreating actual snake. Is memory leak free, so long as snake is initialized with initSnake().
 */
void incrementSnake(Snake *s)
{
	Snake temp = initSnake(NULL, s->size);
	for(uint16_t i = 0; i < s->size; i++)
		temp.body[i] = s->body[i];
	freeSnake(s);
	s->body = (SnakeSegment*)malloc(sizeof(SnakeSegment)*s->size+1);
	s->size++;
	for(uint16_t i = 0; i < s->size-1; i++)
		s->body[i] = temp.body[i];
	freeSnake(&temp);
}	

/* Print snake to given window */
void drawSnake(Snake *s)
{
	for(uint16_t i = 0; i < s->size; i++)
	{
		if(i == 0)
			mvwaddch(s->win, s->body[i].y, s->body[i].x, '0');
		else
			mvwaddch(s->win, s->body[i].y, s->body[i].x, '#');
	}
}

/* Remove snake from window */
void eraseSnake(Snake *s)
{
	for(uint16_t i = 0; i < s->size; i++)
		mvwaddch(s->win, s->body[i].y, s->body[i].x, ' ');
}

/* Given a 16-bit move, adjust the snake's position appropriately */
void moveSnake(Snake *s, uint16_t dir)
{
	eraseSnake(s);
	for(uint16_t i = s->size - 1; i >= 1; i--)
	{
		s->body[i] = s->body[i-1];	
	}
	switch(dir)
	{
		case KEY_DOWN:
			if(s->body[0].y + 1 > s->win_h)
			{
				s->body[0].y = 1;
				break;
			}
			s->body[0].y++;
			break;
		case KEY_UP:
			if(s->body[0].y - 1 < 0)
			{
				s->body[0].y = s->win_h;
				return;
			}
			s->body[0].y--;
			break;
		case KEY_RIGHT:
			if(s->body[0].x + 1 > s->win_w)
			{
				s->body[0].x = 1;
				break;
			}
			s->body[0].x++;
			break;
		case KEY_LEFT:
			if(s->body[0].x - 1 < 0) 		
			{
				s->body[0].x = s->win_w - 1;
                		break;
                	}
			s->body[0].x--;
                	break;
	}
}


/* Is used to check if snake has collided with something */
uint8_t collideSnake(Snake *s, uint8_t death_walls)
{
	if(death_walls)
		if(s->body[0].y == 0 || s->body[0].y == s->win_h || s->body[0].x == s->win_w || s->body[0].x == 0)
			return 1;
	for(uint16_t i = 1; i < s->size; i++)
		if(s->body[0].y == s->body[i].y && s->body[0].x == s->body[i].x)
			return 1;
	return 0;
}
