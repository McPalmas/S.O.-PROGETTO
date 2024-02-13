#include "include.h"

/* ----------------------------------------------
          LOGIC PARTITA E PROCESSI
   ----------------------------------------------*/
// Funzione per inizializzare la partita
void initialize_game()
{

    srand(time(NULL));

    gamedata.game_lost = false;
    gamedata.game_won = false;
    should_not_exit = true;

    // dichiarazione threads
    pthread_t frog_t;
    pthread_t frog_bullet_t;
    pthread_t time_t;
    pthread_t plant_t[N_PLANTS];
    pthread_t plantBullet_t[N_PLANT_BULLETS];
    pthread_t crocodile_t[N_CROCODILE];
    pthread_t gameManche_t;

    // definizione degli id delle piante
    for (int i = 0; i < N_PLANTS; i++)
    {
        pthread_mutex_lock(&mutex);
        plants[i].id = i;
        plant_bullets[i].id = i;
        pthread_mutex_unlock(&mutex);
    }

    // definizione degli id dei coccodrilli
    for (int i = 0; i < N_CROCODILE; i++)
    {
        pthread_mutex_lock(&mutex);
        crocodiles[i].id = i;
        pthread_mutex_unlock(&mutex);
    }

    // Inizializzazione dei flussi del fiume
    initialize_river_flows();
    crocodiles_inizializer();

    // i threads, quando verranno creati, potranno avviare il loro ciclo, che terminerà quando verrà modificato il valore di questa variabile
    // should_not_exit = true;

    //* CREAZIONE THREADS -------------------

    pthread_create(&frog_t, NULL, &frog_thread, NULL);

    for (int i = 0; i < N_PLANTS; i++)
    {
        pthread_create(&plant_t[i], NULL, &plant_thread, (void *)&plants[i].id);
        pthread_create(&plantBullet_t[i], NULL, &plant_bullet_thread, (void *)&plant_bullets[i].id);
    }

    for (int i = 0; i < N_CROCODILE; i++)
    {
        pthread_create(&crocodile_t[i], NULL, &crocodile_thread, (void *)&crocodiles[i].id);
    }

    pthread_create(&time_t, NULL, &time_thread, NULL);

    // pthread_create(&gameManche_t, NULL, &gameManche_thread, NULL);
    gameManche();
    //* TERMINAZIONE THREADS -------------------

    pthread_join(frog_t, NULL);
    pthread_join(frog_bullet_t, NULL);
    pthread_join(time_t, NULL);

    for (int i = 0; i < N_PLANTS; i++)
    {
        pthread_join(plant_t[i], NULL);
        pthread_join(plantBullet_t[i], NULL);
    }

    for (int i = 0; i < N_CROCODILE; i++)
    {
        pthread_join(crocodile_t[i], NULL);
    }

    // pthread_join(gameManche_t, NULL);

    // system("echo 'Messaggio di log: prima di analyze_data' > log.txt");
    analyze_data();
}

/* ----------------------------------------------
          CONTINUA O TERMINA LA PARTITA
   ----------------------------------------------*/
void analyze_data()
{

    int taken_dens = 0;

    erase();
    bkgd(COLOR_PAIR(WHITE_WHITE)); /* Setta il background color dello schermo */
    attron(COLOR_PAIR(BLACK_WHITE));

    // conta il numero di tane occupate
    for (int i = 0; i < N_DENS; i++)
        if (gamedata.dens[i] == true)
            taken_dens++;

    if (gamedata.game_won)
    {
        // se ha occupato tutte le tane si va al menu della vittoria
        if (taken_dens >= N_DENS)
        {
            system("aplay ../SUONI/victory.wav > /dev/null 2>&1 &");
            endGameMenu(1);
        }
        else
        { // altrimenti stampa relativa alle tane occupate e inizio manche successiva
            mvprintw(MAXY / 3, MAXX / 2 - 8, "Tane raggiunte: %d", taken_dens);
            mvprintw(MAXY / 3 + 1, MAXX / 2 - 8, "Vite rimanenti: %d", gamedata.player_lives);
            refresh();
            system("aplay ../SUONI/tanaRaggiunta.wav > /dev/null 2>&1");
            // tempo di attesa prima del caricamento della schermata successiva
            sleep(2);

            initialize_game();
        }
    }
    else if (gamedata.game_lost)
    {
        // pthread_mutex_lock(&mutex);
        gamedata.player_lives--;
        // pthread_mutex_lock(&mutex);

        if (gamedata.player_lives <= 0)
        { // se ha esaurito le vite si va al menu della sconfitta
            system("aplay ../SUONI/gameover.wav > /dev/null 2>&1 &");
            endGameMenu(0);
        }
        else
        { // altrimenti stampa relativa al numero di vite rimanenti
            mvprintw(MAXY / 3, MAXX / 2 - 8, "Tane raggiunte: %d", taken_dens);
            mvprintw(MAXY / 3 + 1, MAXX / 2 - 8, "Vite rimanenti: %d", gamedata.player_lives);
            refresh();
            system("aplay ../SUONI/death.wav > /dev/null 2>&1");
            // tempo di attesa prima del caricamento della schermata successiva
            sleep(2);

            initialize_game();
        }
    }
}

/* ----------------------------------------------
         GESTIONE MANCHE, STAMPE E COLLISIONI
   ----------------------------------------------*/
// void *gameManche_thread(void *id)
void gameManche()
{
    int start_dens[] = {16, 27, 38, 49, 60};
    int crocodile_immersion_timer = getRandomInt(100);

    // posizone di partenza della rana
    /* int frog_start_y = SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + (RIVER_LANES_NUMBER * 2) + START_ZONE_HEIGHT - 2;
     int frog_start_x = FROG_START;

     // bool ausiliaria per controllare se una tana è occupata o meno
     _Bool frog_in_a_den = false;

     //posizione y dei dati relativi al player, ovvero score, time e vite
     int top_score_height =  1 ;
     int bottom_score_height = TOTAL_HEIGHT +1 ;

     // inizializzazioni per evitare collisioni iniziali
     frog.frog_candie = false;
     frog.x = frog_start_x;
     frog.y = frog_start_y;

     // frog bullet
     frog_bullet.bulletisactive = false;

    // Plant
    for (int i = 0; i < N_PLANTS; i++)
    {
        plants[i].y = SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT;
    }

    // Plant bullet
    for (int i = 0; i < N_PLANT_BULLETS; i++)
    {
        plant_bullets[i].bulletisactive = false;
    }

    // crocodiles_inizializer();*/

    while (should_not_exit)
    {
        // COLLISIONI E MORTI --------------------------------------------------------------------------------------

        // se la rana oltrepassa il bordo - ok
        pthread_mutex_lock(&mutex);
        if (frog.x - 2 < MINX || frog.x + 2 > MAXX - 1 || frog.y < SCORE_ZONE_HEIGHT)
        {
            gamedata.player_score -= DEATH_SCORE;
            frog.frog_candie = false;
            gamedata.game_lost = true;
        }
        pthread_mutex_unlock(&mutex);

        // se la rana tocca una pianta - ok
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < N_PLANTS; i++)
        {
            if ((frog.y == plants[i].y + 1 || frog.y == plants[i].y) && (frog.x - 2 >= plants[i].x - (FROG_W - 1) && frog.x + 2 <= plants[i].x + (FROG_W + 1)))
            {
                frog.frog_candie = false;
                gamedata.game_lost = true;
                gamedata.player_score += DEATH_SCORE;
            }
        }
        pthread_mutex_unlock(&mutex);
        

        // Sgestita in crocodile.c
        
      /*  pthread_mutex_lock(&mutex);
        for (int i = 0; i < N_CROCODILE; i++)
        {
            if (frog.y == crocodiles[i].y && (frog.x > (crocodiles[i].x + 2) && frog.x < (crocodiles[i].x + CROCODILE_W - 1)))
            {
                if (crocodiles[i].direction == RIGHT)
                    frog.x += 1;
                else
                    frog.x -= 1;
            }
        }
        pthread_mutex_unlock(&mutex);*/
        
/*
        // Se la rana è nel fiume e si trova sopra un coccodrillo cattivo - non funziona il timer, va subito a zero
        pthread_mutex_lock(&mutex);
        if (frog.y < SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + (RIVER_LANES_NUMBER * 2) && frog.y > SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT)
        {
            for (int i = 0; i < N_CROCODILE; i++)
            {
                if (frog.frog_candie && frog.y == crocodiles[i].y && (frog.x > crocodiles[i].x+1 && frog.x < crocodiles[i].x + CROCODILE_W-1))
                {
                    onCrocodile = true;
                    if (!crocodiles[i].crocodile_is_good)
                    {
                        crocodile_immersion_timer--;

                        switch (gamedata.difficulty)
                        {
                        case EASY:
                            if (crocodile_immersion_timer <= (CROCODILE_IMMERSION_TIME_EASY / 2))
                                crocodiles[i].is_crocodile_immersing = true;
                            break;
                        case NORMAL:
                            if (crocodile_immersion_timer <= (CROCODILE_IMMERSION_TIME_NORMAL / 2))
                                crocodiles[i].is_crocodile_immersing = true;
                            break;

                        case HARD:
                            if (crocodile_immersion_timer <= (CROCODILE_IMMERSION_TIME_HARD / 2))
                                crocodiles[i].is_crocodile_immersing = true;
                            break;
                        }
                        if (crocodile_immersion_timer <= 0)
                        {
                            frog.frog_candie = false;
                            gamedata.game_lost = true;
                        };
                    }else crocodile_immersion_timer=getRandomInt(100);
                    break;
                }else onCrocodile = false;
            }
            if(!onCrocodile){
                frog.frog_candie = false;
                gamedata.game_lost = true;
            };
        }
        pthread_mutex_unlock(&mutex);
  */      

        // RANA - TANA --------------------------------------------------------------------------------------

        // se la rana passa nella zona delle tane - ok
        pthread_mutex_lock(&mutex);
        if (frog.y < SCORE_ZONE_HEIGHT + 2)
        {
            // per ogni tana
            for (int i = 0; i < N_DENS; i++)
            {
                // se la rana tocca in una tana
                if (frog.frog_candie && ((frog.x - 2 >= start_dens[i] && frog.x - 2 < start_dens[i] + FROG_W) || (frog.x + 2 >= start_dens[i] && frog.x + 2 < start_dens[i] + FROG_W)))
                {
                    if (gamedata.dens[i] == false)
                    {
                        // aumenta il punteggio in base a difficoltà e tempo traascorso
                        if (gamedata.difficulty == EASY)
                        {
                            gamedata.player_score += DEN_SCORE_EASY + (MAX_BONUS_SCORE - ((MAX_BONUS_SCORE * (TIMELIMIT_EASY - time_left)) / TIMELIMIT_EASY));
                        }
                        else if (gamedata.difficulty == NORMAL)
                        {
                            gamedata.player_score += DEN_SCORE_NORMAL + (MAX_BONUS_SCORE - ((MAX_BONUS_SCORE * (TIMELIMIT_EASY - time_left)) / TIMELIMIT_NORMAL));
                        }
                        else
                        {
                            gamedata.player_score += DEN_SCORE_HARD + (MAX_BONUS_SCORE - ((MAX_BONUS_SCORE * (TIMELIMIT_EASY - time_left)) / TIMELIMIT_HARD));
                        }

                        frog.frog_candie = false;
                        // Chiudi la tana e setta win a true per il reload del game
                        frog.frog_candie = false;
                        gamedata.game_won = true;
                        gamedata.dens[i] = true;
                    }
                    else
                    {
                        frog.frog_candie = false;
                        gamedata.game_lost = true;
                        if (gamedata.player_score > DEATH_SCORE) // sottrazzione del punteggio dopo la morte
                            gamedata.player_score -= DEATH_SCORE;
                        else
                            gamedata.player_score = 0;
                    }
                }
            }
        }
        pthread_mutex_unlock(&mutex);

        // proiettili rana - piante - ok
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < N_PLANTS; i++)
        {
            // se è presente plant e la rana può sparare
            if (plants[i].plant_isalive)
            {
                // se il proiettile della rana collide con plant
                if (frog_bullet.bulletisactive && (frog_bullet.y <= plants[i].y + 1) && (frog_bullet.x >= plants[i].x && frog_bullet.x <= plants[i].x + 2))
                {
                    // aumenta lo score
                    if (gamedata.difficulty == EASY)
                    {
                        gamedata.player_score += DEN_SCORE_EASY;
                    }
                    else if (gamedata.difficulty == NORMAL)
                    {
                        gamedata.player_score += DEN_SCORE_NORMAL;
                    }
                    else
                    {
                        gamedata.player_score += DEN_SCORE_HARD;
                    }

                    // disattiva il proiettile e uccidi plant
                    frog_bullet.bulletisactive = false;
                    frog.frog_canshoot = true;
                    plants[i].plant_isalive = false;
                }
            }
        }
        pthread_mutex_unlock(&mutex);

        // proiettili rana - proiettili piante - distrugge i proiettili, ma solo graficamente
        /*
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < N_PLANT_BULLETS; i++)
        {
            if (plant_bullets[i].bulletisactive && frog_bullet.bulletisactive)
            {
                // se collide col proiettile della rana
                if ((plant_bullets[i].y == frog_bullet.y) && (plant_bullets[i].x == frog_bullet.x))
                {
                    // disattiva entrambi i proiettili
                    plant_bullets[i].bulletisactive = false;
                    frog_bullet.bulletisactive = false;
                    frog.frog_canshoot = true;
                }
            }
        }
        pthread_mutex_unlock(&mutex);
        */
        
        // proiettili rana - coccodrilli - ok
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < N_CROCODILE; i++)
        {
            // se il proiettile sta sul coccodrillo corrente
            if (frog_bullet.bulletisactive && (crocodiles[i].y == frog_bullet.y) && (frog_bullet.x >= crocodiles[i].x && frog_bullet.x <= crocodiles[i].x + CROCODILE_W))
            {
                if (!crocodiles[i].crocodile_is_good)
                {
                    // se il coccodrillo è cattivo diventa buono
                    crocodiles[i].crocodile_is_good = true;
                    frog_bullet.bulletisactive = false;
                    frog.frog_canshoot = true;
                    break;
                }
                // comunica al frog bullet di distruggere il proiettile
                frog_bullet.bulletisactive = false;
                frog.frog_canshoot = true;
            }
        }
        pthread_mutex_unlock(&mutex);
        
        // MORTE RANA PER TEMPO --------------------------------------------------------------------------------------

        // se il tempo scende a zero perdi la manche
        pthread_mutex_lock(&mutex);
        if (time_left <= 0 && frog.frog_candie)
        {
            frog.frog_candie = false;
            gamedata.game_lost = true;
        }
        pthread_mutex_unlock(&mutex);

        // se la manche è stata vinta o persa, fai terminare tutti i processi cambiando il valore di questa variabile
        pthread_mutex_lock(&mutex);
        if (gamedata.game_lost || gamedata.game_won)
        {
            should_not_exit = false;
        }
        pthread_mutex_unlock(&mutex);
    }

    if (gamedata.player_score <= 0)
    {
        gamedata.player_score = 0;
    }
    analyze_data();
}

/* ----------------------------------------------
         INIZIALIZZAZIONE COCCODRILLI
   ----------------------------------------------*/
void crocodiles_inizializer()
{
    int crocodileIndex = 0;
    // Itera su ciascun fiume

    for (int riverIndex = 0; riverIndex < RIVER_LANES_NUMBER; riverIndex++)
    {
        // Assegna 3 coccodrilli a ciascun fiume
        for (int i = 0; i < CROCODILES_PER_RIVER; i++)
        {
            // Assegna l'indice del fiume al coccodrillo
            crocodiles[crocodileIndex].flow_number = riverIndex;
            // Assegna speed e direction dal fiume corrispondente
            crocodiles[crocodileIndex].crocodile_speed = river_flows[riverIndex].speed;
            crocodiles[crocodileIndex].direction = river_flows[riverIndex].direction;
            crocodiles[crocodileIndex].y = SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + (riverIndex * 2);
            crocodiles[crocodileIndex].is_crocodile_alive = true;
            crocodiles[crocodileIndex].is_crocodile_immersing = false;
            switch (gamedata.difficulty)
            {
            case EASY:
                crocodiles[crocodileIndex].crocodile_is_good = getRandomBoolean(CROCODILE_IS_BAD_PROBABILITY_EASY);
                break;
            case NORMAL:
                crocodiles[crocodileIndex].crocodile_is_good = getRandomBoolean(CROCODILE_IS_BAD_PROBABILITY_NORMAL);
                break;
            case HARD:
                crocodiles[crocodileIndex].crocodile_is_good = getRandomBoolean(CROCODILE_IS_BAD_PROBABILITY_HARD);
                break;
            default:
                break;
            }
            // Avanza all'elemento successivo nell'array di coccodrilli
            crocodileIndex++;
        }
    }

    int minDistance = 10;
    for (int riverIndex = 0; riverIndex < RIVER_LANES_NUMBER; riverIndex++)
    {
        int maxRiverStartX = MAXX - (CROCODILES_PER_RIVER * CROCODILE_W) - minDistance * (CROCODILES_PER_RIVER - 1);
        // Calcola la posizione casuale del primo coccodrillo nella riga
        int riverStartX = MINX + rand() % (maxRiverStartX - MINX + 1);
        // Assegna la posizione a ciascun coccodrillo
        for (int i = 0; i < CROCODILES_PER_RIVER; i++)
        {
            // Calcola la posizione casuale del coccodrillo
            crocodiles[riverIndex * CROCODILES_PER_RIVER + i].x = riverStartX;
            // Avanza la posizione del prossimo coccodrillo garantendo una distanza minima
            riverStartX += CROCODILE_W + minDistance;
        }
    }
}

/* ----------------------------------------------
         INIZIALIZZAZIONE FIUMI
   ----------------------------------------------*/
void initialize_river_flows()
{
    // Inizializza i flussi del fiume con direzioni e velocità casuali
    for (int i = 0; i < RIVER_LANES_NUMBER; ++i)
    {
        river_flows[i].direction = rand() % 2;
        switch (gamedata.difficulty)
        {
        case EASY:
            river_flows[i].speed = MIN_RIVER_SPEED_EASY + rand() % (MAX_RIVER_SPEED_EASY - MIN_RIVER_SPEED_EASY + 1);
            break;
        case NORMAL:
            river_flows[i].speed = MIN_RIVER_SPEED_NORMAL + rand() % (MAX_RIVER_SPEED_NORMAL - MIN_RIVER_SPEED_NORMAL + 1);
            break;
        case HARD:
            river_flows[i].speed = MIN_RIVER_SPEED_HARD + rand() % (MAX_RIVER_SPEED_HARD - MIN_RIVER_SPEED_HARD + 1);
            break;
        default:
            break;
        }
    }
}

// Function to generate a random boolean with a given probability
bool getRandomBoolean(float probability)
{
    if (probability < 0 || probability > 1)
        return false; // Error
    return rand() > probability * ((float)RAND_MAX + 1.0);
}

int getRandomInt(int min)
{
    int randomTimer;

    switch (gamedata.difficulty)
    {
    case (EASY):
        randomTimer = rand() % (CROCODILE_IMMERSION_TIME_EASY + 1) + min;
        break;
    case (NORMAL):
        randomTimer = rand() % (CROCODILE_IMMERSION_TIME_NORMAL + 1) + min;
        break;
    case (HARD):
        randomTimer = rand() % (CROCODILE_IMMERSION_TIME_HARD + 1) + min;
        break;
    default:
        break;
    }

    return randomTimer;
}
