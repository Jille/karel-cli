/*-
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <jille@quis.cx> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Jille Timmermans
 * ----------------------------------------------------------------------------
 */

#define MAP_WIDTH	70
#define MAP_HEIGHT	35

typedef enum { LEEG, BAL, MUUR } hokje;
typedef enum { NOORD, OOST, ZUID, WEST } direction;
struct karel {
	int x;
	int y;
	direction dir;
};

void gui_init(void);
void gui_end(void);
void gui_refresh(hokje [MAP_HEIGHT][MAP_WIDTH], struct karel *);
void _gui_die(char *);

#ifdef __cplusplus
#define gui_die(x) _gui_die((char *)x)
#else
#define gui_die(x) _gui_die(x)
#endif
