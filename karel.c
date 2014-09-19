#include <unistd.h>
#include "karel.h"

int delay = 50000;

//Draai totdat karel naar het noorden wijst
void wijs_naar_noord()
{
	while(!noord())
	{
		linksom();
	}
}

//Leg bal als er geen bal ligt
void leg_mogelijk_bal()
{
	if(!op_bal())
		leg_bal();
}

//Pak bal als er een bal ligt
void pak_mogelijk_bal()
{
	if(op_bal())
		pak_bal();
}

//Loop door totdat karel een muur tegenkomt
void ga_door_tot_muur()
{
	while(!muur_voor())
	{
		stap();
	}
}

//Ga door tot muur en pak potentieel alle ballen
void ga_door_tot_muur_en_pak_mogelijk_bal()
{
	pak_mogelijk_bal();
	while(!muur_voor())
	{
		stap();
		pak_mogelijk_bal();
	}
}

//Leg ballen neer indien er geen bal ligt
void teken_mogelijk_ballenlijn(int aantal_stappen)
{
	for(int i = 0; i < aantal_stappen; i++)
	{
		leg_mogelijk_bal();
		stap();
	}
}

void teken_rechthoek(int breedte, int hoogte)
{
	rechtsom();
	teken_mogelijk_ballenlijn(hoogte-1);
	linksom();
	teken_mogelijk_ballenlijn(breedte-1);
	linksom();
	teken_mogelijk_ballenlijn(hoogte-1);
	linksom();
	teken_mogelijk_ballenlijn(breedte-1);
}

//Assumpties: er zit geen muur tussen het pad van begin tot eind
void ga_terug_naar_uitgangspositie()
{
	wijs_naar_noord();
	ga_door_tot_muur();
	linksom();
	ga_door_tot_muur();
	linksom();
	stappen(13);
	linksom();
}

// Karel loopt naar een nieuwe positie en trekt daarbij een verticale ballenlijn
void start ()
{
	stappen(10);
	linksom();
	teken_ballenlijn (10);
	rechtsom();
	stappen(3);
	ga_terug_naar_uitgangspositie();
}

// Karel loopt vanuit zijn beginpositie een stukje naar rechts, tekent daar een rechthoek
// en loopt tenslotte terug naar beginpositie:
void teken_rechthoek ()
{
	stappen(10);
	teken_rechthoek(10, 6);
	ga_terug_naar_uitgangspositie();
}

// Ga naar rechts en loop alle lijnen van ballen af totdat er een muur wordt tegengekomen. Karel gaat dan terug naar zijn beginpositie
void ruim_ballenchaos_op ()
{
	maak_ballenchaos () ;

	ga_door_tot_muur();
	linksom();

	while(!muur_voor())
	{
		stap();

		if(op_bal())
		{
			linksom();

			while(op_bal())
			{
				pak_bal();
				stap();
			}

			rechtsom();
			rechtsom();

			ga_door_tot_muur();
			linksom();
		}
	}

	ga_terug_naar_uitgangspositie();
}

//Pak elke bal aan de rand van het venster
void ruim_ballensnoer_op ()
{
	maak_ballensnoer () ;
	rechtsom();

	for(int i = 0; i < 4; i++)
	{
		ga_door_tot_muur_en_pak_mogelijk_bal();
		linksom();
	}

	for(int i = 0; i < 13; i++)
	{
		stap();
		pak_mogelijk_bal();
	}

	linksom();
}

//Geeft aan of er een muur rechts van karel zit
bool muur_rechts()
{
	rechtsom();
	bool result = muur_voor();
	linksom();

	return(result);
}

//Volg de eerstvolgende muur naar de eerstvolgende bal en ga dan terug naar beginpositie
void verlaat_doolhof ()
{
	maak_doolhof();

	ga_door_tot_muur();

	linksom();
	while(true)
	{
		while(muur_rechts() && !op_bal())
		{
			if(muur_voor())
				linksom();

			stap();
		}

		if(op_bal())
			break;

		rechtsom();
		stap();
	}

	ga_terug_naar_uitgangspositie();
}
command commands[] = {
	{"teken_rechthoek", teken_rechthoek},
	{"ruim_ballensnoer_op", ruim_ballensnoer_op},
	{"ruim_ballenchaos_op", ruim_ballenchaos_op},
	{"verlaat_doolhof", verlaat_doolhof},
	{NULL}
};
