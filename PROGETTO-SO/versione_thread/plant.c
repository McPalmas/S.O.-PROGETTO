#include "include.h"

/* ----------------------------------------------
          PLANT
   ----------------------------------------------*/
void *plant_thread(void *id)
{
    objectData plants[N_PLANTS];
    srand(getpid());

    // Estrazione dell'id passato alla funzione
    int plantIndex = *((int *)id);
    // srand sulla base del thread
    unsigned int thread_id = (unsigned int)(size_t)pthread_self();
    srand(thread_id);

    int plant_bullet_timer;
    // int plant_respawn_timer;
    int i;

    // Inizializzazione piante
    if (plants[plantIndex].id == 0)
        plants[plantIndex].x = PLANT_0_START;
    else if (plants[plantIndex].id == 1)
        plants[plantIndex].x = PLANT_1_START;
    else if (plants[plantIndex].id == 2)
        plants[plantIndex].x = PLANT_2_START;
    plants[plantIndex].plant_canshoot = true;
    // plants[plantIndex].plant_isalive = true;
    plants[plantIndex].y = SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT;
    plant_bullet_timer = getPlantReloadTimer(PLANT_BULLET_RELOAD_MIN);

    // plant_respawn_timer = PLANT_RESPAWN_MIN + rand() % (PLANT_RESPAWN_MAX - PLANT_RESPAWN_MIN + 1); // Da gestire in logic

    // Ciclo di esecuzione della pianta
    while (should_not_exit)
    {
        while (block)
        {
        }
        /*
        if (plants[plantIndex].plant_isalive)
        {

            {
                plant_bullets[plantIndex].plant_bulletisactive = true;
                plant_bullet_timer = getPlantReloadTimer(PLANT_BULLET_RELOAD_MIN);
            }

            // Se la rana è sulla pianta
            if (frog.y == plants[plantIndex].y && (frog.x >= plants[plantIndex].x && frog.x < plants[plantIndex].x + 2))
            {
                frog.frog_candie = false;
                gamedata.game_lost = true;
            }
        }
        else
        {
            // Aggiornamento timer
            plant_respawn_timer--;

            if (plant_respawn_timer <= 0)
            { // a timer scaduto la pianta deve rinascere
                plants[plantIndex].plant_isalive = true;
                plant_respawn_timer = PLANT_RESPAWN_MIN + rand() % (PLANT_RESPAWN_MAX - PLANT_RESPAWN_MIN + 1);
            }
        }
        */

        /*
        // Gestione proiettile
        if (plant_bullets[plantIndex].y == frog_bullet.y && plant_bullets[plantIndex].x == frog_bullet.x)
        {
            plant_bullets[plantIndex].plant_bulletisactive = false;
        }*/
        // Da gestire in logic con la distruzione del thread

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
void *plant_bullet_thread(void *id)
{
    objectData plant_bullets[N_PLANTS];
    // Estrazione dell'id passato alla funzione
    int plantBulletIndex = *((int *)id);
    // srand sulla base del thread
    unsigned int thread_id = (unsigned int)(size_t)pthread_self();
    srand(thread_id);

    // Inizializzazione
    int plant_bullet_delay;

    // Inizializzazione proiettile
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

    insertObject(plant_bullets[plantBulletIndex]);

    // Suono
    system("aplay ../SUONI/lasershot.wav > /dev/null 2>&1");

    // Ciclo di esecuzione del proiettile
    while (should_not_exit)
    {
        while (block)
        {
        }
        /*
        if (plant_bullets[plantBulletIndex].plant_bulletisactive)
        {
            // Posizionamento proiettile
            plant_bullets[plantBulletIndex].x = plants[plantBulletIndex].x + 1;
            plant_bullets[plantBulletIndex].y = plants[plantBulletIndex].y + 1;

            while (plant_bullets[plantBulletIndex].y < MAXY - 12)
            {
                // Aggiornamento posizione
                plant_bullets[plantBulletIndex].y += 1;

                usleep(plant_bullet_delay);
            }
            // Disattivazione proiettile
            plant_bullets[plantBulletIndex].plant_bulletisactive = false;
        }*/
        // Da gestire in logic con la distruzione del thread

        // Aggiornamento posizione
        plant_bullets[plantBulletIndex].y += 1;

        insertObject(plant_bullets[plantBulletIndex]);

        usleep(plant_bullet_delay);
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
