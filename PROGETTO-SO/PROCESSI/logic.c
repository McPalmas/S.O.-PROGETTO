#include "include.h"

void game(GameData gamedata){

Crocodile cr;			// righe di prova per le stampe da non prendere sul serio
    cr.x=(MAXX)/2; cr.y=SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + (RIVER_LANES_NUMBER * 2) + START_ZONE_HEIGHT - 5; cr.is_good=false; cr.direction=LEFT;
    
	while(1){				// prova da non considerare
	
		gameField();
    
    frogBody((MAXX)/2, SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + (RIVER_LANES_NUMBER * 2) + START_ZONE_HEIGHT - 2);
    crocodileBody(cr);
    getch();
	}

}
