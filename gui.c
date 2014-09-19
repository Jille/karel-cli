/*-
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <jille@quis.cx> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Jille Timmermans
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "kernel.h"

extern int delay;

void
gui_init(void) {
	printf("Welcome to kareltje (CLI version)\n  By Jille Timmermans (c) 2008\n");
}

void
gui_end(void) {
	printf("\033[%dB", MAP_HEIGHT);
	exit(0);
}

void
gui_refresh(hokje map[MAP_HEIGHT][MAP_WIDTH], struct karel *karel) {
	int y, x;
	for(y=0; MAP_HEIGHT>y; y++) {
		for(x=0; MAP_WIDTH>x; x++) {
			if(karel->x == x && karel->y == y) {
				switch(karel->dir) {
					case NOORD:
						putchar('^');
						break;
					case OOST:
						putchar('>');
						break;
					case ZUID:
						putchar('v');
						break;
					case WEST:
						putchar('<');
						break;
				}
				continue;
			}
			switch(map[y][x]) {
				case LEEG:
					putchar(' ');
					break;
				case BAL:
					putchar('o');
					break;
				case MUUR:
					putchar('x');
					break;
			}
		}
		putchar('\n');
	}
	printf("\033[%dA", MAP_HEIGHT);
	usleep(delay);
}

void
_gui_die(char *str) {
	printf("\033[%dB", MAP_HEIGHT);
	printf("\n%s\n", str);
	exit(0);
}
