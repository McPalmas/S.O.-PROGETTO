#include "include.h"

/* ----------------------------------------------
          PLANT
   ----------------------------------------------*/

pthread_mutex_t plantBulletMutex;
pthread_cond_t cond_var_plant;
int plant_thread_created = 0;

void *plant_thread(void *plant_data)
{
    // Estrazione dell'id passato alla funzione
    objectData *plant = (objectData *)plant_data;
    objectData plantData = *plant;
    plantData.thread_id = pthread_self();
    plantData.plant_bullet_timer = plant->plant_bullet_timer;

    int plant_timer = plantData.plant_bullet_timer;

    pthread_t plant_bullet_thread_t;
    int i;
    insertObject(plantData);
    // Ciclo di esecuzione della pianta
    while (should_not_exit)
    {
        while (block)
        {
        }
        // Se il timer è scaduto
        if (plantData.plant_bullet_timer <= 0 && !plant_thread_created)
        {
            pthread_mutex_lock(&plantBulletMutex);
            // Inizializzazione proiettile
            objectData *plantBullet = (objectData *)malloc(sizeof(plantBullet));
            objectData plant_bullet = *plantBullet;
            plant_bullet.x = plantData.x;
            plant_bullet.y = plantData.y+1;
            plant_bullet.id = plantData.id+3;
            if(plant_timer == PLANT_BULLET_RELOAD_EASY)plant_bullet.plant_bullet_delay = PLANT_BULLET_DELAY_EASY;
            else if(plant_timer == PLANT_BULLET_RELOAD_NORMAL)plant_bullet.plant_bullet_delay = PLANT_BULLET_DELAY_NORMAL;
            else if(plant_timer == PLANT_BULLET_RELOAD_HARD)plant_bullet.plant_bullet_delay = PLANT_BULLET_DELAY_HARD;
            // Creazione thread
            
            if (pthread_create(&plant_bullet_thread_t, NULL, &plant_bullet_thread, &plant_bullet) != 0)
            {
                _exit(1);
            }
            plant_thread_created = 1;
            plantData.plant_bullet_timer = plant_timer;
            pthread_mutex_unlock(&plantBulletMutex);

            system("aplay ../SUONI/lasershot.wav > /dev/null 2>&1");
        }else plantData.plant_bullet_timer--;

        insertObject(plantData);
        sleep(1); // 1 secondo
    }
}

/* ----------------------------------------------
          PLANT BULLET
   ----------------------------------------------*/
void *plant_bullet_thread(void *data)
{
    pthread_cleanup_push(plantBulletDeletion, NULL);
    // Inizializzazione proiettile
    objectData *plantBullet = (objectData *)data;
    objectData plant_bullet = *plantBullet;

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
    pthread_cleanup_pop(1);
}

void plantBulletDeletion()
{
    pthread_mutex_lock(&plantBulletMutex);
    plant_thread_created = 0;             // Il thread è terminato, quindi possiamo creare un altro thread
    pthread_cond_signal(&cond_var_plant); // Notifica il main thread che il thread è terminato
    pthread_mutex_unlock(&plantBulletMutex);
    pthread_exit(0);
}
