#pragma once

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct SnakeSegment {
	uint16_t x, y;
} SnakeSegment;

typedef struct Snake {
	SnakeSegment *body;
	WINDOW* win;
	uint16_t size;
	uint16_t win_h, win_w;
} Snake;

Snake initSnake(WINDOW *win, uint16_t size)
{
	Snake s = {
		(SnakeSegment*)malloc(sizeof(SnakeSegment)*size),
		win,
		size
		};
	
	if(win == NULL)
	{
		s.win_h = 0;
		s.win_w = 0;
	}else
		getmaxyx(s.win, s.win_h, s.win_w);

	return s;
}

void freeSnake(Snake* s)
{
	free(s->body);
}

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

void eraseSnake(Snake *s)
{
	for(uint16_t i = 0; i < s->size; i++)
		mvwaddch(s->win, s->body[i].y, s->body[i].x, ' ');
}

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
			if(s->body[0].y + 1 == s->body[1].y)
			{
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
			if(s->body[0].y - 1 == s->body[1].y)
			{
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
			if(s->body[0].x + 1 == s->body[1].x)
			{
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
                	if(s->body[0].x - 1 == s->body[1].x)
			{
				break;
			}
			s->body[0].x--;
                	break;
	}
}



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
