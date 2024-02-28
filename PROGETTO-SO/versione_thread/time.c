#include "include.h"

/* ----------------------------------------------
          TIMER PARTITA
   ----------------------------------------------*/
void *time_thread(void *time_data)
{

    objectData *timeData = (objectData *)time_data;
    objectData time;
    time.id = timeData->id;
    time.thread_id = pthread_self();
    time.time_left = timeData->time_left;
    
    insertObject(time);
    // Invio del dato
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
