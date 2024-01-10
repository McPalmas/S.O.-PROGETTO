#include "include.h"


/* ----------------------------------------------   
		  TIMER PARTITA 
   ----------------------------------------------*/ 
void time_process(int p[2], int difficulty){

    // Gestione pipe
    close(p[0]);

    Frog time;
    
    // Tempo massimo
    int time_limit;

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

    // Inizializzazione
    time.id = TIME_ID;
    time.time_left = time_limit + 1;

    // Invio del dato
    while(1){
        // Aggiorna lo stato
        time.time_left--;
        write(p[1], &time, sizeof(Frog));
        sleep(1);
    }

}
