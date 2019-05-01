#pragma once

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

typedef struct Bite {
	uint16_t x;
	uint16_t y;
} Bite;

typedef struct Food {
	Bite *arr;
	uint8_t size;
	WINDOW *win; 
} Food;

Food initFood(WINDOW* win, uint8_t size)
{
	Food f = {
		(Bite*)malloc(sizeof(Bite)*size),
		size,
		win
	};
	return f;
}

void freeFood(Food* f)
{
	free(f->arr);
}

void setFood(Food *f, uint8_t index, uint16_t x, uint16_t y)
{
	if(index >= 0 && index < f->size)
	{
		f->arr[index].x = x;
		f->arr[index].y = y;
	}
}

void setFoodRand(Food *f, uint8_t index)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	srand(tv.tv_usec);
	uint16_t x, y;
	getmaxyx(f->win, y, x);
	if(index >= 0 && index < f->size)
	{
		f->arr[index].x = rand() % x;
		f->arr[index].y = rand() % y;
	}
}

void eraseFood(Food *f)
{
	for(int i = 0; i < f->size; i++)
		mvwaddch(f->win, f->arr[i].y, f->arr[i].x, ' ');
}

void drawFood(Food *f)
{
	for(int i = 0; i < f->size; i++)
		mvwaddch(f->win, f->arr[i].y, f->arr[i].x, 'O');
}

int16_t isIn(Food *f, uint16_t x, uint16_t y)
{
	for(uint8_t i = 0; i < f->size; i++)
	{
		if(f->arr[i].x == x && f->arr[i].y == y)
			return i;
	}
	return -1;
}
