#include "include.h"

/* ----------------------------------------------
          FROG
   ----------------------------------------------*/
// Funzione per la gestione del thread frog
void *frog_thread(void *frogData)
{
    objectData frog = *(objectData *)frogData;
    frog.thread_id = pthread_self();

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
                objectData *frogBullet = (objectData *)malloc(sizeof(frogBullet));
                objectData frog_bullet = *frogBullet;
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
    objectData *frogBullet = (objectData *)a;
    objectData frog_bullet = *frogBullet;
    frog_bullet.thread_id = pthread_self();
    frog_bullet.frog_bulletisactive = true;
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
