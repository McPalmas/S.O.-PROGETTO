#include "include.h"

/* ----------------------------------------------
          FROG
   ----------------------------------------------*/

// Funzione per la gestione del thread frog
void *frog_thread(void *data)
{
    // Estrazione dei dati
    objectData frog = *(objectData *)data;
    frog.thread_id = pthread_self();

    // Posizione y delle corsie del fiume
    int laneY[RIVER_LANES_NUMBER];
    for (int i = 0; i < RIVER_LANES_NUMBER; ++i)
    {

        laneY[i] = SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + i * 2;
    }

    // Inizializzazione variabili
    int timer_counter = 0;
    int last_frog_y = frog.y;
    pthread_t frog_bullet_thread_t;
    int frog_start_x = frog.x;
    int frog_start_y = frog.y;

    // Ciclo di esecuzione di Frog
    while (should_not_exit)
    {
        while (block)
        {
            int c = getch();
            if (c == 'q')
            {
                block = false;
                system("aplay ../SUONI/riverSound.wav > /dev/null 2>&1 &");
            }
        }

        // Attende l'input, altrimenti continua senza bloccare il gioco
        timeout(1);
        // Acquisizione input
        int c = getch();
        frog.frog_is_shooting = false;
        // Comandi Frog
        switch (c)
        {
        // Movimenti
        case KEY_UP:
            frog.y -= 2;
            system("aplay ../SUONI/jump.wav > /dev/null 2>&1 &");
            break;
        case KEY_DOWN:
            if (frog.y < frog_start_y)
            {
                frog.y += 2;
                system("aplay ../SUONI/jump.wav > /dev/null 2>&1 &");
            }
            break;
        case KEY_LEFT:
            if (frog.x > MINX + 2)
                frog.x -= 1;
            break;
        case KEY_RIGHT:
            if (frog.x < MAXX - 3)
                frog.x += 1;
            break;
        // Proiettile
        case SPACE:
            frog.frog_is_shooting = true;
            break;
        case 'q':
            if (!block)
            {
                block = true;
                system("killall aplay");
                pausePrint();
            }
            break;
        }

        // Controllo se Frog è nel fiume
        bool inRiver = false;
        int delay = 0;
        for (int i = 0; i < RIVER_LANES_NUMBER; i++)
        {
            if (frog.y == laneY[i])
            {
                frog.flow_speed = frog.river_flow[i].speed;
                frog.direction = frog.river_flow[i].direction;
                delay = frog.flow_speed;
                inRiver = true;
                break;
            }
        }

        // Gestione movimento Frog nel fiume
        if (inRiver)
        {
            timer_counter++;
            if (timer_counter == delay / 2000 - 27)
            {
                if (last_frog_y == frog.y)
                {
                    if (frog.direction == RIGHT && frog.x < MAXX - 3)
                        frog.x += 1;
                    else if (frog.direction == LEFT && frog.x > MINX + 2)
                        frog.x -= 1;
                    timer_counter = 0;
                }
            }
        }

        // Inserisco Frog
        insertObject(frog);
        last_frog_y = frog.y;
        usleep(1000); // ogni millisecondo
    }
}

/* ----------------------------------------------
          FROG BULLET
   ----------------------------------------------*/
// Funzione per la gestione del processo frog_bullet
void *frog_bullet_thread(void *a)
{
    // Estrazione dei dati
    objectData *frogBullet = (objectData *)a;
    objectData frog_bullet = *frogBullet;
    // Inizializzazione variabili
    frog_bullet.thread_id = pthread_self();
    frog_bullet.frog_bulletisactive = true;
    system("aplay ../SUONI/lasershot.wav > /dev/null 2>&1 &");
    insertObject(frog_bullet);
    while (should_not_exit)
    {
        while (block)
        {
        }
        pthread_testcancel();
        frog_bullet.y -= 1;

        insertObject(frog_bullet);
        usleep(FROG_BULLET_DELAY);
    }
}
