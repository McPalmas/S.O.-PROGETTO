#include "include.h"
#include <math.h>

/* ----------------------------------------------
          CROCODILE
   ----------------------------------------------*/
// Funzione per la gestione del thread crocodile
void *crocodile_thread(void *data)
{
    // srand sulla base del thread
    unsigned int thread_id = (unsigned int)(size_t)pthread_self();
    srand(thread_id);

    // Estrazione dei dati passati alla funzione
    args *receivedData = (args *)data;
    objectData crocodileData = receivedData->object;
    objectData river_flowData[RIVER_LANES_NUMBER] = receivedData->riverFlow;
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

            // Se crocodile supera i margini dello schermo, muore
            if (crocodileData.x <= 1 || crocodileData.x + CROCODILE_W >= MAXX + CROCODILE_W)
            {
                crocodileData.is_crocodile_alive = false;
            }

            // Aggiornamento posizione di crocodile
            if (crocodileData.direction == RIGHT)
                crocodileData.x += 1;
            else
                crocodileData.x -= 1;
        }
        else
        {
            // Aggiornamento posizione di crocodile
            if (crocodileData.direction == RIGHT)
                crocodileData.x += 1;
            else
                crocodileData.x -= 1;

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
        }

        insertObject(crocodileData);
        usleep(crocodileData.crocodile_speed);
    }
}
