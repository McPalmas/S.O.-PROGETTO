#include "include.h"


/* ----------------------------------------------   
		  TIMER PARTITA 
   ----------------------------------------------*/ 
void* time_thread(void *a, int difficulty){

    // Tempo massimo in base alla difficoltà
    pthread_mutex_lock(&mutex);
    if (difficulty == EASY) {
        time_limit = TIMELIMIT_EASY;
    }
    else if (difficulty == NORMAL){
        time_limit = TIMELIMIT_NORMAL;
    }
    else{
        time_limit = TIMELIMIT_HARD;     
    }
    pthread_mutex_unlock(&mutex);

    // Invio del dato
    while(1){
        // Aggiorna lo stato
        pthread_mutex_lock(&mutex);
        time_limit--;
        pthread_mutex_unlock(&mutex);
        
        sleep(1);
    }

}
