#include "include.h"

/* ----------------------------------------------
          FROG
   ----------------------------------------------*/
// Funzione per la gestione del thread frog
void *frog_thread(void *a)
{
    objectData frog;
    pthread_t frog_thread, frog_bullet_thread;

    // Posizione di partenza della rana
    int frog_start_y = SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + (RIVER_LANES_NUMBER * 2) + START_ZONE_HEIGHT - 3;
    int frog_start_x = FROG_START;

    // Inizializzazione dei parametri di Frog
    frog.y = frog_start_y;
    frog.x = frog_start_x;
    frog.frog_canshoot = true;
    // frog.frog_candie = true; Sarà gestita in logic

    insertObject(frog);

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
            frog.x -= 1;
            break;
        case KEY_RIGHT:
            frog.x += 1;
            break;
        // Proiettile
        case SPACE:
            if (frog.frog_canshoot)
            {
                /*
                frog_bullet.y = frog.y - 1;
                frog_bullet.x = frog.x; -> Questo non so ancora come gestirlo.
                frog_bullet.frog_bulletisactive = true; // Da sostituire con creazione thread
                */

                // Aggiornamento variabile
                frog.frog_canshoot = false;
                // Inizializzazione proiettile
                bulletData *frogBullet = (bulletData *)malloc(sizeof(frogBullet));
                frogBullet->x = frog.x;
                frogBullet->y = frog.y;
                frogBullet->id = FROG_BULLET_ID;
                // Creazione thread
                if (pthread_create(&frog_bullet_thread, NULL, &frogBullett, frogBullet) != 0)
                {
                    _exit(1);
                }
                // Suono
                system("aplay ../SUONI/lasershot.wav > /dev/null 2>&1 &");
            }
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

        /*
        for (int i = 0; i < N_PLANT_BULLETS; i++)
        {
            if (frog_bullet.bulletisactive && plant_bullets[i].bulletisactive > 0)
            {
                if (frog_bullet.x == plant_bullets[i].x && frog_bullet.y == plant_bullets[i].y)
                {
                    // Se il proiettile della rana colpisce un proiettile delle piante, entrambi vengono disattivati
                    frog_bullet.bulletisactive = false;
                    plant_bullets[i].bulletisactive = false;
                }
            }
        }
        */
        // Da gestire in logic

        insertObject(frog);
        usleep(1000);
    }
}

/* ----------------------------------------------
          FROG BULLET
   ----------------------------------------------*/
// Funzione per la gestione del processo frog_bullet
void *frog_bullet_thread(void *a)
{
    objectData frog_bullet;

    // Estrazione dell'id passato alla funzione
    unsigned int thread_id = (unsigned int)(size_t)pthread_self();
    srand(thread_id);

    while (should_not_exit)
    {
        while (block)
        {
        }
        /*
        if (frog_bullet.frog_bulletisactive)
        {
            while (frog_bullet.y > SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT) // Logic
            {
                // Aggiornamento posizione del proiettile
            }
            // Se il proiettile esce dallo schermo, viene disattivato
            // frog_bullet.frog_bulletisactive = false;
            // frog.frog_canshoot = true; -> Questo non so ancora come gestirlo.
        }
        */
        // Da gestire in logic con la distruzione dei thread, meno la 146

        frog_bullet.y -= 1;

        insertObject(frog_bullet);
        usleep(FROG_BULLET_DELAY);
    }
}
