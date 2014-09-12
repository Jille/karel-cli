/*-
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <jille@quis.cx> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Jille Timmermans
 * ----------------------------------------------------------------------------
 */

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "kernel.h"
#include "karel.h"

int myrand(int, int);
void resetmap(void);

static hokje map[MAP_HEIGHT][MAP_WIDTH];
static struct karel kareltje;

extern command commands[];

int
main(int argc, char **argv) {
	kareltje.x = 1;
	kareltje.y = MAP_HEIGHT / 2;
	kareltje.dir = OOST;
#ifdef __FreeBSD__
	srandomdev();
#else
	srandom(time(NULL) ^ getpid());
#endif
	if(argc == 1) {
		printf("Usage: %s <command, [command... ]>\n", argv[0]);
		printf("commands: ");
		for(const command* cmd = commands; cmd->name != NULL;cmd++) {
			printf("%s", cmd->name);
			if ((cmd+1)->name != NULL) printf(", ");
		}
		printf("\n");
		exit(0);
	}
	gui_init();
	resetmap();
	gui_refresh(map, &kareltje);
	while(--argc) {
		++argv;
		const command* cmd = commands;
		while(cmd->name != NULL) {
			if (strcmp(*argv, cmd->name) == 0) {
				cmd->cmd();
				break;
			}
			cmd++;
		}
		if (cmd->name == NULL) {
			gui_die("Onbekende functie");
		}
	}
	gui_end();
	return 0;
}

void
resetmap(void) {
	int y, x;
	for(y=1; MAP_HEIGHT-1>y; y++) {
		map[y][0] = MUUR;
		map[y][MAP_WIDTH-1] = MUUR;
		for(x=1; MAP_WIDTH-1>x; x++) {
			map[y][x] = LEEG;
		}
	}
	for(x=0; MAP_WIDTH>x; x++) {
		map[0][x] = MUUR;
		map[MAP_HEIGHT-1][x] = MUUR;
	}
}

int
myrand(int min, int max) {
	return (random() % (max - min)) + min;
}

/* Kareltje functies */
static void
stap_undelayed(void) {
	int x = kareltje.x;
	int y = kareltje.y;
	switch(kareltje.dir) {
		case NOORD:
			y--;
			break;
		case OOST:
			x++;
			break;
		case ZUID:
			y++;
			break;
		case WEST:
			x--;
			break;
	}
	if(x<0 || x>=MAP_WIDTH || y<0 || y>=MAP_HEIGHT) {
		gui_die("Out of map");
	}
	if(map[y][x] == MUUR) {
		gui_die("Hit the wall");
	}
	kareltje.x = x;
	kareltje.y = y;
}

void
stap(void) {
	stap_undelayed();
	gui_refresh(map, &kareltje);
}


void
linksom(void) {
	switch(kareltje.dir) {
		case NOORD:
			kareltje.dir = WEST;
			break;
		case OOST:
			kareltje.dir = NOORD;
			break;
		case ZUID:
			kareltje.dir = OOST;
			break;
		case WEST:
			kareltje.dir = ZUID;
			break;
	}
	gui_refresh(map, &kareltje);
}

void
rechtsom(void) {
	switch(kareltje.dir) {
		case NOORD:
			kareltje.dir = OOST;
			break;
		case OOST:
			kareltje.dir = ZUID;
			break;
		case ZUID:
			kareltje.dir = WEST;
			break;
		case WEST:
			kareltje.dir = NOORD;
			break;
	}
	gui_refresh(map, &kareltje);
}

void
leg_bal(void) {
	if(map[kareltje.y][kareltje.x] == BAL) {
		gui_die("Cannot put ball");
	}
	map[kareltje.y][kareltje.x] = BAL;
}

void
pak_bal(void) {
	if(map[kareltje.y][kareltje.x] == LEEG) {
		gui_die("Cannot fetch ball");
	}
	map[kareltje.y][kareltje.x] = LEEG;
}

void
stappen(int s) {
	while(s--) {
		stap_undelayed();
	}
	gui_refresh(map, &kareltje);
}

void
teken_ballenlijn(int s) {
	while(s--) {
		stap_undelayed();
		leg_bal();
	}
	gui_refresh(map, &kareltje);
}

/* Checks */
int
muur_voor(void) {
	int x = kareltje.x;
	int y = kareltje.y;
	switch(kareltje.dir) {
		case NOORD:
			y--;
			break;
		case OOST:
			x++;
			break;
		case ZUID:
			y++;
			break;
		case WEST:
			x--;
			break;
	}
	if(x<0 || x>=MAP_WIDTH || y<0 || y>=MAP_HEIGHT) {
		gui_die("Invalid map! (checking wall out of map)");
	}
	return (map[y][x] == MUUR);
}

int
op_bal(void) {
	return (map[kareltje.y][kareltje.x] == BAL);
}

int
noord(void) {
	return kareltje.dir == NOORD;
}

/* Gekke dingen */
void
plaats_rechthoek(void) {
#if MAP_HEIGHT < 10
#error Map height must be at least 10
#endif
#if MAP_WIDTH < 30
#error Map width must be at least 30
#endif
	int x1 = myrand(10, MAP_WIDTH/2);
	int x2 = myrand(MAP_WIDTH/2 + 5, MAP_WIDTH - 3);
	int y1 = myrand(3, MAP_HEIGHT/2 - 3);
	int y2 = myrand(MAP_HEIGHT/2 + 3, MAP_HEIGHT - 3);
	int x, y;
	for(x = x1; x2 >= x; x++) {
		map[y1][x] = MUUR;
		map[y2][x] = MUUR;
	}
	for(y = y1; y2 >= y; y++) {
		map[y][x1] = MUUR;
		map[y][x2] = MUUR;
	}

	if(map[kareltje.y][kareltje.x] == MUUR) {
		gui_die("You got hit by a wall");
	}
	gui_refresh(map, &kareltje);
}

void
maak_ballensnoer(void) {
	int x, y;
	for(x=1; MAP_WIDTH-1>x; x++) {
		map[1][x] = BAL;
		map[MAP_HEIGHT-2][x] = BAL;
	}
	for(y=1; MAP_HEIGHT-1>y; y++) {
		map[y][1] = BAL;
		map[y][MAP_WIDTH-2] = BAL;
	}
	gui_refresh(map, &kareltje);
}

void
maak_ballenchaos(void) {
	int x, y;
	for(y = MAP_HEIGHT / 2 - 1; 1 < y; y--) {
		for(x = myrand(MAP_WIDTH - 10, MAP_WIDTH - 2); MAP_WIDTH - 1 > x; x++) {
			map[y][x] = BAL;
		}
	}
	gui_refresh(map, &kareltje);
}

void
maak_doolhof(void) {
	int kx, ky, kd, bx, by, hmuren, vmuren;
	FILE *fh = fopen("doolhof", "r");
	if(fh == NULL) {
		gui_die("No doolhof file");
	}
	fscanf(fh, "%d %d %d %d %d %d %d\n", &kx, &ky, &kd, &bx, &by, &hmuren, &vmuren);

	if(kx >= MAP_WIDTH || ky >= MAP_HEIGHT || bx >= MAP_WIDTH || by >= MAP_HEIGHT) {
		gui_die("Doolhof doesn't fit");
	}

	resetmap();
	kareltje.x = kx;
	kareltje.y = MAP_HEIGHT - ky;
	switch(kd) {
		case 0:
			kareltje.dir = NOORD;
			break;
		case 1:
			kareltje.dir = WEST;
			break;
		case 2:
			kareltje.dir = ZUID;
			break;
		case 3:
			kareltje.dir = OOST;
			break;
	}
	map[MAP_HEIGHT - by][bx] = BAL;

	while(hmuren--) {
		int mx, my, md;
		fscanf(fh, "%d %d %d\n", &mx, &my, &md);

		if(mx+md >= MAP_WIDTH || my >= MAP_HEIGHT) {
			gui_die("Doolhof doesn't fit");
		}

		while(md--) {
			map[MAP_HEIGHT - my][mx] = MUUR;
			mx++;
		}
	}

	while(vmuren--) {
		int mx, my, md;
		fscanf(fh, "%d %d %d\n", &mx, &my, &md);

		if(mx >= MAP_WIDTH || my+md >= MAP_HEIGHT) {
			gui_die("Doolhof doesn't fit");
		}

		while(md--) {
			map[MAP_HEIGHT - my][mx] = MUUR;
			my++;
		}
	}

	//fclose(fh);
	gui_refresh(map, &kareltje);
}
