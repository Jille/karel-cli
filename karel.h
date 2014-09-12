/*-
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <jille@quis.cx> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Jille Timmermans
 * ----------------------------------------------------------------------------
 */

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
