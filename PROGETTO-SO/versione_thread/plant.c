#include "include.h"

/* ----------------------------------------------
          PLANT
   ----------------------------------------------*/
void *plant_thread(void *plant_data)
{
    // Estrazione dell'id passato alla funzione
    objectData *plantData = (objectData *)plant_data;
    int plantIndex = *((int *)plantData->id);
    // srand sulla base del thread
    unsigned int thread_id = (unsigned int)(size_t)pthread_self();
    srand(thread_id);
    srand(getpid()); // Quale ci va dei due?

    int i;

    // Inizializzazione piante
    if (plantData.id == 0)
        plantData.x = PLANT_0_START;
    else if (plantData.id == 1)
        plantData.x = PLANT_1_START;
    else if (plantData.id == 2)
        plantData.x = PLANT_2_START;
    plantData.plant_canshoot = true;
    plantData.y = SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT;

    insertObject(plantData[plantIndex]); // Potrebbe non servire
    // Ciclo di esecuzione della pianta
    while (should_not_exit)
    {
        while (block)
        {
        }

        // Aggiornamento timer
        plantData.plant_bullet_timer--;
        // Se il timer è scaduto
        if (plantData.plant_bullet_timer <= 0)
        {
            // Inizializzazione proiettile
            bulletData *plantBullet = (bulletData *)malloc(sizeof(plantBullet));
            plantBullet->x = plantData.x;
            plantBullet->y = plantData.y;
            plantBullet->id = PLANT_BULLET_ID_0; // Questa non so come gestirla
            // Creazione thread
            if (pthread_create(&frog_bullet_thread, NULL, &frogBullett, plantBullet) != 0)
            {
                _exit(1);
            }
        }

        insertObject(plantData);
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
    objectData plant_bullet;
    int bulletIndex = *((int *)plantBullet->id);
    plant_bullet.x = plantBullet->x;
    plant_bullet.y = plantBullet->y;

    // Estrazione dell'id passato alla funzione
    unsigned int thread_id = (unsigned int)(size_t)pthread_self();
    srand(thread_id);

    // Suono
    system("aplay ../SUONI/lasershot.wav > /dev/null 2>&1");

    insertObject(plant_bullet);

    // Ciclo di esecuzione del proiettile
    while (should_not_exit)
    {
        while (block)
        {
        }

        // Aggiornamento posizione
        plant_bullet.y += 1;

        insertObject(plant_bullet);

        usleep(plant_bullet.plant_bullet_delay);
    }
}

