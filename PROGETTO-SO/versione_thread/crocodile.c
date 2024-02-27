#include "include.h"
#include <math.h>

/* ----------------------------------------------
          CROCODILE
   ----------------------------------------------*/
// Funzione per la gestione del thread crocodile
void *crocodile_thread(void *crocodile_data, void *river_flow_data)
{
    // srand sulla base del thread
    unsigned int thread_id = (unsigned int)(size_t)pthread_self();
    srand(thread_id);

    // Estrazione dei dati passati alla funzione
    objectData *crocodileData = (objectData *)crocodile_data;
    objectData *river_flowData = (objectData *)river_flow_data;

    // Inizializzazione oggetto crocodile
    int crocodileIndex = *((int *)crocodileData->id);
    crocodileData[crocodileIndex].flow_number = crocodileData->flow_number;
    crocodileData[crocodileIndex].crocodile_speed = crocodileData->crocodile_speed;
    crocodileData[crocodileIndex].direction = crocodileData->direction;
    crocodileData[crocodileIndex].y = crocodileData->y;
    crocodileData[crocodileIndex].is_crocodile_alive = true;

    // Salvataggio dati dei flussi del fiume
    objectData river_flows[RIVER_LANES_NUMBER];
    for (int i = 0; i < RIVER_LANES_NUMBER; i++)
    {
        river_flows[i].flow_speed = river_flowData[i].flow_speed;
        river_flows[i].direction = river_flowData[i].direction;
    }

    insertObject(crocodileData[crocodileIndex]);

    // Ciclo di esecuzione di crocodile
    while (should_not_exit)
    {

        while (block)
        {
        }

        // Se crocodile è vivo
        if (crocodileData[crocodileIndex].is_crocodile_alive)
        {

            // Se crocodile supera i margini dello schermo, muore
            if (crocodileData[crocodileIndex].x <= 1 || crocodileData[crocodileIndex].x + CROCODILE_W >= MAXX + CROCODILE_W)
            {
                crocodileData[crocodileIndex].is_crocodile_alive = false;
            }

            // Aggiornamento posizione di crocodile
            if (crocodileData[crocodileIndex].direction == RIGHT)
                crocodileData[crocodileIndex].x += 1;
            else
                crocodileData[crocodileIndex].x -= 1;
        }
        else
        {
            // Aggiornamento posizione di crocodile
            if (crocodileData[crocodileIndex].direction == RIGHT)
                crocodileData[crocodileIndex].x += 1;
            else
                crocodileData[crocodileIndex].x -= 1;

            // Se crocodile è morto, viene inizializzato a una nuova corsia
            if (crocodileData[crocodileIndex].flow_number == 0 || crocodileData[crocodileIndex].flow_number % 2 == 0)
            {
                crocodileData[crocodileIndex].y += 2;
                crocodileData[crocodileIndex].flow_number++;
            }
            else if (crocodileData[crocodileIndex].flow_number % 2 == 1)
            {
                crocodileData[crocodileIndex].y -= 2;
                crocodileData[crocodileIndex].flow_number--;
            }

            // Inizializzazione delle nuove variabili di crocodile
            crocodileData[crocodileIndex].crocodile_speed = river_flows[crocodileData[crocodileIndex].flow_number].flow_speed;
            crocodileData[crocodileIndex].direction = river_flows[crocodileData[crocodileIndex].flow_number].direction;
            crocodileData[crocodileIndex].is_crocodile_alive = true;

            if (crocodileData[crocodileIndex].direction == LEFT)
                crocodileData[crocodileIndex].x = MAXX - 2;
            else
                crocodileData[crocodileIndex].x = 2;
        }

        insertObject(crocodileData[crocodileIndex]);
        usleep(crocodileData[crocodileIndex].crocodile_speed);
    }
}
