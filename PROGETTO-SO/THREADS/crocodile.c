#include "include.h"
#include <math.h>

/* ----------------------------------------------
          CROCODILE
   ----------------------------------------------*/
void *crocodile_thread(void *id)
{

    // estrazione dell'id passato alla funzione
    int crocodileIndex = *((int *)id);

    // srand sulla base del thread
    unsigned int thread_id = (unsigned int)(size_t)pthread_self();
    srand(thread_id);

    pthread_mutex_lock(&mutex);
    // Inizializzazione oggetto crocodile (Da completare)
    crocodiles[crocodileIndex].is_crocodile_immersing = false;
    crocodiles[crocodileIndex].is_crocodile_alive = true;
    pthread_mutex_unlock(&mutex);

    // Ciclo di esecuzione di crocodile
    while (should_not_exit)
    {

        // Se crocodile supera i margini dello schermo, muore
        if (crocodiles[crocodileIndex].x <= 1 || crocodiles[crocodileIndex].x + CROCODILE_W >= MAXX + CROCODILE_W)
        {
            crocodiles[crocodileIndex].is_crocodile_alive = false;
        }

        // Se crocodile è vivo
        if (crocodiles[crocodileIndex].is_crocodile_alive)
        {
            // Se crocodile è stato colpito, diventa buono -> prima era gestito da pipe.
            pthread_mutex_lock(&mutex);
            // Aggiornamento posizione di crocodile
            if (crocodiles[crocodileIndex].direction == RIGHT)
                crocodiles[crocodileIndex].x += 1;
            else
                crocodiles[crocodileIndex].x -= 1;
            pthread_mutex_unlock(&mutex);

            pthread_mutex_lock(&mutex);
            if (frog.y == crocodiles[crocodileIndex].y && (frog.x > (crocodiles[crocodileIndex].x) && frog.x < (crocodiles[crocodileIndex].x + CROCODILE_W - 2)))
            {
                if (crocodiles[crocodileIndex].direction == RIGHT)
                    frog.x += 1;
                else
                    frog.x -= 1;
            }
            pthread_mutex_unlock(&mutex);
        }
        else
        {
            pthread_mutex_lock(&mutex);
            // Aggiornamento posizione di crocodile
            if (crocodiles[crocodileIndex].direction == RIGHT)
                crocodiles[crocodileIndex].x += 1;
            else
                crocodiles[crocodileIndex].x -= 1;
            pthread_mutex_unlock(&mutex);

            pthread_mutex_lock(&mutex);
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
            pthread_mutex_unlock(&mutex);

            pthread_mutex_lock(&mutex);
            // Inizializzazione delle nuove variabili di crocodile
            crocodiles[crocodileIndex].crocodile_speed = river_flows[crocodiles[crocodileIndex].flow_number].speed;
            crocodiles[crocodileIndex].direction = river_flows[crocodiles[crocodileIndex].flow_number].direction;
            crocodiles[crocodileIndex].crocodile_is_good = rand() % 2;
            crocodiles[crocodileIndex].is_crocodile_alive = true;
            crocodiles[crocodileIndex].is_crocodile_immersing = false;
            crocodiles[crocodileIndex].crocodile_immersion_timer = getRandomInt(100);
            if (crocodiles[crocodileIndex].direction == LEFT)
                crocodiles[crocodileIndex].x = MAXX - 2;
            else
                crocodiles[crocodileIndex].x = 2;
            pthread_mutex_unlock(&mutex);
        }

        usleep(crocodiles[crocodileIndex].crocodile_speed);
    }
}
