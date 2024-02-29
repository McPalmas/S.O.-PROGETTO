#include "include.h"

/* ----------------------------------------------
          TIMER PARTITA
   ----------------------------------------------*/
void *time_thread(void *time_data)
{
    // Estrazione dei dati del tempo
    objectData *timeData = (objectData *)time_data;
    objectData time = *timeData;

    // Inizializzazione variabili
    time.time_left = timeData->time_left;
    time.id = TIME_ID;
    time.thread_id = pthread_self();
    
    insertObject(time);
    // Ciclo di esecuzione del timer
    while (should_not_exit)
    {
        while (block)
        {
        }
        sleep(1);
        time.time_left--;
        insertObject(time);
    }
}
