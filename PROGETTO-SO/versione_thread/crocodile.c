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
    objectData *crocodileData = (objectData *)data;

    // Inizializzazione oggetto crocodile
    objectData crocodiles[N_CROCODILE];
    int crocodileIndex = *((int *)crocodileData->id);
    crocodiles[crocodileIndex].flow_number = crocodileData->flow_number;
    crocodiles[crocodileIndex].crocodile_speed = crocodileData->crocodile_speed;
    crocodiles[crocodileIndex].direction = crocodileData->direction;
    crocodiles[crocodileIndex].y = crocodileData->y;
    crocodiles[crocodileIndex].is_crocodile_alive = true;

    insertObject(crocodiles[crocodileIndex]);

    // Ciclo di esecuzione di crocodile
    while (should_not_exit)
    {

        while (block)
        {
        }

        // Se crocodile è vivo
        if (crocodiles[crocodileIndex].is_crocodile_alive)
        {

            // Se crocodile supera i margini dello schermo, muore
            if (crocodiles[crocodileIndex].x <= 1 || crocodiles[crocodileIndex].x + CROCODILE_W >= MAXX + CROCODILE_W)
            {
                crocodiles[crocodileIndex].is_crocodile_alive = false;
            }

            // Aggiornamento posizione di crocodile
            if (crocodiles[crocodileIndex].direction == RIGHT)
                crocodiles[crocodileIndex].x += 1;
            else
                crocodiles[crocodileIndex].x -= 1;
        }
        else
        {
            // Aggiornamento posizione di crocodile
            if (crocodiles[crocodileIndex].direction == RIGHT)
                crocodiles[crocodileIndex].x += 1;
            else
                crocodiles[crocodileIndex].x -= 1;

            // Se crocodile è morto, viene inizializzato a una nuova corsia
            if (crocodiles[crocodileIndex].flow_number == 0 || crocodiles[crocodileIndex].flow_number % 2 == 0)
            {
                crocodiles[crocodileIndex].y += 2;
                crocodiles[crocodileIndex].flow_number++;
            }
            else if (crocodiles[crocodileIndex].flow_number % 2 == 1)
            {
                crocodiles[crocodileIndex].y -= 2;
                crocodiles[crocodileIndex].flow_number--;
            }

            // Inizializzazione delle nuove variabili di crocodile
            crocodiles[crocodileIndex].crocodile_speed = river_flows[crocodiles[crocodileIndex].flow_number].flow_speed;
            crocodiles[crocodileIndex].direction = river_flows[crocodiles[crocodileIndex].flow_number].direction;
            crocodiles[crocodileIndex].is_crocodile_alive = true;

            if (crocodiles[crocodileIndex].direction == LEFT)
                crocodiles[crocodileIndex].x = MAXX - 2;
            else
                crocodiles[crocodileIndex].x = 2;
        }

        insertObject(crocodiles[crocodileIndex]);
        usleep(crocodiles[crocodileIndex].crocodile_speed);
    }
}
