#include "include.h"

/* ----------------------------------------------
          TIMER PARTITA
   ----------------------------------------------*/
void *time_thread(void *time_data)
{

    objectData *timeData = (objectData *)time_data;
    objectData time = *timeData;
    time.time_left = timeData->time_left;
    time.id = TIME_ID;
    time.thread_id = pthread_self();
    
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
