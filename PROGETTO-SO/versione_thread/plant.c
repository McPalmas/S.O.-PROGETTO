#include "include.h"

/* ----------------------------------------------
          PLANT
   ----------------------------------------------*/
void *plant_thread(void *plant_data)
{
    // Estrazione dell'id passato alla funzione
    objectData plantData = *(objectData *)plant_data;

    // srand sulla base del thread
    unsigned int thread_id = (unsigned int)(size_t)pthread_self();
    srand(thread_id);
    srand(getpid()); // Quale ci va dei due?

    pthread_t plant_bullet_thread_t;
    int i;
    
    insertObject(plantData);
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
            plantBullet->timer = plantData.plant_bullet_timer;
            // Creazione thread
            if (pthread_create(&plant_bullet_thread_t, NULL, &plant_bullet_thread, plantBullet) != 0)
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
    int bulletIndex = plantBullet->id;
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

