#include "include.h"

/* ----------------------------------------------
          LOGIC PARTITA E PROCESSI
   ----------------------------------------------*/
void initialize_game()
{
    // i coccodrilli si immergono a caso sempre non solo se ci salgo sopra
    // Inizializzazione variabili
    srand(time(NULL));
    gamedata.game_lost = false;
    gamedata.game_won = false;
    should_not_exit = true;

    // Dichiarazione threads
    pthread_t frog_t;
    pthread_t frog_bullet_t;
    pthread_t time_t;
    pthread_t plant_t[N_PLANTS];
    pthread_t plantBullet_t[N_PLANT_BULLETS];
    pthread_t crocodile_t[N_CROCODILE];
    pthread_t gameManche_t;

    // Dichiarazione variabili
    objectData frog;
    objectData frogBullet;
    objectData plants[N_PLANTS];
    objectData plant_bullets[N_PLANT_BULLETS];
    objectData crocodiles[N_CROCODILE];

    /**
     * Definizione id
     */
    // Piante e proiettili piante
    for (int i = 0; i < N_PLANTS; i++)
    {
        plants[i].id = i;
        plant_bullets[i].id = i;
    }

    // Coccodrilli
    for (int i = 0; i < N_CROCODILE; i++)
    {
        crocodiles[i].id = i;
    }

    // Inizializzazione dei flussi del fiume
    initialize_river_flows();
    crocodiles_inizializer(crocodiles);

    /**
     * Gestione dei threads
     */

    // Inizializzazione della rana
    pthread_create(&frog_t, NULL, &frog_thread, NULL);
    pthread_create(&frog_bullet_t, NULL, &frog_bullet_thread, NULL);

    pthread_create(&gameManche_t, NULL, &gameManche_thread, NULL);

    // Inizializzazione delle piante e dei proiettili delle piante
    for (int i = 0; i < N_PLANTS; i++)
    {
        pthread_create(&plant_t[i], NULL, &plant_thread, (void *)&plants[i].id);
        pthread_create(&plantBullet_t[i], NULL, &plant_bullet_thread, (void *)&plant_bullets[i].id);
    }

    // Inizializzazione dei coccodrilli
    for (int i = 0; i < N_CROCODILE; i++)
    {
        pthread_create(&crocodile_t[i], NULL, &crocodile_thread, (void *)&crocodiles[i].id);
    }

    // Inizializzazione del timer
    pthread_create(&time_t, NULL, &time_thread, NULL);

    // Inizializzazione del suono del fiume
    system("aplay ../SUONI/riverSound.wav > /dev/null 2>&1 &");

    /**
     * Gestione della manche
     */
    // gameManche();

    /**
     * Join dei threads
     */

    pthread_join(gameManche_t, NULL);
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

    analyze_data();
}

/*-----------------------------------------------------------------------
   PROCEDURA 'CONSUMATORE' - CONSUME OBJECT IN BUFFER
   La procedura si occupa di prelevare/consumare un elemento presente
   all'interno del buffer
   -----------------------------------------------------------------------*/
void removeObject()
{

    sem_wait(&semaphoreSlotFull);     /* Esegue una wait sul semaforo full */
    pthread_mutex_lock(&mutexBuffer); /* Blocca il mutex principale */

    /* SEZIONE CRITICA */
    consumedObject = buffer[out]; /* Preleva l'elemento e scorre l'indice del buffer */
    out = (out + 1) % DIMBUFFER;

    pthread_mutex_unlock(&mutexBuffer); /* Sblocca il mutex principale */
    sem_post(&semaphoreSlotEmpty);      /* Esegue una SIGNAL al semaforo empty */
}

/*-----------------------------------------------------------------------
   PROCEDURA 'PRODUTTORE' - NEW OBJECT IN BUFFER
   La procedura si occupa di aggiungere un nuovo elemento nel buffer.
   Come parametro prende l'oggetto che andrà inserito
   all'interno del Buffer stesso.
   -----------------------------------------------------------------------*/
void insertObject(objectData newObject)
{

    sem_wait(&semaphoreSlotEmpty);    /* Esegue una wait sul semaforo empty */
    pthread_mutex_lock(&mutexBuffer); /* Blocca il mutex principale */

    /* SEZIONE CRITICA */
    buffer[in] = newObject; /* Inserisce l'elemento e scorre l'indice del buffer */
    in = (in + 1) % DIMBUFFER;

    pthread_mutex_unlock(&mutexBuffer); /* Sblocca mutex */
    sem_post(&semaphoreSlotFull);       /* Esegue una SIGNAL al semaforo full */
}

/* ----------------------------------------------
          ANALISI DEI DATI E GESTIONE FINE PARTITA
   ----------------------------------------------*/
void analyze_data()
{
    // Termina la riproduzione dei suoni della partita
    system("killall aplay");

    // Pulisce lo schermo
    erase();
    bkgd(COLOR_PAIR(WHITE_WHITE)); /* Setta il background color dello schermo */
    attron(COLOR_PAIR(BLACK_WHITE));

    int taken_dens = 0;

    // Conta le tane occupate
    for (int i = 0; i < N_DENS; i++)
        if (gamedata.dens[i] == true)
            taken_dens++;

    // Se la manche è stata vinta
    if (gamedata.game_won)
    {
        // Se ha occupato tutte le tane si va al menu della vittoria
        if (taken_dens >= N_DENS)
        {
            system("aplay ../SUONI/victory.wav > /dev/null 2>&1 &");
            attron(COLOR_PAIR(BLACK_GREEN));
            mvprintw(3, 14, "Punti : %d    Vite : %d    Tane raggiunte : %d", gamedata.player_score, gamedata.player_lives, taken_dens);
            attroff(COLOR_PAIR(BLACK_GREEN));
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
    else
        // Se la manche è stata persa
        if (gamedata.game_lost)
        {
            gamedata.player_lives--;

            if (gamedata.player_lives <= 0)
            { // se ha esaurito le vite si va al menu della sconfitta
                system("aplay ../SUONI/gameover.wav > /dev/null 2>&1 &");
                attron(COLOR_PAIR(BLACK_RED));
                mvprintw(3, 14, "Punti : %d    Vite : %d    Tane raggiunte : %d", gamedata.player_score, gamedata.player_lives, taken_dens);
                attroff(COLOR_PAIR(BLACK_RED));
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
void *gameManche_thread(void *a, objectData frog, objectData frog_bullet, objectData plant[N_PLANTS], objectData plant_bullet[N_PLANT_BULLETS], objectData crocodile[N_CROCODILE], objectData time)
{
    sleep(1);

    int start_dens[] = {16, 27, 38, 49, 60};
    bool onCrocodile = true;

    objectData receivedPacket; // dove ricevo i dati letti
    objectData frogData = frog;
    objectData frog_bulletData = frog_bullet;
    objectData plantData[N_PLANTS] = plant;
    objectData plant_bulletData[N_PLANT_BULLETS] = plant_bullet;
    objectData crocodileData[N_CROCODILE] = crocodile;
    objectData time;

    int crocodile_immersion_timer = getCrocodileTimer(100, gamedata.difficulty); // = getRandomTimer (tempo minimo, difficoltà)

    while (should_not_exit)
    {

        while (block)
        {
        }

        erase();
        // stampa dello sfondo di gioco
        gameField();
        // stampa tane
        printDens(gamedata.dens);

        consumatore();
        receivedPacket = consumedObject;

        // assegnamento del dato al rispettivo elemento
        if (receivedPacket.id == FROG_ID)
        {
            frogData.x = receivedPacket.x;
            frogData.y = receivedPacket.y;
        }
        else if (receivedPacket.id == FROG_BULLET_ID)
        {
            frog_bulletData.x = receivedPacket.x;
            frog_bulletData.y = receivedPacket.y;
        }
        else if (receivedPacket.id >= PLANT_BULLET_ID_0 && receivedPacket.id <= PLANT_BULLET_ID_2)
        {
            for (int i = 0; i < N_PLANT_BULLETS; i++)
            {
                if (receivedPacket.id == i + PLANT_BULLET_ID_0)
                {
                    plant_bulletData[i].x = receivedPacket.x;
                    plant_bulletData[i].y = receivedPacket.y;
                }
            }
        }
        else if (receivedPacket.id >= PLANT_ID_0 && receivedPacket.id <= PLANT_ID_2)
        {
            for (int i = 0; i < N_PLANTS; i++)
            {
                if (receivedPacket.id == i + PLANT_ID_0)
                {
                    // in realtà non mi serve nulla della pianta deve fare tutt0  da sola però quando la sparo gli devo comunicare che è morta
                    //  In caso di collisione dovremo fare l'uccisione del processo
                }
            }
        }
        else if (receivedPacket.id >= CROCODILE_ID_0 && receivedPacket.id <= CROCODILE_ID_23)
        {
            for (int i = 0; i < N_CROCODILE; i++)
            {
                if (receivedPacket.id == i + CROCODILE_ID_0)
                {
                    crocodileData[i].x = receivedPacket.x;
                    crocodileData[i].y = receivedPacket.y;
                    crocodileData[i].flow_number = receivedPacket.flow_number;
                    crocodileData[i].crocodile_speed = receivedPacket.crocodile_speed;
                    crocodileData[i].direction = receivedPacket.direction;
                    crocodileData[i].is_crocodile_alive = receivedPacket.is_crocodile_alive;
                    crocodileData[i].is_crocodile_immersing = receivedPacket.is_crocodile_immersing;
                }
            }
        }

        // STAMPA ELEMENTI ----------------------------------------

        // stampa dei coccodrilli
        for (int i = 0; i < N_CROCODILE; i++)
        {
            if (crocodileData[i].is_crocodile_alive)
                crocodileBody(crocodileData[i]);
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
            if (plantData[i].plant_isalive)
                plantBody(plantData[i]);
        }

        // stampa dei proiettili delle piante
        for (int i = 0; i < N_PLANT_BULLETS; i++)
        {
            if (plant_bulletData[i].frog_bulletisactive)
            {
                plantBullett(plant_bulletData[i].x, plant_bulletData[i].y);
            }
        }

        // stampa del proiettile della rana
        if (frog_bulletData.frog_bulletisactive == true)
        {
            frogBullett(frog_bulletData.x, frog_bulletData.y);
        }

        // stampa della rana
        frogBody(frogData.x, frogData.y);

        // stampa dello score a schermo
        attron(COLOR_PAIR(WHITE_BLUE));
        mvprintw(1, PAUSE_X - 5, "Press 'Q' to pause");
        mvprintw(1, SCORE_X, "Score: %d", gamedata.player_score);
        // stampa delle vite a schermo
        mvprintw(TOTAL_HEIGHT + 1, LIFES_X, "Lifes: %d", gamedata.player_lives);
        // stampa del tempo a schermo
        mvprintw(TOTAL_HEIGHT + 1, TIME_X + 15, "Time: %d", time.time_left);
        attroff(COLOR_PAIR(WHITE_BLUE));

        refresh();

        /*
            Collisioni:
            - Rana con piante
            - Rana con proiettili piante
            - Rana con coccodrillo
            - Rana con tane
            - Proiettile rana con coccodrillo
            - Proiettile rana con piante
        */

        // SE LA RANA COLLIDE CON UNA PIANTA
        for (int i = 0; i < N_PLANTS; i++)
        {
            if ((frogData.y == plantData[i].y + 1 || frogData.y == plantData[i].y) && (frogData.x - 2 >= plantData[i].x - (FROG_W - 1) && frogData.x + 2 <= plantData[i].x + (FROG_W + 1)))
            {
                gamedata.game_lost = true;
                gamedata.player_score += DEATH_SCORE;
            }
        }

        // SE LA RANA E' COLPITA DA UN PROIETTILE DELLA PIANTA
        for (int i = 0; i < N_PLANT_BULLETS; i++)
        {
            if (plant_bulletData[i].plant_bulletisactive && frogData.y == plant_bulletData[i].y && (frogData.x >= plant_bulletData[i].x - 2 && frogData.x <= plant_bulletData[i].x + 2))
            {
                // La rana muore
                frogData.frog_candie = false; // idem di sopra
                gamedata.game_lost = true;
            }
        }

        // SE LA RANA E' SUL COCCODRILLO
        if (frogData.frog_candie && frogData.y < SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + (RIVER_LANES_NUMBER * 2) && frogData.y > SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT)
        {
            for (int i = 0; i < N_CROCODILE; i++)
            {
                /*
                if (crocodileData[i].crocodile_immersion_timer_counter < 0)
                {
                    onCrocodile = false;
                    break;
                };
                */
                // Questo in realta si può gestire con una variabile locale come in versione processi
                if (frogData.frog_candie && frogData.y == crocodileData[i].y && (frogData.x > crocodileData[i].x + 1 && frogData.x < crocodileData[i].x + CROCODILE_W - 2))
                {
                    onCrocodile = true;
                    break;
                }
                else
                    onCrocodile = false;
            }
        }

        // SE LA RANA RAGGIUNGE UNA TANA
        if (frogData.y < SCORE_ZONE_HEIGHT + 2)
        {
            // per ogni tana
            for (int i = 0; i < N_DENS; i++)
            {
                // se la rana tocca in una tana
                if (frogData.frog_candie && ((frogData.x - 2 >= start_dens[i] && frogData.x - 2 < start_dens[i] + FROG_W) || (frogData.x + 2 >= start_dens[i] && frogData.x + 2 < start_dens[i] + FROG_W)))
                {
                    if (gamedata.dens[i] == false)
                    {
                        // aumenta il punteggio in base a difficoltà e tempo traascorso
                        if (gamedata.difficulty == EASY)
                        {
                            gamedata.player_score += DEN_SCORE_EASY + (MAX_BONUS_SCORE - ((MAX_BONUS_SCORE * (TIMELIMIT_EASY - time.time_left)) / TIMELIMIT_EASY));
                        }
                        else if (gamedata.difficulty == NORMAL)
                        {
                            gamedata.player_score += DEN_SCORE_NORMAL + (MAX_BONUS_SCORE - ((MAX_BONUS_SCORE * (TIMELIMIT_EASY - time.time_left)) / TIMELIMIT_NORMAL));
                        }
                        else
                        {
                            gamedata.player_score += DEN_SCORE_HARD + (MAX_BONUS_SCORE - ((MAX_BONUS_SCORE * (TIMELIMIT_EASY - time.time_left)) / TIMELIMIT_HARD));
                        }

                        // Chiudi la tana e setta win a true per il reload del game
                        gamedata.game_won = true;
                        gamedata.dens[i] = true;
                    }
                    else
                    {
                        gamedata.game_lost = true;
                        if (gamedata.player_score > DEATH_SCORE) // sottrazzione del punteggio dopo la morte
                            gamedata.player_score -= DEATH_SCORE;
                        else
                            gamedata.player_score = 0;
                    }
                }
            }
        }

        // SE IL PROIETTILE DELLA RANA COLPISCE UN COCCODRILLO
        for (int i = 0; i < N_CROCODILE; i++)
        {
            // se il proiettile sta sul coccodrillo corrente
            if (frog_bulletData.frog_bulletisactive && (crocodileData[i].y == frog_bulletData.y) && (frog_bulletData.x > crocodileData[i].x && frog_bulletData.x < crocodileData[i].x + CROCODILE_W))
            {
                if (!crocodileData[i].crocodile_is_good)
                {
                    // se il coccodrillo è cattivo diventa buono
                    crocodileData[i].crocodile_is_good = true;
                    frog_bulletData.frog_bulletisactive = false;
                    frogData.frog_canshoot = true;

                    produttore(crocodileData[i]);
                    break;
                }
                // comunica al frog bullet di distruggere il proiettile
                frog_bulletData.frog_bulletisactive = false;
                frogData.frog_canshoot = true;

                produttore(frog_bulletData);
            }
        }

        // SE IL PROIETTILE DELLA RANA COLPISCE UNA PIANTA
        for (int i = 0; i < N_PLANTS; i++)
        {
            // se è presente plant e la rana può sparare
            if (plantData[i].plant_isalive)
            {
                // se il proiettile della rana collide con plant
                if (frog_bulletData.frog_bulletisactive && (frog_bulletData.y <= plantData[i].y + 1) && (frog_bulletData.x >= plantData[i].x && frog_bulletData.x <= plantData[i].x + 2))
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

                    // disattiva il proiettile e uccidi plant -> commento sotto
                    frog_bulletData.frog_bulletisactive = false;
                    frogData.frog_canshoot = true;
                    plantData[i].plant_isalive = false;

                    /*
                    Qui bisogna uccidere il processo della pianta, se ne creerà uno nuovo al respawn.
                    Per frog_canshoot, invece, non saprei su come fare. Non possiamo comunicarlo al thread della rana, ed è lì che viene gestito lo sparo.
                    */
                }
            }
        }

        /*
            Morti:
            - Rana per out of bounds
            - Rana per caduta in acqua
            - Rana per tempo
        */

        // MORTE RANA PER OUT OF BOUNDS
        if (frogData.x - 2 < MINX || frogData.x + 2 > MAXX - 1 || frogData.y < SCORE_ZONE_HEIGHT)
        {
            gamedata.player_score -= DEATH_SCORE;
            frogData.frog_candie = false;
            gamedata.game_lost = true;
        }

        // MORTE RANA PER CADUTA IN ACQUA
        if (!onCrocodile)
        {
            frogData.frog_candie = false;
            gamedata.game_lost = true;
        }

        // MORTE RANA PER TEMPO
        if (time.time_left <= 0 && frogData.frog_candie)
        {
            frogData.frog_candie = false;
            gamedata.game_lost = true;
        }

        /*
            Vittorie/Perdite:
            - Vittoria manche
            - Sconfitta manche
        */

        // se la manche è stata vinta o persa, fai terminare tutti i processi cambiando il valore di questa variabile
        if (gamedata.game_lost || gamedata.game_won)
        {
            should_not_exit = false;
        }
    }

    if (gamedata.player_score <= 0)
    {
        gamedata.player_score = 0;
    }
    // analyze_data();
}

/* ----------------------------------------------
         INIZIALIZZAZIONE COCCODRILLI
   ----------------------------------------------*/
void crocodiles_inizializer(objectData crocodiles[])
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
            // crocodiles[crocodileIndex].crocodile_speed = river_flows[riverIndex].speed;
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
            // river_flows[i].speed = MIN_RIVER_SPEED_EASY + rand() % (MAX_RIVER_SPEED_EASY - MIN_RIVER_SPEED_EASY + 1);
            break;
        case NORMAL:
            // river_flows[i].speed = MIN_RIVER_SPEED_NORMAL + rand() % (MAX_RIVER_SPEED_NORMAL - MIN_RIVER_SPEED_NORMAL + 1);
            break;
        case HARD:
            // river_flows[i].speed = MIN_RIVER_SPEED_HARD + rand() % (MAX_RIVER_SPEED_HARD - MIN_RIVER_SPEED_HARD + 1);
            break;
        default:
            break;
        }
    }
}

/* ----------------------------------------------
         Utility
   ----------------------------------------------*/

// Genera un booleano casuale con una certa probabilità
bool getRandomBoolean(float probability)
{
    if (probability < 0 || probability > 1)
        return false; // Error
    return rand() > probability * ((float)RAND_MAX + 1.0);
}

// Restituisce un timer casuale per l'immersione dei coccodrilli
int getCrocodileTimer()
{
    int randomTimer;

    switch (gamedata.difficulty)
    {
    case (EASY):
        randomTimer = rand() % (CROCODILE_IMMERSION_TIME_EASY + CROCODILE_IMMERSION_TIME_MIN);
        break;
    case (NORMAL):
        randomTimer = rand() % (CROCODILE_IMMERSION_TIME_NORMAL + CROCODILE_IMMERSION_TIME_MIN);
        break;
    case (HARD):
        randomTimer = rand() % (CROCODILE_IMMERSION_TIME_HARD + CROCODILE_IMMERSION_TIME_MIN);
        break;
    default:
        break;
    }
    return randomTimer;
}
