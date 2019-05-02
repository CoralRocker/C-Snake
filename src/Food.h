#pragma once

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

/* Structs */
/* Struct holding a single food item */
typedef struct Bite {
	uint16_t x;
	uint16_t y;
} Bite;
/* Struct containing all food items on screen or game */
typedef struct Food {
	Bite *arr;
	uint8_t size;
	WINDOW *win; 
} Food;

/* Initializes a Food struct with given window and size.
 */
Food initFood(WINDOW* win, uint8_t size)
{
	Food f = {
		(Bite*)malloc(sizeof(Bite)*size),
		size,
		win
	};
	return f;
}

/* If food array is created using initializer or malloc, this method frees used memory. */
void freeFood(Food* f)
{
	free(f->arr);
}

/* Sets the x and y coordinates of a specific food item. */
void setFood(Food *f, uint8_t index, uint16_t x, uint16_t y)
{
	if(index >= 0 && index < f->size)
	{
		f->arr[index].x = x;
		f->arr[index].y = y;
	}
}

/* Sets the x and y coordinates of a specific food item to random values. 
 * Is seeded by timeval.tv_usec because must be used multiple times in rapid succession 
 */
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

/* Remove all food items from screen */
void eraseFood(Food *f)
{
	for(int i = 0; i < f->size; i++)
		mvwaddch(f->win, f->arr[i].y, f->arr[i].x, ' ');
}

/* Draw all food items on screen */
void drawFood(Food *f)
{
	for(int i = 0; i < f->size; i++)
		mvwaddch(f->win, f->arr[i].y, f->arr[i].x, 'O');
}

/* Returns whether  or not a coordinate pair is the same as a food item's position. 
 * Used to check for collisions
 */
int8_t isIn(Food *f, uint16_t x, uint16_t y)
{
	for(uint8_t i = 0; i < f->size; i++)
	{
		if(f->arr[i].x == x && f->arr[i].y == y)
			return i;
	}
	return -1;
}
