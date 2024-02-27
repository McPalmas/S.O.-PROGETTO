#include "include.h"

/* ----------------------------------------------
          FROG
   ----------------------------------------------*/
// Funzione per la gestione del thread frog
void *frog_thread(void *frogData)
{
    objectData frog = *(objectData *)frogData;
    pthread_t frog_thread, frog_bullet_thread_t;

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
                // Aggiornamento variabile
                frog.frog_canshoot = false;
                // Inizializzazione proiettile
                bulletData *frogBullet = (bulletData *)malloc(sizeof(frogBullet));
                objectData frog_bullet;
                frog_bullet.x = frog.x;
                frog_bullet.y = frog.y;
                frog_bullet.id = FROG_BULLET_ID;
                // Creazione thread
                if (pthread_create(&frog_bullet_thread_t, NULL, &frog_bullet_thread, &frog_bullet) != 0)
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
    bulletData *frogBullet = (bulletData *)a;

    objectData frog_bullet;
    frog_bullet.x = frogBullet->x;
    frog_bullet.y = frogBullet->y;

    // Estrazione dell'id passato alla funzione
    unsigned int thread_id = (unsigned int)(size_t)pthread_self();
    srand(thread_id);

    insertObject(frog_bullet);

    while (should_not_exit)
    {
        while (block)
        {
        }

        frog_bullet.y -= 1;

        insertObject(frog_bullet);
        usleep(FROG_BULLET_DELAY);
    }
}
