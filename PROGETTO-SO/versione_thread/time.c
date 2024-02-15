#include "include.h"

/* ----------------------------------------------
          TIMER PARTITA
   ----------------------------------------------*/
void *time_thread(void *a)
{

    // Tempo massimo in base alla difficoltà
    pthread_mutex_lock(&mutex);
    if (gamedata.difficulty == EASY)
    {
        time_left = TIMELIMIT_EASY;
    }
    else if (gamedata.difficulty == NORMAL)
    {
        time_left = TIMELIMIT_NORMAL;
    }
    else
    {
        time_left = TIMELIMIT_HARD;
    }
    pthread_mutex_unlock(&mutex);

    // Invio del dato
    while (should_not_exit)
    {
        sleep(1);
        // Aggiorna IL TIMER
        pthread_mutex_lock(&mutex);
        time_left--;
        pthread_mutex_unlock(&mutex);
    }
}
