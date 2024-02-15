#include "include.h"

/* ----------------------------------------------
          FROG
   ----------------------------------------------*/
// Funzione per la gestione del thread frog
void *frog_thread(void *a)
{

    // Posizione di partenza della rana
    int frog_start_y = SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + (RIVER_LANES_NUMBER * 2) + START_ZONE_HEIGHT - 3;
    int frog_start_x = FROG_START;

    // Inizializzazione dei parametri di Frog
    pthread_mutex_lock(&mutex);
    frog.y = frog_start_y;
    frog.x = frog_start_x;
    frog.frog_canshoot = true;
    frog.frog_candie = true;
    pthread_mutex_unlock(&mutex);

    // Ciclo di esecuzione di Frog
    while (should_not_exit)
    {
        while(block){
            int c = getch();
            if(c == 'q'){
                 block = false;
                 system("aplay ../SUONI/riverSound.wav > /dev/null 2>&1 &");
            }
        }
        
        // Stampa di tutto lo schermo di gioco
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
                frog_bullet.y = frog.y - 1;
                frog_bullet.x = frog.x;
                frog.frog_canshoot = false;
                frog_bullet.bulletisactive = true;
                system("aplay ../SUONI/lasershot.wav > /dev/null 2>&1 &");
            }
            break;
        case 'q': 
            if(!block){
		 block = true;
		 system("killall aplay");
	    }
	    break;
        }
        pthread_mutex_unlock(&mutex);

        for (int i = 0; i < N_PLANT_BULLETS; i++)
        {
            if (frog_bullet.bulletisactive && plant_bullets[i].bulletisactive > 0)
            {
                if (frog_bullet.x == plant_bullets[i].x && frog_bullet.y == plant_bullets[i].y)
                {   
                    // Se il proiettile della rana colpisce un proiettile delle piante, entrambi vengono disattivati
                    pthread_mutex_lock(&mutex);
                    frog_bullet.bulletisactive = false;
                    plant_bullets[i].bulletisactive = false;
                    pthread_mutex_unlock(&mutex);
                }
            }
        }

        usleep(1000);
    }
}

/* ----------------------------------------------
          FROG BULLET
   ----------------------------------------------*/
// Funzione per la gestione del processo frog_bullet
void *frog_bullet_thread(void *a)
{   
    unsigned int thread_id = (unsigned int)(size_t)pthread_self();
    srand(thread_id);

    while (should_not_exit)
    {
        while(block){}
        
        if(frog_bullet.bulletisactive){
		while (frog_bullet.y > SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT)
		{
		    // Aggiornamento posizione del proiettile
		    pthread_mutex_lock(&mutex);
		    frog_bullet.y -= 1;
		    pthread_mutex_unlock(&mutex);

		    usleep(FROG_BULLET_DELAY);
		}
		// Se il proiettile esce dallo schermo, viene disattivato
		pthread_mutex_lock(&mutex);
		frog_bullet.bulletisactive = false;
		frog.frog_canshoot = true;
		pthread_mutex_unlock(&mutex);
        }
    }
}

/* ----------------------------------------------
          STAMMPA
   ----------------------------------------------*/
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
