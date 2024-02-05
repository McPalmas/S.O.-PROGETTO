#include "include.h"


/* ----------------------------------------------   
		  TIMER PARTITA 
   ----------------------------------------------*/ 
void time_thread(int difficulty){

    // Tempo massimo in base alla difficoltà
    if (difficulty == EASY) {
        time_limit = TIMELIMIT_EASY;
    }
    else if (difficulty == NORMAL){
        time_limit = TIMELIMIT_NORMAL;
    }
    else{
        time_limit = TIMELIMIT_HARD;     
    }


    // Invio del dato
    while(1){
        // Aggiorna lo stato
        time_limit--;
        
        sleep(1);
    }

}
