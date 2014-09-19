/*-
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <jille@quis.cx> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Jille Timmermans
 * ----------------------------------------------------------------------------
 */
#include <stdlib.h>
#include "kernel.h"
/* adaptation helpers */
#define rand random
#define WereldHoogte MAP_HEIGHT
#define WereldBreedte MAP_WIDTH
#define min(a,b) (((a) < (b)) ? (a) : (b))

/* Acties */
void stap(void);
void linksom(void);
void rechtsom(void);
void pak_bal(void);
void leg_bal(void);
void stappen(int);
void teken_ballenlijn(int);

/* Checks */
int muur_voor(void);
int op_bal(void);
int noord(void);

/* Gekke dingen */
void plaats_rechthoek(void);
void maak_ballensnoer(void);
void maak_ballenchaos(void);
void maak_doolhof(void);

typedef void voidfunc_t(void);
struct command {
	const char* name;
	const voidfunc_t* cmd;
};


// english version of functions
void step		() ;
void turn_left 	() ;
void turn_right	() ;
bool on_ball	    () ;
void get_ball	() ;
void put_ball	() ;

bool in_front_of_wall    () ;
bool north		() ;

void rest		(int n) ;
void makeWorldEmpty () ;

void steps ( int ) ;
void place_rectangle (int, int, int, int) ;
void place_walls (int, int, int, bool) ;
void make_string_with_balls () ;
void make_chaos_with_balls () ;
void make_labyrinth () ;
void make_path_with_balls () ;			// toegevoegd in origineel door Peter Achten dd 8 sept 09
void create_ball (int x, int y) ;        // toegevoegd in origineel door Peter Achten 21 aug 13

void make_cave();               // toegevoegd in origineel door Peter
