#include "include.h"

/* ----------------------------------------------
          PLANT
   ----------------------------------------------*/

pthread_mutex_t plantMutex,plantBulletMutex;

void *plant_thread(void *plant_data)
{
    // Estrazione dell'id passato alla funzione
    objectData plantData = *(objectData *)plant_data;
    plantData.thread_id = pthread_self();

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
            objectData *plantBullet = (objectData *)malloc(sizeof(plantBullet));
            objectData plant_bullet = *plantBullet;
            plant_bullet.x = plantData.x;
            plant_bullet.y = plantData.y;
            plant_bullet.id = plantData.id+3;
            if(plantData.plant_bullet_timer == PLANT_BULLET_RELOAD_EASY)plant_bullet.plant_bullet_delay = PLANT_BULLET_DELAY_EASY;
            else if(plantData.plant_bullet_timer == PLANT_BULLET_RELOAD_NORMAL)plant_bullet.plant_bullet_delay = PLANT_BULLET_DELAY_NORMAL;
            else if(plantData.plant_bullet_timer == PLANT_BULLET_RELOAD_HARD)plant_bullet.plant_bullet_delay = PLANT_BULLET_DELAY_HARD;
            // Creazione thread
            /*
            if (pthread_create(&plant_bullet_thread_t, NULL, &plant_bullet_thread, &plant_bullet) != 0)
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
    objectData *plantBullet = (objectData *)data;
    objectData plant_bullet = *plantBullet;

    plant_bullet.thread_id = pthread_self();

    insertObject(plant_bullet);
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

