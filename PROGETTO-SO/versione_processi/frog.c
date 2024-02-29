#include "include.h"

/* ----------------------------------------------
          FROG
   ----------------------------------------------*/
// Funzione per la gestione del processo frog
void frog_process(int pipe[2], int pipe_shoot[2], int pipe_canshoot[2], int pipe_frogoncrocodile[2], int difficulty)
{

    // Gestione pipe
    close(pipe[0]);
    close(pipe_shoot[0]);
    close(pipe_canshoot[1]);
    close(pipe_frogoncrocodile[1]);

    // Definizione variabili
    objectData frog;
    objectData frog_bullet;
    objectData crocodile;

    // Posizione di partenza della rana
    int frog_start_y = SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + (RIVER_LANES_NUMBER * 2) + START_ZONE_HEIGHT - 3;

    // Inizializzazione dei parametri di Frog
    frog.id = FROG_ID;
    frog.y = frog_start_y;
    frog.x = FROG_START;
    frog.frog_canshoot = true;
    frog.frog_candie = true;

    // Comunica lo stato di Frog dopo l'inizializzazione
    write(pipe[1], &frog, sizeof(objectData));

    // Ciclo di esecuzione di Frog
    while (1)
    {
        // Legge il dato dalla pipe, se è presente
        if (read(pipe_canshoot[0], &frog_bullet, sizeof(objectData)) != -1)
        {
            // e un altro proiettile non è attivo
            if (frog_bullet.frog_bulletisactive == false)
            {
                // Frog può sparare
                frog.frog_canshoot = true;
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
            if(frog.x > MINX + 2)
                 frog.x -= 1;
            break;
        case KEY_RIGHT:
            if(frog.x < MAXX - 3)
                frog.x += 1;
            break;
        // Proiettile
        case SPACE:
            if (frog.frog_canshoot)
            {
                frog.frog_canshoot = false;
                write(pipe_shoot[1], &frog, sizeof(objectData));
                system("aplay ../SUONI/lasershot.wav > /dev/null 2>&1");
            }
            break; 
        }         

        // Legge il dato dalla pipe, se è presente
        if (read(pipe_frogoncrocodile[0], &crocodile, sizeof(objectData)) != -1)
        {
            // Se Frog è sulla schiena del coccodrillo
            if (frog.y == crocodile.y && (frog.x > crocodile.x + 2 - 2 * crocodile.direction && frog.x < (crocodile.x + CROCODILE_W - 1 - 2 * crocodile.direction)))
            {
                // la posizione di Frog è aggiornata
                if (crocodile.direction == LEFT){
                    if(frog.x > MINX +2)
                        frog.x -= 1;
                }else{
                    if(frog.x < MAXX - 3)
                        frog.x += 1;
                }
            }
        }
        

        // Comunica il nuovo stato della rana
        write(pipe[1], &frog, sizeof(objectData));

        usleep(1000);
    }
}

/* ----------------------------------------------
          FROG BULLET
   ----------------------------------------------*/
// Funzione per la gestione del processo frog_bullet
void frog_bullet_process(int p[2], int p_shoot[2], int p_can_shoot[2], int p_destroy_frog_bullet[2])
{

    // Gestione pipe
    close(p[0]);
    close(p_shoot[1]);
    close(p_can_shoot[0]);
    close(p_destroy_frog_bullet[1]);

    // Definizione delle variabili
    objectData frog_bullet;
    objectData frog_bullet_data;
    objectData frog;

    // Inizializzazione oggetto Frog Bullet
    frog_bullet.id = FROG_BULLET_ID;
    frog_bullet.x = frog.x;
    frog_bullet.y = frog.y;
    frog_bullet.frog_bulletisactive = false;

    // Ciclo di esecuzione di Frog Bullet
    while (1)
    {

        // Se è presente il dato (Frog ha sparato)
        read(p_shoot[0], &frog, sizeof(objectData));

        // Se Frog ha sparato
        if (frog.frog_canshoot == false)
        {
            // Aggiorna la posizione di Froog Bullet
            frog_bullet.x = frog.x;
            frog_bullet.y = frog.y;
            // Attiva Frog Bullet
            frog_bullet.frog_bulletisactive = true;

            // Fintanto che Frog Bullet è attivo
            while (frog_bullet.frog_bulletisactive == true)
            {

                // Legge il dato, se è presente
                if (read(p_destroy_frog_bullet[0], &frog_bullet_data, sizeof(objectData)) != -1)
                {
                    // disattiva Frog Bullet
                    frog_bullet.frog_bulletisactive = false;
                }

                // Se Frog Bullet è oltre i margini dello schermo
                if (frog_bullet.y <= SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT)
                {
                    // disattiva Frog Bullet
                    frog_bullet.frog_bulletisactive = false;
                }
                else if (frog_bullet.frog_bulletisactive)
                {
                    // Aggiorna la posizione di Frog Bullet
                    frog_bullet.y -= 1;
                }

                // Comunica lo stato
                write(p[1], &frog_bullet, sizeof(objectData));

                // Delay di Frog Bullet
                usleep(FROG_BULLET_DELAY);
            }

            // Aggiorna lo stato
            frog_bullet.frog_bulletisactive = false;
        }

        // Comunica lo stato
        write(p_can_shoot[1], &frog_bullet, sizeof(objectData));
    }
}
