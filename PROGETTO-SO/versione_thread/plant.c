#include "include.h"

/* ----------------------------------------------
          PLANT
   ----------------------------------------------*/
void *plant_thread(void *id)
{
    // Estrazione dell'id passato alla funzione
    int plantIndex = *((int *)id);
    // srand sulla base del thread
    unsigned int thread_id = (unsigned int)(size_t)pthread_self();
    srand(thread_id);
    srand(getpid()); // Quale ci va dei due?

    
    int plant_bullet_timer;
    int i;

    // Inizializzazione piante
    objectData plants[N_PLANTS];
    if (plants[plantIndex].id == 0)
        plants[plantIndex].x = PLANT_0_START;
    else if (plants[plantIndex].id == 1)
        plants[plantIndex].x = PLANT_1_START;
    else if (plants[plantIndex].id == 2)
        plants[plantIndex].x = PLANT_2_START;
    plants[plantIndex].plant_canshoot = true;
    plants[plantIndex].y = SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT;

    insertObject(plants[plantIndex]); // Potrebbe non servire
    // Ciclo di esecuzione della pianta
    while (should_not_exit)
    {
        while (block)
        {
        }

        // Aggiornamento timer
        plant_bullet_timer--;
        // Se il timer è scaduto
        if (plant_bullet_timer <= 0)
        {
            // Inizializzazione proiettile
            bulletData *plantBullet = (bulletData *)malloc(sizeof(plantBullet));
            plantBullet->x = plants[plantIndex].x;
            plantBullet->y = plants[plantIndex].y;
            plantBullet->id = PLANT_BULLET_ID_0; // Questa non so come gestirla
            // Creazione thread
            if (pthread_create(&frog_bullet_thread, NULL, &frogBullett, plantBullet) != 0)
            {
                _exit(1);
            }
        }

        insertObject(plants[plantIndex]);
        sleep(1); // 1 secondo
    }
}

/* ----------------------------------------------
          PLANT BULLET
   ----------------------------------------------*/
void *plant_bullet_thread(void *data)
{
    // Estrazione dei dati passati alla funzione
    bulletData *plantBullet = (bulletData *)data;

    // Inizializzazione proiettile
    objectData plant_bullet[N_PLANT_BULLETS];
    int bulletIndex = *((int *)plantBullet->id);
    plant_bullet[bulletIndex].x = plantBullet->x;
    plant_bullet[bulletIndex].y = plantBullet->y;

    // Estrazione dell'id passato alla funzione
    unsigned int thread_id = (unsigned int)(size_t)pthread_self();
    srand(thread_id);

    insertObject(plant_bullet[bulletIndex]);

    // Suono
    system("aplay ../SUONI/lasershot.wav > /dev/null 2>&1");

    insertObject(plant_bullet[bulletIndex]);

    // Ciclo di esecuzione del proiettile
    while (should_not_exit)
    {
        while (block)
        {
        }

        // Aggiornamento posizione
        plant_bullet[bulletIndex].y += 1;

        insertObject(plant_bullet[bulletIndex]);

        usleep(plant_bullet[bulletIndex].plant_bullet_delay);
    }
}

