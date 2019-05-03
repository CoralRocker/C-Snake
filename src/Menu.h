#pragma once

#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MENU_ERROR 256

typedef struct MenuItem {
	char* msg;
	uint8_t msgSize;
} MenuItem;

typedef struct Menu {
	MenuItem* options;
	uint8_t menuSize;
	WINDOW* win;
	uint16_t win_h, win_w;
} Menu;

/* Init menu with given size and window */
Menu initMenu(WINDOW* win, uint8_t menuSize)
{
	Menu m = {
		(MenuItem*)malloc(sizeof(MenuItem)*menuSize),
		menuSize,
		win
	};
	if(win != NULL)
		getmaxyx(stdscr, m.win_h, m.win_w);
	else{
		m.win_h = 0;
		m.win_w = 0;
	}
	return m;
}	

/* Free allocated memory */
void freeMenu(Menu* m)
{
	free(m->options);
}

/* Add options to menu. Can add up to 256 option (8-bit). 
 * Option messages can be up to 256 bytes in length.
 */
void menuAddOpts(Menu* m, uint8_t numOpts, ...)
{
	/* Alloc Mem For Options */
	m->menuSize = numOpts;
	freeMenu(m);
	m->options = (MenuItem*)malloc(sizeof(MenuItem)*numOpts);
	
	/* VA_LIST */
	va_list va_opts;
	va_start(va_opts, numOpts);
	for(uint8_t i = 0; i < numOpts; i++)
	{
		m->options[i].msg = va_arg(va_opts, char*);
		m->options[i].msgSize = strlen(m->options[i].msg);
	}
	va_end(va_opts);
}

uint16_t runMenu(Menu* m)
{
	uint8_t opt = 0;
	while(1)
	{
		for(uint8_t i = 0; i < m->menuSize; i++)
		{
			if(i == opt)
			{
				attron(A_REVERSE);
				mvwaddstr(m->win, m->win_h/2+i, m->win_w/2 - m->options[i].msgSize/2, m->options[i].msg);
				attroff(A_REVERSE);
			}else
				mvwaddstr(m->win, m->win_h/2+i, m->win_w/2 - m->options[i].msgSize/2, m->options[i].msg);
		}
		int16_t c = getch();
		if( c == 'q' || c == 'Q')
			break;
		if(c == KEY_UP)
		{
			opt--;
			if(opt < 0)
				opt = m->menuSize - 1;
		}
		if(c == KEY_DOWN)
		{
			opt++;
			if(opt > m->menuSize - 1)
				opt = 0;
		}
		if(c == '\n')
		{
			werase(m->win);
			return opt;
		}
	}
	return MENU_ERROR;
}
