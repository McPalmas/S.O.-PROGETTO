#include "include.h"
#include <math.h>

/* ----------------------------------------------
          CROCODILE
   ----------------------------------------------*/
// Funzione per la gestione del thread crocodile
void *crocodile_thread(void *data)
{
    // Estrazione dei dati passati alla funzione
    args *receivedData = (args *)data;
    objectData crocodileData = receivedData->object;
    crocodileData.thread_id = pthread_self();
    objectData river_flowData[RIVER_LANES_NUMBER];
    for (int i = 0; i < RIVER_LANES_NUMBER; i++)
    {
        river_flowData[i].flow_speed = receivedData->riverFlow[i].flow_speed;
        river_flowData[i].direction = receivedData->riverFlow[i].direction;
    }
    int is_bad_probability = crocodileData.crocodile_is_bad_probability;
    insertObject(crocodileData);
    // Ciclo di esecuzione di crocodile
    while (should_not_exit)
    {

        while (block)
        {
        }

        // Se crocodile è vivo
        if (crocodileData.is_crocodile_alive)
        {
            // Aggiornamento posizione di crocodile
            if (crocodileData.direction == RIGHT)
                crocodileData.x += 1;
            else
                crocodileData.x -= 1;
            // Se crocodile supera i margini dello schermo, muore
            if (crocodileData.x <= 1 || crocodileData.x + CROCODILE_W >= MAXX + CROCODILE_W)
            {
                crocodileData.is_crocodile_alive = false;
            }
        }
        else
        {
            // Se crocodile è morto, viene inizializzato a una nuova corsia
            if (crocodileData.flow_number == 0 || crocodileData.flow_number % 2 == 0)
            {
                crocodileData.y += 2;
                crocodileData.flow_number++;
            }
            else if (crocodileData.flow_number % 2 == 1)
            {
                crocodileData.y -= 2;
                crocodileData.flow_number--;
            }

            // Inizializzazione delle nuove variabili di crocodile
            crocodileData.crocodile_speed = river_flowData[crocodileData.flow_number].flow_speed;
            crocodileData.direction = river_flowData[crocodileData.flow_number].direction;
            crocodileData.is_crocodile_alive = true;

            if (crocodileData.direction == LEFT)
                crocodileData.x = MAXX - 2;
            else
                crocodileData.x = 2;

            crocodileData.is_crocodile_alive = true;
            crocodileData.crocodile_is_good = getRandomBoolean(crocodileData.crocodile_is_bad_probability); // test
        }

        insertObject(crocodileData);
        usleep(crocodileData.crocodile_speed);
    }
}
