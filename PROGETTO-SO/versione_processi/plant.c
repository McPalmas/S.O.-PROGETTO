#include "include.h"

/* ----------------------------------------------
          PLANT
   ----------------------------------------------*/
void plant_process(int id, int pipe[2], int pipe_plant_is_dead[2], int pipe_destroy_plant_bullet[2], int difficulty)
{

    // Gestione pipe
    close(pipe[0]);
    close(pipe_plant_is_dead[1]);
    
    srand(getpid());
    // Posizione oggetti di gioco
    objectData plant;
    objectData plant_data;
    pid_t plant_bullet;
    int plant_bullet_timer;
    int plant_respawn_timer;
    int i;

    // Inizializzazione piante
    plant.id = id;
    plant.plant_canshoot = true;
    plant.plant_isalive = true;
    plant.y = SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT;
    plant_bullet_timer = getPlantReloadTimer(PLANT_BULLET_RELOAD_MIN, difficulty);
    plant_respawn_timer = PLANT_RESPAWN_MIN + rand() % (PLANT_RESPAWN_MAX - PLANT_RESPAWN_MIN + 1);
    if (plant.id == PLANT_ID_0)
        plant.x = PLANT_0_START;
    else if (plant.id == PLANT_ID_1)
        plant.x = PLANT_1_START;
    else if (plant.id == PLANT_ID_2)
        plant.x = PLANT_2_START;

    // Ciclo di esecuzione della pianta
    while (1)
    {        
        
        if (read(pipe_plant_is_dead[0], &plant_data, sizeof(objectData)) != -1)
            if(plant_data.id == plant.id)
                plant = plant_data;
   
      
      
        if (plant.plant_isalive)
        {        
            plant_bullet_timer--;
            // Se il timer è scaduto
            if (plant_bullet_timer <= 0)
            {
                plant_bullet_timer = getPlantReloadTimer(PLANT_BULLET_RELOAD_MIN, difficulty);
                // Crea un nuovo proiettile
                plant_bullet = fork();
                // Se è il processo figlio
                if (plant_bullet == 0)
                {
                    // Chiama la funzione per il proiettile
                    plant_bullet_process(pipe, plant, pipe_destroy_plant_bullet, difficulty);
                }
                plant_bullet_timer = getPlantReloadTimer(PLANT_BULLET_RELOAD_MIN, difficulty);
            }

            // Comunicazione con display
            write(pipe[1], &plant, sizeof(objectData));
        }
        else
        { // se la rana è morta
            plant_respawn_timer--;
            if (plant_respawn_timer <= 0)
            { // a timer scaduto la pianta deve rinascere
                plant.plant_isalive = true;
                plant_respawn_timer = PLANT_RESPAWN_MIN + rand() % (PLANT_RESPAWN_MAX - PLANT_RESPAWN_MIN + 1);
            }
        }
        sleep(1); // 1 secondo
    }
}

/* ----------------------------------------------
          PLANT BULLET
   ----------------------------------------------*/
void plant_bullet_process(int p[2], objectData plant, int pipe_destroy_plant_bullet[2], int difficulty)
{

    // Gestione pipe
    close(p[0]);

    // Inizializzazione
    objectData plant_bullet;
    objectData plant_bullet_data;
    int plant_bullet_delay;

    // Inizializzazione proiettile
    plant_bullet.id = plant.id + 3;
    plant_bullet.x = plant.x + 1;
    plant_bullet.y = plant.y + 1;
    plant_bullet.plant_bulletisactive = true;
    switch (difficulty)
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

    // Comunica con display per la stampa e le collisioni
    write(p[1], &plant_bullet, sizeof(objectData));

    // Suono
    system("aplay ../SUONI/lasershot.wav > /dev/null 2>&1");

    // Ciclo di esecuzione del proiettile
    while (plant_bullet.plant_bulletisactive)
    {
        // Aggiorna lo stato
        if (read(pipe_destroy_plant_bullet[0], &plant_bullet_data, sizeof(objectData)) != -1)
            plant_bullet.plant_bulletisactive = false;

        // Sposta il proiettile
        plant_bullet.y += 1;

        // Se il proiettile esce dall'area di gioco
        if (plant_bullet.y >= MAXY - 12)
        {
            plant_bullet.plant_bulletisactive = false;
        }

        // Comunica con display per la stampa e le collisioni
        write(p[1], &plant_bullet, sizeof(objectData));
        // Delay di avanzamento del proiettile
        usleep(plant_bullet_delay);

        if (!plant_bullet.plant_bulletisactive)
            _exit(0);
    }
    // Comunica con display per la stampa e le collisioni
    write(p[1], &plant_bullet, sizeof(objectData));
}

/**
 * Utility
 */

// Restituisce un timer per il prossimo sparo della pianta
int getPlantReloadTimer(int min, int difficulty)
{
    int randomTimer;

    switch (difficulty)
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
