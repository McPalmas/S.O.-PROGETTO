#include "include.h"

/* ----------------------------------------------
        LOGIC PARTITA E PROCESSI
   ----------------------------------------------*/
void initialize_game(GameData game_data)
{
    // Inizializzazione variabili
    srand(time(NULL));
    GameData gameData = game_data;
    should_not_exit = true;
    in = 0;
    out = 0;
    gameData.game_won = false;
    gameData.game_lost = false;

    // Inizializzazione di mutex buffer e semafori
    pthread_mutex_init(&mutexBuffer, NULL);
    sem_init(&semaphoreSlotFull, 0, 0);
    sem_init(&semaphoreSlotEmpty, 0, BUFSIZ);
    // Creazione del thread gameManche
    pthread_t gameManche_t;
    pthread_create(&gameManche_t, NULL, &gameManche_thread, &gameData);

    // Attendi la terminazione di gameManche_t e termina il thread
    pthread_join(gameManche_t, NULL);
}

/*-------------------------------------------------
        PROCEDURA 'CONSUMATORE'
   -----------------------------------------------*/
void removeObject()
{
    sem_wait(&semaphoreSlotFull);     /* Esegue una wait sul semaforo full */
    pthread_mutex_lock(&mutexBuffer); /* Blocca il mutex principale */

    // Sezione critica
    consumedObject = buffer[out]; /* Preleva l'elemento e scorre l'indice del buffer */
    out = (out + 1) % DIMBUFFER;

    pthread_mutex_unlock(&mutexBuffer); /* Sblocca il mutex principale */
    sem_post(&semaphoreSlotEmpty);      /* Esegue una SIGNAL al semaforo empty */
}

/*------------------------------------------------
        PROCEDURA 'PRODUTTORE'
   ----------------------------------------------*/
void insertObject(objectData newObject)
{

    sem_wait(&semaphoreSlotEmpty);    /* Esegue una wait sul semaforo empty */
    pthread_mutex_lock(&mutexBuffer); /* Blocca il mutex principale */

    // Sezione critica
    buffer[in] = newObject; /* Inserisce l'elemento e scorre l'indice del buffer */
    in = (in + 1) % DIMBUFFER;

    pthread_mutex_unlock(&mutexBuffer); /* Sblocca mutex */
    sem_post(&semaphoreSlotFull);       /* Esegue una SIGNAL al semaforo full */
}

/* -----------------------------------------------
        ANALISI DEI DATI E GESTIONE FINE PARTITA
   ----------------------------------------------*/
void analyze_data(GameData gamedata)
{
    // Pulisce lo schermo
    erase();
    bkgd(COLOR_PAIR(WHITE_WHITE)); /* Setta il background color dello schermo */
    attron(COLOR_PAIR(BLACK_WHITE));

    // Calcola il numero di tane occupate
    int taken_dens = 0;
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
            initialize_game(gamedata);
            pthread_join(gamedata.thread_id, NULL);
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
                pthread_join(gamedata.thread_id, NULL);
            }
            else
            { // altrimenti stampa relativa al numero di vite rimanenti
                mvprintw(MAXY / 3, MAXX / 2 - 8, "Tane raggiunte: %d", taken_dens);
                mvprintw(MAXY / 3 + 1, MAXX / 2 - 8, "Vite rimanenti: %d", gamedata.player_lives);
                refresh();
                system("aplay ../SUONI/death.wav > /dev/null 2>&1");
                // tempo di attesa prima del caricamento della schermata successiva
                sleep(2);
                initialize_game(gamedata);
                pthread_join(gamedata.thread_id, NULL);
            }
        }
}

/* ----------------------------------------------
         GESTIONE MANCHE, STAMPE E COLLISIONI
   ----------------------------------------------*/
void *gameManche_thread(void *game_data)
{
    // Inizializzazione del suono del fiume
    system("aplay ../SUONI/riverSound.wav > /dev/null 2>&1 &");

    int start_dens[] = {16, 27, 38, 49, 60};
    bool onCrocodile = true;

    // Dichiarazione threads
    pthread_t frog_t;
    pthread_t frog_bullet_thread_t;
    pthread_t frog_bullet_t;
    pthread_t time_t;
    pthread_t plant_t[N_PLANTS];
    pthread_t plantBullet_t[N_PLANT_BULLETS];
    pthread_t crocodile_t[N_CROCODILE];

    // Dichiarazione variabili
    objectData receivedPacket;
    objectData frogData;
    objectData frog_bulletData;
    objectData *plantData = (objectData *)malloc(N_PLANTS * sizeof(objectData));
    objectData *plant_bulletData = (objectData *)malloc(N_PLANT_BULLETS * sizeof(objectData));
    objectData *crocodileData = (objectData *)malloc(N_CROCODILE * sizeof(objectData));
    objectData time;
    RiverFlow *river_flow = (RiverFlow *)malloc(RIVER_LANES_NUMBER * sizeof(RiverFlow));
    bool frogBulletCreated = false;
    // Estrazione dei dati
    GameData gamedata = *(GameData *)game_data;

    // Inizializzazione variabili
    gamedata.thread_id = pthread_self();
    int crocodile_immersion_timer = getCrocodileTimer(CROCODILE_IMMERSION_TIME_MIN, gamedata);
    initialize_plants(plantData, plant_bulletData, gamedata.difficulty);
    initialize_river_flows(gamedata, river_flow);
    initialize_frog(&frogData, river_flow);
    crocodiles_inizializer(crocodileData, gamedata, river_flow);
    initialize_time(&time, gamedata.difficulty);

    // Creazione dei threads
    pthread_create(&frog_t, NULL, &frog_thread, (void *)&frogData);
    pthread_create(&time_t, NULL, &time_thread, (void *)&time);
    for (int i = 0; i < N_PLANTS; i++)
        pthread_create(&plant_t[i], NULL, &plant_thread, (void *)&plantData[i]);
    for (int i = 0; i < N_CROCODILE; i++)
    {
        pthread_create(&crocodile_t[i], NULL, &crocodile_thread, (void *)&crocodileData[i]);
    }

    // Ciclo di gioco
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
            if (plant_bulletData[i].plant_bulletisactive)
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

        /*
            Gestione dati:
            - Ricezione dati
            - Assegnamento dati
        */
        // Ricezione dati
        removeObject();
        receivedPacket = consumedObject;

        // Assegnamento dati
        if (receivedPacket.id == FROG_ID)
        {
            frogData.x = receivedPacket.x;
            frogData.y = receivedPacket.y;
            frogData.frog_is_shooting = receivedPacket.frog_is_shooting;
            if (frogData.frog_is_shooting && !frogBulletCreated)
            {
                // Inizializzazione proiettile
                objectData *frogBullet = (objectData *)malloc(sizeof(frogBullet));
                objectData frog_bullet = *frogBullet;
                frog_bullet.x = frogData.x;
                frog_bullet.y = frogData.y - 1;
                frog_bullet.id = FROG_BULLET_ID;
                frog_bullet.plant_bulletisactive = true;
                frogBulletCreated = true;
                // Creazione thread
                if (pthread_create(&frog_bullet_thread_t, NULL, &frog_bullet_thread, &frog_bullet) != 0)
                {
                    _exit(1);
                }
            }
        }
        else if (receivedPacket.id == FROG_BULLET_ID)
        {
            frog_bulletData = receivedPacket;
        }
        else if (receivedPacket.id >= PLANT_BULLET_ID_0 && receivedPacket.id <= PLANT_BULLET_ID_2)
        {
            for (int i = 0; i < N_PLANT_BULLETS; i++)
            {
                if (receivedPacket.id == i + PLANT_BULLET_ID_0)
                {
                    plant_bulletData[i] = receivedPacket;
                }
            }
        }
        else if (receivedPacket.id >= PLANT_ID_0 && receivedPacket.id <= PLANT_ID_2)
        {
            for (int i = 0; i < N_PLANTS; i++)
            {
                if (receivedPacket.id == i + PLANT_ID_0)
                {
                    plantData[i] = receivedPacket;
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
                    crocodileData[i].direction = receivedPacket.direction;
                    crocodileData[i].is_crocodile_alive = receivedPacket.is_crocodile_alive;
                    if (crocodileData[i].is_crocodile_alive == false)
                    {
                        crocodileData[i].crocodile_is_good = getRandomBoolean(crocodileData[i].crocodile_is_bad_probability);
                        crocodileData[i].is_crocodile_immersing = false;
                        crocodileData[i].crocodile_immersion_timer = getCrocodileTimer(CROCODILE_IMMERSION_TIME_MIN, gamedata);
                        crocodileData[i].crocodile_immersion_timer_counter = crocodileData[i].crocodile_immersion_timer;
                    }
                }
            }
        }
        else if (receivedPacket.id == TIME_ID)
        {
            time.time_left = receivedPacket.time_left;
        }

        /*
            Collisioni:
            - Rana con piante
            - Rana con proiettili piante
            - Rana con coccodrillo
            - Rana con tane
            - Proiettile rana con coccodrillo
            - Proiettile rana con piante
            - Proiettile rana Out of Bounds
            - Proiettile pianta Out of Bounds
            - Proiettile rana con proiettile pianta
        */
        // Rana con piante
        for (int i = 0; i < N_PLANTS; i++)
        {
            if ((frogData.y == plantData[i].y + 1 || frogData.y == plantData[i].y) && (frogData.x - 2 >= plantData[i].x - (FROG_W - 1) && frogData.x + 2 <= plantData[i].x + (FROG_W + 1)))
            {
                gamedata.game_lost = true;
                gamedata.player_score += DEATH_SCORE;
            }
        }

        // Rana con proiettili piante
        for (int i = 0; i < N_PLANT_BULLETS; i++)
        {
            if (plant_bulletData[i].plant_bulletisactive && frogData.y == plant_bulletData[i].y && (frogData.x >= plant_bulletData[i].x - 2 && frogData.x <= plant_bulletData[i].x + 2))
            {
                // La rana muore
                frogData.frog_candie = false;
                gamedata.game_lost = true;
            }
        }

        // Rana con coccodrillo
        if (frogData.frog_candie && frogData.y < SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + (RIVER_LANES_NUMBER * 2) && frogData.y > SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT)
        {
            for (int i = 0; i < N_CROCODILE; i++)
            {

                if (frogData.frog_candie && frogData.y == crocodileData[i].y && (frogData.x > crocodileData[i].x + 1 && frogData.x < crocodileData[i].x + CROCODILE_W - 2))
                {
                    onCrocodile = true;
                    if (!crocodileData[i].crocodile_is_good)
                    {
                        crocodileData[i].crocodile_immersion_timer_counter--;
                        if (crocodileData[i].crocodile_immersion_timer_counter < (crocodileData[i].crocodile_immersion_timer / 2))
                            crocodileData[i].is_crocodile_immersing = true;
                    }
                    if (crocodileData[i].crocodile_immersion_timer_counter <= 0)
                        onCrocodile = false;
                    break;
                }
                else
                    onCrocodile = false;
            }
        }

        // Rana con tane
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
                        break;
                    }
                    else
                    {
                        gamedata.game_lost = true;
                        if (gamedata.player_score > DEATH_SCORE) // sottrazzione del punteggio dopo la morte
                            gamedata.player_score -= DEATH_SCORE;
                        else
                            gamedata.player_score = 0;
                        break;
                    }
                }
            }
        }

        // Proiettile rana con coccodrillo
        for (int i = 0; i < N_CROCODILE; i++)
        {
            // se il proiettile sta sul coccodrillo corrente
            if (frog_bulletData.frog_bulletisactive && (crocodileData[i].y == frog_bulletData.y) && (frog_bulletData.x > crocodileData[i].x && frog_bulletData.x < crocodileData[i].x + CROCODILE_W))
            {
                if (!crocodileData[i].crocodile_is_good)
                {
                    crocodileData[i].crocodile_is_good = true;
                    crocodileData[i].is_crocodile_immersing = false;
                    break;
                }
                // comunica al frog bullet di distruggere il proiettile
                destroyFrogBullet(&frog_bulletData, &frogBulletCreated);
            }
        }

        // Proiettile rana con piante
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

                    // disattiva il proiettile e uccidi plant
                    plantData[i].plant_isalive = false;
                    pthread_cancel(plantData[i].thread_id);
                    destroyFrogBullet(&frog_bulletData, &frogBulletCreated);
                }
            }
            else
            {
                plantData[i].plant_respawn_timer--;
                if (plantData[i].plant_respawn_timer <= 0)
                {
                    // Da vedere se va bene
                    plantData[i].plant_isalive = true;
                    plantData[i].plant_respawn_timer = PLANT_RESPAWN_MIN + rand() % (PLANT_RESPAWN_MAX - PLANT_RESPAWN_MIN + 1);
                    pthread_create(&plant_t[i], NULL, &plant_thread, (void *)&plantData[i]);
                }
            }
        }

        // Proiettile rana Out of Bounds
        if (frog_bulletData.frog_bulletisactive && frog_bulletData.y <= SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT)
        {
            destroyFrogBullet(&frog_bulletData, &frogBulletCreated);
        }

        /* Proiettili rana */
        for (int i = 0; i < N_PLANT_BULLETS; i++)
        {
            // Proiettile pianta Out of Bounds
            if (plant_bulletData[i].plant_bulletisactive && plant_bulletData[i].y >= SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + (RIVER_LANES_NUMBER * 2))
            {
                destroyPlantBullet(&plant_bulletData[i]);
            }

            // Proiettile rana con proiettile pianta
            if (frog_bulletData.frog_bulletisactive && plant_bulletData[i].plant_bulletisactive > 0)
            {
                if (frog_bulletData.x == plant_bulletData[i].x && frog_bulletData.y == plant_bulletData[i].y)
                {
                    // Se il proiettile della rana colpisce un proiettile delle piante, entrambi vengono disattivati
                    destroyFrogBullet(&frog_bulletData, &frogBulletCreated);
                    destroyPlantBullet(&plant_bulletData[i]);
                }
            }
        }

        /*
            Morti:
            - Rana per out of bounds
            - Rana per caduta in acqua
            - Rana per tempo
        */
        // Morte rana per out of bounds
        if (frogData.y < SCORE_ZONE_HEIGHT)
        {
            gamedata.player_score -= DEATH_SCORE;
            frogData.frog_candie = false;
            gamedata.game_lost = true;
        }

        // Morte rana per caduta in acqua
        if (!onCrocodile)
        {
            frogData.frog_candie = false;
            gamedata.game_lost = true;
        }

        // Morte rana per tempo
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
        }
    }

    if (gamedata.player_score <= 0)
    {
        gamedata.player_score = 0;
    }
    // Termina la riproduzione dei suoni della partita
    system("killall aplay");
    analyze_data(gamedata);
}

/* ----------------------------------------------
         INIZIALIZZAZIONE RANA
   ----------------------------------------------*/
void initialize_frog(objectData *frogData, RiverFlow river_flows[])
{
    // Salvataggio flussi del fiume
    RiverFlow riverFlow[RIVER_LANES_NUMBER];
    for (int i = 0; i < RIVER_LANES_NUMBER; i++)
    {
        riverFlow[i].speed = river_flows[i].speed;
        riverFlow[i].direction = river_flows[i].direction;

        frogData->river_flow[i] = riverFlow[i];
    }

    // Posizione di partenza della rana
    frogData->y = SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + (RIVER_LANES_NUMBER * 2) + START_ZONE_HEIGHT - 3;
    frogData->x = FROG_START;

    // Inizializzazione variabili
    frogData->frog_candie = true;
    frogData->frog_bulletisactive = false;
    frogData->id = FROG_ID;
}

/* ----------------------------------------------
         INIZIALIZZAZIONE COCCODRILLI
   ----------------------------------------------*/
void crocodiles_inizializer(objectData crocodiles[], GameData gamedata, RiverFlow river_flows[])
{
    int crocodileIndex = 0;

    // Salvataggio flussi del fiume
    RiverFlow riverFlow[RIVER_LANES_NUMBER];
    for (int i = 0; i < RIVER_LANES_NUMBER; i++)
    {
        riverFlow[i].speed = river_flows[i].speed;
        riverFlow[i].direction = river_flows[i].direction;
        for (int j = 0; j < N_CROCODILE; j++)
        {
            crocodiles[j].river_flow[i] = riverFlow[i];
        }
    }

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
            crocodiles[crocodileIndex].id = CROCODILE_ID_0 + crocodileIndex;
            crocodiles[crocodileIndex].crocodile_immersion_timer = getCrocodileTimer(CROCODILE_IMMERSION_TIME_MIN, gamedata);
            crocodiles[crocodileIndex].crocodile_immersion_timer_counter = crocodiles[crocodileIndex].crocodile_immersion_timer;
            switch (gamedata.difficulty)
            {
            case EASY:
                crocodiles[crocodileIndex].crocodile_is_good = getRandomBoolean(CROCODILE_IS_BAD_PROBABILITY_EASY);
                crocodiles[crocodileIndex].crocodile_is_bad_probability = CROCODILE_IS_BAD_PROBABILITY_EASY;
                break;
            case NORMAL:
                crocodiles[crocodileIndex].crocodile_is_good = getRandomBoolean(CROCODILE_IS_BAD_PROBABILITY_NORMAL);
                crocodiles[crocodileIndex].crocodile_is_bad_probability = CROCODILE_IS_BAD_PROBABILITY_NORMAL;
                break;
            case HARD:
                crocodiles[crocodileIndex].crocodile_is_good = getRandomBoolean(CROCODILE_IS_BAD_PROBABILITY_HARD);
                crocodiles[crocodileIndex].crocodile_is_bad_probability = CROCODILE_IS_BAD_PROBABILITY_HARD;
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
void initialize_river_flows(GameData gamedata, RiverFlow river_flows[])
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

/* ----------------------------------------------
         INIZIALIZZAZIONE PIANTE
   ----------------------------------------------*/
void initialize_plants(objectData plants[], objectData plant_bullets[], int difficulty)
{
    for (int i = 0; i < N_PLANTS; i++)
    {
        plants[i].plant_canshoot = true;
        plants[i].y = SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT;
        plants[i].plant_isalive = true;
        plants[i].id = PLANT_ID_0 + i;
        plants[i].plant_respawn_timer = PLANT_RESPAWN_MIN + rand() % (PLANT_RESPAWN_MAX - PLANT_RESPAWN_MIN + 1);
        plants[i].plant_bullet_timer = getPlantReloadTimer(PLANT_BULLET_RELOAD_MIN, difficulty);
        plant_bullets[i].id = PLANT_BULLET_ID_0 + i;
        switch (difficulty)
        {
        case EASY:
            plant_bullets[i].plant_bullet_delay = PLANT_BULLET_DELAY_EASY;
            break;
        case NORMAL:
            plant_bullets[i].plant_bullet_delay = PLANT_BULLET_DELAY_NORMAL;
            break;
        case HARD:
            plant_bullets[i].plant_bullet_delay = PLANT_BULLET_DELAY_HARD;
            break;
        default:
            break;
        }

        switch (plants[i].id)
        {
        case PLANT_ID_0:
            plants[i].x = PLANT_0_START;
            break;
        case PLANT_ID_1:
            plants[i].x = PLANT_1_START;
            break;
        case PLANT_ID_2:
            plants[i].x = PLANT_2_START;
            break;
        }
    }
}

/* ----------------------------------------------
         INIZIALIZZAZIONE TEMPO
   ----------------------------------------------*/
void initialize_time(objectData *time, int difficulty)
{
    switch (difficulty)
    {
    case EASY:
        time->time_left = TIMELIMIT_EASY;
        break;
    case NORMAL:
        time->time_left = TIMELIMIT_NORMAL;
        break;
    case HARD:
        time->time_left = TIMELIMIT_HARD;
        break;
    default:
        break;
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
int getCrocodileTimer(int min, GameData gamedata)
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

// Termina il processo del proiettile
void destroyFrogBullet(objectData *frog_bulletData, bool *frogBulletCreated)
{
    *frogBulletCreated = false;
    // Termina il processo del proiettile
    frog_bulletData->frog_bulletisactive = false;
    pthread_cancel(frog_bulletData->thread_id);
}

// Termina il processo del proiettile della pianta
void destroyPlantBullet(objectData *plant_bulletData)
{
    // Termina il processo del proiettile
    plant_bulletData->plant_bulletisactive = false;
    pthread_cancel(plant_bulletData->thread_id);
}
