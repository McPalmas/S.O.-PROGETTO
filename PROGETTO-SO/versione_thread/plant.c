#include "include.h"

/* ----------------------------------------------
          PLANT
   ----------------------------------------------*/
void *plant_thread(void *id)
{

    srand(getpid());

    // Estrazione dell'id passato alla funzione
    int plantIndex = *((int *)id);
    // srand sulla base del thread
    unsigned int thread_id = (unsigned int)(size_t)pthread_self();
    srand(thread_id);

    int plant_bullet_timer;
    int plant_respawn_timer;
    int i;

    // Inizializzazione piante
    pthread_mutex_lock(&mutex);
    if (plants[plantIndex].id == 0)
        plants[plantIndex].x = PLANT_0_START;
    else if (plants[plantIndex].id == 1)
        plants[plantIndex].x = PLANT_1_START;
    else if (plants[plantIndex].id == 2)
        plants[plantIndex].x = PLANT_2_START;
    plants[plantIndex].plant_canshoot = true;
    plants[plantIndex].plant_isalive = true;
    plants[plantIndex].y = SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT;
    plant_bullet_timer = getPlantReloadTimer(PLANT_BULLET_RELOAD_MIN);
    plant_respawn_timer = PLANT_RESPAWN_MIN + rand() % (PLANT_RESPAWN_MAX - PLANT_RESPAWN_MIN + 1);
    pthread_mutex_unlock(&mutex);

    // Ciclo di esecuzione della pianta
    while (should_not_exit)
    {

        if (plants[plantIndex].plant_isalive)
        {
            // Aggiornamento timer
            pthread_mutex_lock(&mutex);
            plant_bullet_timer--;
            pthread_mutex_unlock(&mutex);

            // Se il timer è scaduto
            pthread_mutex_lock(&mutex);
            if (plant_bullet_timer <= 0)
            {
                plant_bullets[plantIndex].bulletisactive = true;
                plant_bullet_timer = getPlantReloadTimer(PLANT_BULLET_RELOAD_MIN);
            }
            pthread_mutex_unlock(&mutex);

            // Se la rana è sulla pianta
            pthread_mutex_lock(&mutex);
            if (frog.y == plants[plantIndex].y && (frog.x >= plants[plantIndex].x && frog.x < plants[plantIndex].x + 2))
            {
                frog.frog_candie = false;
                gamedata.game_lost = true;
            }
            pthread_mutex_unlock(&mutex);
        }
        else
        {
            // Aggiornamento timer
            pthread_mutex_lock(&mutex);
            plant_respawn_timer--;
            pthread_mutex_unlock(&mutex);

            pthread_mutex_lock(&mutex);
            if (plant_respawn_timer <= 0)
            { // a timer scaduto la pianta deve rinascere
                plants[plantIndex].plant_isalive = true;
                plant_respawn_timer = PLANT_RESPAWN_MIN + rand() % (PLANT_RESPAWN_MAX - PLANT_RESPAWN_MIN + 1);
            }
            pthread_mutex_unlock(&mutex);
        }
        // Gestione proiettile
        pthread_mutex_lock(&mutex);
        if (plant_bullets[plantIndex].y == frog_bullet.y && plant_bullets[plantIndex].x == frog_bullet.x)
        {
            plant_bullets[plantIndex].bulletisactive = false;
        }
        pthread_mutex_unlock(&mutex);
        sleep(1); // 1 secondo
    }
}

/* ----------------------------------------------
          PLANT BULLET
   ----------------------------------------------*/
void *plant_bullet_thread(void *id)
{

    // Estrazione dell'id passato alla funzione
    int plantBulletIndex = *((int *)id);
    // srand sulla base del thread
    unsigned int thread_id = (unsigned int)(size_t)pthread_self();
    srand(thread_id);

    // Inizializzazione
    int plant_bullet_delay;

    // Inizializzazione proiettile
    pthread_mutex_lock(&mutex);
    plant_bullets[plantBulletIndex].x = plants[plantBulletIndex].x + 1;
    plant_bullets[plantBulletIndex].y = plants[plantBulletIndex].y + 1;
    switch (gamedata.difficulty)
    {
    case EASY:
        plant_bullet_delay = PLANT_BULLET_DELAY_EASY;
        break;
    case NORMAL:
        plant_bullet_delay = PLANT_BULLET_DELAY_NORMAL;
        break;
    case HARD:
        plant_bullet_delay = PLANT_BULLET_DELAY_HARD;
        break;
    default:
        break;
    }
    pthread_mutex_unlock(&mutex);

    // Suono
    system("aplay ../SUONI/lasershot.wav > /dev/null 2>&1");

    // Ciclo di esecuzione del proiettile
    while (should_not_exit)
    {
        if (plant_bullets[plantBulletIndex].bulletisactive)
        {
            // Posizionamento proiettile
            pthread_mutex_lock(&mutex);
            plant_bullets[plantBulletIndex].x = plants[plantBulletIndex].x + 1;
            plant_bullets[plantBulletIndex].y = plants[plantBulletIndex].y + 1;
            pthread_mutex_unlock(&mutex);

            while (plant_bullets[plantBulletIndex].y < MAXY - 12)
            {
                // Aggiornamento posizione
                pthread_mutex_lock(&mutex);
                plant_bullets[plantBulletIndex].y += 1;
                pthread_mutex_unlock(&mutex);

                usleep(plant_bullet_delay);
            }
            // Disattivazione proiettile
            pthread_mutex_lock(&mutex);
            plant_bullets[plantBulletIndex].bulletisactive = false;
            pthread_mutex_unlock(&mutex);
        }
    }
}

/**
 * Utility
 */

// Restituisce un timer per il prossimo sparo della pianta
int getPlantReloadTimer(int min)
{
    int randomTimer;

    switch (gamedata.difficulty)
    {
    case (EASY):
        randomTimer = rand() % (min + 1) + PLANT_BULLET_RELOAD_EASY;
        break;
    case (NORMAL):
        randomTimer = rand() % (min + 1) + PLANT_BULLET_RELOAD_NORMAL;
        break;
    case (HARD):
        randomTimer = rand() % (min + 1) + PLANT_BULLET_RELOAD_HARD;
        break;
    default:
        break;
    }

    return randomTimer;
}
