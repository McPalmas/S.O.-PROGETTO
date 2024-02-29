#include "include.h"

/* ----------------------------------------------
          PLANT
   ----------------------------------------------*/

void *plant_thread(void *plant_data)
{
    // Estrazione dei dati della pianta
    objectData *plant = (objectData *)plant_data;
    objectData plantData = *plant;

    // Dichiarazione del thread del proiettile
    pthread_t plant_bullet_thread_t;

    // Inizializzazione variabili
    plantData.thread_id = pthread_self();
    plantData.plant_bullet_timer = plant->plant_bullet_timer;
    int bullet_timer = getPlantReloadTimer(PLANT_BULLET_RELOAD_MIN, difficulty);

    insertObject(plantData);
    // Ciclo di esecuzione della pianta
    while (should_not_exit)
    {
        while (block)
        {
        }
        // Se il timer è scaduto
        if (bullet_timer <=0)
        {
            // Inizializzazione proiettile
            objectData *plantBullet = (objectData *)malloc(sizeof(plantBullet));
            objectData plant_bullet = *plantBullet;
            plant_bullet.x = plantData.x;
            plant_bullet.y = plantData.y+1;
            plant_bullet.id = plantData.id+3;
            if(difficulty == EASY)plant_bullet.plant_bullet_delay = PLANT_BULLET_DELAY_EASY;
            else if(difficulty == NORMAL)plant_bullet.plant_bullet_delay = PLANT_BULLET_DELAY_NORMAL;
            else if(difficulty == HARD)plant_bullet.plant_bullet_delay = PLANT_BULLET_DELAY_HARD;
            // Creazione thread
            if (pthread_create(&plant_bullet_thread_t, NULL, &plant_bullet_thread, &plant_bullet) != 0)
            {
                _exit(1);
            }
            bullet_timer = getPlantReloadTimer(PLANT_BULLET_RELOAD_MIN, difficulty);

            system("aplay ../SUONI/lasershot.wav > /dev/null 2>&1");
        }else if(bullet_timer > 0)bullet_timer--;

        insertObject(plantData);
        sleep(1); // 1 secondo
    }
}

/* ----------------------------------------------
          PLANT BULLET
   ----------------------------------------------*/
void *plant_bullet_thread(void *data)
{
    // Estrazione dei dati del proiettile
    objectData *plantBullet = (objectData *)data;
    objectData plant_bullet = *plantBullet;

    // Inizializzazione variabili
    plant_bullet.thread_id = pthread_self();
    plant_bullet.plant_bulletisactive = true;
    plant_bullet.x = plantBullet->x;
    plant_bullet.y = plantBullet->y;
    plant_bullet.plant_bullet_delay = plantBullet->plant_bullet_delay;

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
        usleep(PLANT_BULLET_DELAY_EASY);
    }
}
