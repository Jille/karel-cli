/*-
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <jille@quis.cx> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Jille Timmermans
 * ----------------------------------------------------------------------------
 */

#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "kernel.h"

extern int delay;

void
gui_init(void) {
	printf("Welcome to kareltje (NCurses version)\n  By Jille Timmermans (c) 2008\n");
	fflush(NULL);
	initscr();
}

void
gui_end(void) {
	endwin();
	exit(0);
}

void
gui_refresh(hokje map[MAP_HEIGHT][MAP_WIDTH], struct karel *karel) {
	int y, x;
	clear();
	for(y=0; MAP_HEIGHT>y; y++) {
		move(y, 0);
		for(x=0; MAP_WIDTH>x; x++) {
			if(karel->x == x && karel->y == y) {
				switch(karel->dir) {
					case NOORD:
						addch('^');
						break;
					case OOST:
						addch('>');
						break;
					case ZUID:
						addch('v');
						break;
					case WEST:
						addch('<');
						break;
				}
				continue;
			}
			switch(map[y][x]) {
				case LEEG:
					addch(' ');
					break;
				case BAL:
					addch('o');
					break;
				case MUUR:
					addch('x');
					break;
			}
		}
	}
	refresh();
	usleep(delay);
}

void
_gui_die(char *str) {
	endwin();
	printf("\n%s\n", str);
	exit(0);
}
