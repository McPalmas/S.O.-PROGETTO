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
        if (plantData.plant_bullet_timer == 0)
        {
            // Inizializzazione proiettile
            objectData plantBullet;
            plantBullet.x = plantData.x;
            plantBullet.y = plantData.y;
            plantBullet.id = plantData.id+3;
            if(plantData.plant_bullet_timer == PLANT_BULLET_RELOAD_EASY)plantBullet.plant_bullet_delay = PLANT_BULLET_DELAY_EASY;
            else if(plantData.plant_bullet_timer == PLANT_BULLET_RELOAD_NORMAL)plantBullet.plant_bullet_delay = PLANT_BULLET_DELAY_NORMAL;
            else if(plantData.plant_bullet_timer == PLANT_BULLET_RELOAD_HARD)plantBullet.plant_bullet_delay = PLANT_BULLET_DELAY_HARD;
            // Creazione thread
            /*
            if (pthread_create(&plant_bullet_thread_t, NULL, &plant_bullet_thread, &plantBullet) != 0)
            {
                _exit(1);
            }*/
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

    // Inizializzazione proiettile
    objectData plant_bullet = *(objectData *)data;
    int bulletIndex = plant_bullet.id;

    // Estrazione dell'id passato alla funzione
    unsigned int thread_id = (unsigned int)(size_t)pthread_self();
    srand(thread_id);

    // Suono
    system("aplay ../SUONI/lasershot.wav > /dev/null 2>&1");
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

