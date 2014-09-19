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
		stap();
	}
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

// copy-paste uit charles_package
void teken_rechthoek ( int links, int onder, int breedte, int hoogte )
{
	for (int hor = links; hor<=links+breedte; hor++)
	{
		map[MAP_HEIGHT-onder][hor]        = MUUR ;
		map[MAP_HEIGHT-(onder+hoogte)][hor] = MUUR ;
	};

	for (int vert = onder; vert<=onder+hoogte; vert++)
	{
		map[MAP_HEIGHT-vert][links]         = MUUR ;
		map[MAP_HEIGHT-vert][links+breedte] = MUUR ;
	};

	if(map[kareltje.y][kareltje.x] == MUUR) {
		gui_die("You got hit by a wall");
	}
	gui_refresh(map, &kareltje);
}

// copy-paste uit charles_package
void maak_muur ( int links, int onder, int aantal, bool horizontaal )
{
    if (horizontaal)
    {
        for (int x = links ; x <= links + aantal ; x++)
        {
            map[MAP_HEIGHT-onder][x] = MUUR ;
        }
    }
    else
    {
        for (int y = onder; y <= onder+aantal ; y++)
        {
            map[MAP_HEIGHT-y][links] = MUUR ;
        }
    }
	if(map[kareltje.y][kareltje.x] == MUUR) {
		gui_die("You got hit by a wall");
	}
	gui_refresh(map, &kareltje);
}

// copy-paste uit charles_package
void
creeer_bal(int x, int y) {
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT)
    {
        map[MAP_HEIGHT-y-1][x] = BAL;
        gui_refresh(map, &kareltje);
    }
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

// copy-paste uit charles_package
void
maak_ballenpad ()			// toegevoegd in origineel door Peter
{
	resetmap();

	for (int ix = 1; ix < MAP_WIDTH-2; ix++)				map[MAP_HEIGHT/2][ix] = BAL;
	for (int iy = MAP_HEIGHT/2; iy < MAP_HEIGHT-3; iy++)	map[iy][MAP_WIDTH-3]  = BAL;
	for (int ix = MAP_WIDTH-3; ix >= MAP_WIDTH / 2; ix--)	map[MAP_HEIGHT-4][ix] = BAL;
	for (int iy = MAP_HEIGHT-4; iy >= 1; iy--)				map[iy][MAP_WIDTH/2]  = BAL;
	for (int ix = MAP_WIDTH / 2; ix >= 2; ix--)		    	map[1][ix]            = BAL;
	for (int iy = 1; iy <= MAP_HEIGHT / 2 - 3; iy++)		map[iy][2]            = BAL;

	gui_refresh(map, &kareltje);
}

// copy-paste uit charles_package
void
cave_wall (direction richting)    // toegevoegd in origineel door Peter
{
    const int marge       = MAP_HEIGHT / 3 ;
    const int max_breedte = MAP_WIDTH / 10 ;

    for (int ix = 2; ix <= MAP_WIDTH-3; )
    {
        const int breedte = rand() % (min (max_breedte, MAP_WIDTH - ix - 2)) + 1 ;
        const int dy = rand() % marge + 2 ;
        int y ;
        if (richting == NOORD)
        {
            y = dy ;
        }
        else
        {
            y = MAP_HEIGHT - dy - 1 ;
        }
        for (int i=1; i <= breedte; i++)
        {
            map[y][ix] = MUUR ;
            ix++ ;
        }
    }
}

// copy-paste uit charles_package
void
make_cave()               // toegevoegd in origineel door Peter
{
    resetmap();

    cave_wall (NOORD) ;
    cave_wall (ZUID) ;
	gui_refresh(map, &kareltje);
}

// For English version:
void step			() { stap(); }
void turn_left 		() { linksom(); }
void turn_right		() { rechtsom(); }
bool on_ball		() { return op_bal(); }
void get_ball		() { pak_bal(); }
void put_ball		() { leg_bal(); }

bool in_front_of_wall    () { return muur_voor(); }
bool north			() { return noord(); }

void makeWorldEmpty () { resetmap(); }
void place_rectangle        (int left, int bottom, int width, int height)  { teken_rechthoek(left,bottom,width,height); }
void place_walls            (int left, int bottom, int nr_of_walls, bool horizontal) { maak_muur(left,bottom,nr_of_walls,horizontal); }
void make_string_with_balls ()  { maak_ballensnoer (); }
void make_chaos_with_balls  ()  { maak_ballenchaos(); }
void make_path_with_balls   ()  { maak_ballenpad(); }
void make_labyrinth         ()  { maak_doolhof(); }
void create_ball            (int x, int y) { creeer_bal(x,y); }

void steps (int number_of_steps) { stappen(number_of_steps ) ; }
