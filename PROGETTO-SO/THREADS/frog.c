#include "include.h"

/* ----------------------------------------------
          FROG
   ----------------------------------------------*/
// Funzione per la gestione del processo frog
void *frog_thread(void *a)
{

    // posizione di partenza della rana
    int frog_start_y = SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + (RIVER_LANES_NUMBER * 2) + START_ZONE_HEIGHT - 3;
    int frog_start_x = FROG_START;

    // inizializzazione posizione e possibilità di sparare e morire della rana
    pthread_mutex_lock(&mutex);
    frog.y = frog_start_y;
    frog.x = frog_start_x;
    frog.frog_canshoot = true;
    frog.frog_candie = true;
    pthread_mutex_unlock(&mutex);

    // Ciclo di esecuzione di Frog
    while (should_not_exit)
    {
        // stampa di tutto lo schermo di gioco
        pthread_mutex_lock(&mutex);
        printAll();
        pthread_mutex_unlock(&mutex);

        // Attende l'input, altrimenti continua senza bloccare il gioco
        timeout(1);
        // Acquisizione input
        int c = getch();

        pthread_mutex_lock(&mutex);
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
                frog.frog_canshoot = false;
                frog_bullet.x = frog.x + 1;
                frog_bullet.y = frog.y - 1;
                frog_bullet.bulletisactive = true;
                system("aplay ../SUONI/lasershot.wav > /dev/null 2>&1");
            }
            break;
        }
        pthread_mutex_unlock(&mutex);

        pthread_mutex_lock(&mutex);
        for (int i = 0; i < N_PLANT_BULLETS; i++)
        {
            // Se il proiettile colpisce la rana
            if (frog.y == plant_bullets[i].y && (frog.x >= plant_bullets[i].x - 2 && frog.x <= plant_bullets[i].x + 2))
            {
                // Il proiettile viene disattivato
                plant_bullets[i].bulletisactive = false;
                // La rana muore
                frog.frog_candie = false;
                gamedata.game_lost = true;
            }
        }
        pthread_mutex_unlock(&mutex);

        if (frog_bullet.bulletisactive)
        {
            if (frog_bullet.y > SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT)
            {
                frog_bullet.y -= 1;
                usleep(FROG_BULLET_DELAY);
            }
            else
            {
                frog_bullet.bulletisactive = false;
                frog.frog_canshoot = true;
            }
        }
        // Rimosso usleep
        usleep(1000);
    }
}

//* FROG BULLET ----------------------------------------------

// Funzione per la gestione del processo frog_bullet
void *frog_bullet_thread(void *a)
{
    // Funziona anche se questo è vuoto? Tutto ciò che viene fatto avviene nel processo di frog
}

void printAll()
{
    erase();
    // stampa dello sfondo di gioco
    gameField();
    // stampa tane
    printDens(gamedata.dens);

    // STAMPA ELEMENTI ----------------------------------------

    // stampa dei coccodrilli
    for (int i = 0; i < N_CROCODILE; i++)
    {
        if (crocodiles[i].is_crocodile_alive)
            crocodileBody(crocodiles[i]);
    }

    // STAMPA DEL NERO SUI COCCODRILLI USCENTI DAL CAMPO DI GIOCO
    attron(COLOR_PAIR(BLACK_BLACK));
    for (int i = SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT; i < TOTAL_HEIGHT; i++)
    {
        mvprintw(i, MAXX, "            ");
    }
    for (int i = SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT; i < TOTAL_HEIGHT; i++)
    {
        mvprintw(i, 0, "          ");
    }
    attroff(COLOR_PAIR(BLACK_BLACK));

    // stampa piante
    for (int i = 0; i < N_PLANTS; i++)
    {
        if (plants[i].plant_isalive)
            plantBody(plants[i]);
    }

    // stampa dei proiettili delle piante
    for (int i = 0; i < N_PLANT_BULLETS; i++)
    {
        if (plant_bullets[i].bulletisactive)
        {
            plantBullett(plant_bullets[i].x, plant_bullets[i].y);
        }
    }

    // stampa del proiettile della rana
    if (frog_bullet.bulletisactive == true)
    {
        frogBullett(frog_bullet.x, frog_bullet.y);
    }

    // stampa della rana
    frogBody(frog.x, frog.y);

    // stampa dello score a schermo
    attron(COLOR_PAIR(WHITE_BLUE));
    mvprintw(1, SCORE_X, "Score: %d", gamedata.player_score);
    // stampa delle vite a schermo
    mvprintw(TOTAL_HEIGHT + 1, LIFES_X, "Lifes: %d", gamedata.player_lives);
    // stampa del tempo a schermo
    mvprintw(TOTAL_HEIGHT + 1, TIME_X + 15, "Time: %d", time_left);
    attroff(COLOR_PAIR(WHITE_BLUE));

    refresh();
}
