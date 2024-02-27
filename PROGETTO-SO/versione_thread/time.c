#include "include.h"

/* ----------------------------------------------
          TIMER PARTITA
   ----------------------------------------------*/
void *time_thread(void *a)
{
    objectData time;
    // Tempo massimo in base alla difficoltà
    if (gamedata.difficulty == EASY)
    {
        time.time_left = TIMELIMIT_EASY;
    }
    else if (gamedata.difficulty == NORMAL)
    {
        time.time_left = TIMELIMIT_NORMAL;
    }
    else
    {
        time.time_left = TIMELIMIT_HARD;
    }

    // Invio del dato
    while (should_not_exit)
    {
    	while(block){}
    	
        sleep(1);
        // Aggiorna IL TIMER -> Probabilmente anche lui va gestito con produttore consumatore e non con globale
        time.time_left--;
        insertObject(time);
    }
}
