#include "include.h"

RiverFlow river_flows[RIVER_LANES_NUMBER];

/* ----------------------------------------------
          LOGIC PARTITA E PROCESSI
   ----------------------------------------------*/
void initialize_game(GameData gamedata)
{
    // Inizializza la struttura di gioco
    gamedata.game_won = false;
    gamedata.game_lost = false;
    // Definizione variabili per i processi
    pid_t frog;
    pid_t frog_bullet;
    pid_t plant[N_PLANTS];
    pid_t crocodile[N_CROCODILE];
    pid_t time;

    /**
     * Gestione pipes
     */
    // Comunicazione con il display
    int pip[2];
    pipe(pip);

    // Comunicazione fra frog e frog_bullet (sparo)
    int pipe_shoot[2];
    pipe(pipe_shoot);

    // Comunicazione fra frog e frog_bullet (può sparare nuovamente)
    int pipe_canshoot[2];
    pipe(pipe_canshoot);
    fcntl(pipe_canshoot[0], F_SETFL, fcntl(pipe_canshoot[0], F_GETFL) | O_NONBLOCK);

    // Comunicazione fra frog e crocodile
    int pipe_frog_on_crocodile[2];
    pipe(pipe_frog_on_crocodile);
    fcntl(pipe_frog_on_crocodile[0], F_SETFL, fcntl(pipe_frog_on_crocodile[0], F_GETFL) | O_NONBLOCK);

    // Comunicazione fra oggetti e bullet (Bullet ha avuto una collisione)
    int pipe_destroy_frog_bullet[2];
    pipe(pipe_destroy_frog_bullet);
    fcntl(pipe_destroy_frog_bullet[0], F_SETFL, fcntl(pipe_destroy_frog_bullet[0], F_GETFL) | O_NONBLOCK);

    // Comunicazione fra oggetti e plant_bullet (Plant Bullet ha avuto una collisione)
    int pipe_destroy_plant_bullet[2];
    pipe(pipe_destroy_plant_bullet);
    fcntl(pipe_destroy_plant_bullet[0], F_SETFL, fcntl(pipe_destroy_plant_bullet[0], F_GETFL) | O_NONBLOCK);


    // Comunicazione della posizione dei coccodrilli
    int pipe_crocodile_position[2];
    pipe(pipe_crocodile_position);


    // Comunicazione della morte delle piante
    int pipe_plant_is_dead[2];
    pipe(pipe_plant_is_dead);
    fcntl(pipe_plant_is_dead[0], F_SETFL, fcntl(pipe_plant_is_dead[0], F_GETFL) | O_NONBLOCK);


    // Comunicazione della collisione fra Bullet e coccodrilli
    int pipe_crocodile_is_shot[2];
    pipe(pipe_crocodile_is_shot);
    fcntl(pipe_crocodile_is_shot[0], F_SETFL, fcntl(pipe_crocodile_is_shot[0], F_GETFL) | O_NONBLOCK);


    // Inizializzazione dei flussi del fiume
    initialize_river_flows(river_flows, gamedata);

    /**
     * Creazione processi
     */
    frog = fork();
    if (frog == 0)
    {
        frog_process(pip, pipe_shoot, pipe_canshoot, pipe_frog_on_crocodile, gamedata.difficulty);
    }
    else
    {
        frog_bullet = fork();

        if (frog_bullet == 0)
        {
            frog_bullet_process(pip, pipe_shoot, pipe_canshoot, pipe_destroy_frog_bullet);
        }
        else
        {
            time = fork();

            if (time == 0)
            {
                time_process(pip, gamedata.difficulty);
            }
            else
            {
                // Creazione processi crocodile con un ciclo for
                for (int i = 0; i < N_CROCODILE; i++)
                {
                    crocodile[i] = fork();
                    if (crocodile[i] == 0)
                    {
                        crocodile_process(CROCODILE_ID_0 + i, pip, pipe_crocodile_position, pipe_frog_on_crocodile, pipe_crocodile_is_shot, gamedata.difficulty, river_flows);
                        exit(0); // Importante per evitare che il processo figlio entri nel ciclo for successivo
                    }
                }

                // Creazione processi plant con un ciclo for
                for (int i = 0; i < N_PLANTS; i++)
                {
                    plant[i] = fork();
                    if (plant[i] == 0)
                    {
                        plant_process(PLANT_ID_0 + i, pip, pipe_plant_is_dead, pipe_destroy_plant_bullet, gamedata.difficulty);
                        exit(0); // Importante per evitare che il processo figlio entri nel ciclo for successivo
                    }
                }

                system("aplay ../SUONI/riverSound.wav > /dev/null 2>&1 &");

                /**
                 * Gestione della manche
                 */
                gamedata = gameManche(pip, pipe_plant_is_dead, pipe_destroy_frog_bullet, pipe_destroy_plant_bullet, pipe_crocodile_position, pipe_crocodile_is_shot,pipe_frog_on_crocodile, gamedata);

                // se la manche è stata vinta
                if (gamedata.game_won)
                {
                    gamedata.game_lost = false;
                }

                // se la manche è stata persa
                else
                {
                    gamedata.game_won = false;
                    gamedata.game_lost = true;
                }

                /**
                 * Terminazione dei processi
                 */
                kill(frog, 1);
                kill(frog_bullet, 1);
                kill(time, 1);
                for (int i = 0; i < N_CROCODILE; i++)
                {
                    kill(crocodile[i], 1);
                }
                for (int i = 0; i < N_PLANTS; i++)
                {
                    kill(plant[i], 1);
                }

                /**
                 * Analisi dei dati
                 */
                analyze_data(gamedata);
            }
        }
    }
}

/* ----------------------------------------------
          ANALISI DEI DATI E GESTIONE FINE PARTITA
   ----------------------------------------------*/
void analyze_data(GameData gamedata)
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

            initialize_game(gamedata);
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

                initialize_game(gamedata);
            }
        }
}

/* ----------------------------------------------
         GESTIONE MANCHE, STAMPE E COLLISIONI
   ----------------------------------------------*/
GameData gameManche(int pip[2], int pipe_plant_is_dead[2], int pipe_destroy_frog_bullet[2], int pipe_destroy_plant_bullet[2], int pipe_crocodile_position[2], int pipe_crocodile_is_shot[2],int pipe_frog_on_crocodile[2],GameData gamedata)
{

    int i, j;

    _Bool should_not_exit = true;

    // Gestione pipe
    close(pip[1]);
    close(pipe_destroy_frog_bullet[0]);
    close(pipe_plant_is_dead[0]);
    close(pipe_crocodile_position[0]);
    close(pipe_frog_on_crocodile[0]);

    srand(getpid());

    objectData receivedPacket; // dove ricevo i dati letti
    objectData frog;
    objectData frog_bullet;
    objectData plant[N_PLANTS];
    objectData plant_bullet[N_PLANT_BULLETS];
    objectData crocodile[N_CROCODILE];
    objectData time;

    int crocodile_immersion_timer = getCrocodileTimer(100, gamedata.difficulty); // = getRandomTimer (tempo minimo, difficoltà)

    int start_dens[] = {16, 27, 38, 49, 60};

    // posizone di partenza della rana
    int frog_start_y = SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + (RIVER_LANES_NUMBER * 2) + START_ZONE_HEIGHT - 2;
    int frog_start_x = FROG_START;

    // bool ausiliaria per controllare se una tana è occupata o meno
    _Bool frog_in_a_den = false;

    // posizione y dei dati relativi al player, ovvero score, time e vite
    int top_score_height = 1;
    int bottom_score_height = TOTAL_HEIGHT + 1;

    // inizializzazioni per evitare collisioni iniziali
    frog.frog_candie = false;
    frog.x = frog_start_x;
    frog.y = frog_start_y;

    // frog bullet
    frog_bullet.frog_bulletisactive = false;

    // Plant
    for (i = 0; i < N_PLANTS; i++)
    {
        plant[i].y = SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT;
    }

    // Plant bullet
    for (i = 0; i < N_PLANT_BULLETS; i++)
    {
        plant_bullet[i].plant_bulletisactive = false;
    }

    // time
    time.time_left = TIMELIMIT_EASY;

    crocodiles_inizializer(gamedata, crocodile);
    // l'inizializzazione dei coccodrilli viene comunicata a displlay
    

    write(pipe_crocodile_position[1], &crocodile, N_CROCODILE * sizeof(objectData));


    while (should_not_exit)
    {

        //* STAMPA SFONDO DI GIOCO ----------------------------------------

        erase();
        // stampa dello sfondo di gioco
        gameField();
        // stampa tane
        printDens(gamedata.dens);

        //* LETTURA E ASSEGNAMENTO DATI ----------------------------------------

        // lettura dei dati di tutti gli oggetti di gioco
        read(pip[0], &receivedPacket, sizeof(objectData));

        // assegnamento del dato al rispettivo elemento
        if (receivedPacket.id == FROG_ID)
        {
            frog = receivedPacket;
        }
        else if (receivedPacket.id == FROG_BULLET_ID)
        {
            frog_bullet = receivedPacket;
        }
        else if (receivedPacket.id == TIME_ID)
        {
            time = receivedPacket;
        }
        else if (receivedPacket.id >= PLANT_BULLET_ID_0 && receivedPacket.id <= PLANT_BULLET_ID_2)
        {
            for (i = 0; i < N_PLANT_BULLETS; i++)
            {
                if (receivedPacket.id == i + PLANT_BULLET_ID_0)
                {
                    plant_bullet[i] = receivedPacket;
                }
            }
        }
        else if (receivedPacket.id >= PLANT_ID_0 && receivedPacket.id <= PLANT_ID_2)
        {
            for (i = 0; i < N_PLANTS; i++)
            {
                if (receivedPacket.id == i + PLANT_ID_0)
                {
                    plant[i] = receivedPacket;
                }
            }
        }
        else if (receivedPacket.id >= CROCODILE_ID_0 && receivedPacket.id <= CROCODILE_ID_23)
        {
            for (i = 0; i < N_CROCODILE; i++)
            {
                if (receivedPacket.id == i + CROCODILE_ID_0)
                {
                    crocodile[i] = receivedPacket;
                }
            }
        }

        // STAMPA ELEMENTI ----------------------------------------

        // stampa dei coccodrilli
        for (i = 0; i < N_CROCODILE; i++)
        {
            if (crocodile[i].is_crocodile_alive)
                crocodileBody(crocodile[i]);
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
        for (i = 0; i < N_PLANTS; i++)
        {
            if (plant[i].plant_isalive)
                plantBody(plant[i]);
        }

        // stampa dei proiettili delle piante
        for (i = 0; i < N_PLANT_BULLETS; i++)
        {
            if (plant_bullet[i].plant_bulletisactive)
            {
                plantBullett(plant_bullet[i].x, plant_bullet[i].y);
            }
        }

        // stampa del proiettile della rana
        if (frog_bullet.frog_bulletisactive == true)
        {
            frogBullett(frog_bullet.x, frog_bullet.y);
        }

        // stampa della rana
        frogBody(frog.x, frog.y);

        // stampa dello score a schermo
        attron(COLOR_PAIR(WHITE_BLUE));
        mvprintw(top_score_height, SCORE_X, "Score: %d", gamedata.player_score);
        // stampa delle vite a schermo
        mvprintw(bottom_score_height, LIFES_X, "Lifes: %d", gamedata.player_lives);
        // stampa del tempo a schermo
        mvprintw(bottom_score_height, TIME_X + 15, "Time: %d", time.time_left);
        attroff(COLOR_PAIR(WHITE_BLUE));

        refresh();

        /*
            Collisioni:
            - Proiettile rana con piante
            - Rana con piante
            - Rana con proiettili piante
            - Proiettile rana con coccodrillo
            - Proiettili rana con proiettili piante
            - Rana con coccodrillo
            - Rana con tane
        */

        // SE LA RANA E' COLPITA DA UN PROIETTILE DELLA PIANTA
        // SE LA RANA TOCCA UNA PIANTA
        for (i = 0; i < N_PLANTS; i++)
        {
            // se è presente plant e la rana può sparare
            if (plant[i].plant_isalive)
            {
                // se il proiettile della rana collide con plant
                if (frog_bullet.frog_bulletisactive && (frog_bullet.y <= plant[i].y + 1) && (frog_bullet.x >= plant[i].x && frog_bullet.x <= plant[i].x + 2))
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
                    frog_bullet.frog_bulletisactive = false;
                    frog.frog_canshoot = true;
                    plant[i].plant_isalive = false;

                    // comunica che la pianta è morta
                    for(int y = 0; y < N_PLANTS; y++ )
                        write(pipe_plant_is_dead[1], &plant[i], sizeof(objectData));
                    // comunica al frog bullet di distruggere il proiettile
                    write(pipe_destroy_frog_bullet[1], &frog_bullet, sizeof(objectData));
                }
                // se la rana tocca una pianta
                if ((frog.y == plant[i].y + 1 || frog.y == plant[i].y) && (frog.x - 2 >= plant[i].x - (FROG_W - 1) && frog.x + 2 <= plant[i].x + (FROG_W + 1)))
                {
                    frog.frog_candie = false;
                    gamedata.game_lost = true;
                    gamedata.player_score += DEATH_SCORE;
                }
            }
        }

        // SE LA RANA E' COLPITA DA UN PROIETTILE DELLA PIANTA
        for (i = 0; i < N_PLANT_BULLETS; i++)
        {
            // se il proeittile è attivo e la rana può morire
            if (frog.frog_candie && plant_bullet[i].plant_bulletisactive)
            {
                // se un proiettile delle collide con la rana, perdi la manche
                if (plant_bullet[i].plant_bulletisactive && (plant_bullet[i].y == frog.y || plant_bullet[i].y == frog.y + 1) && (plant_bullet[i].x >= frog.x - 2 && plant_bullet[i].x <= frog.x + 2))
                {
                    frog.frog_candie = false;
                    gamedata.game_lost = true;
                    plant_bullet[i].plant_bulletisactive = false;
                    // comunica a plant bullet che il proiettile deve essere disattivato
                    write(pipe_destroy_plant_bullet[1], &plant_bullet, sizeof(objectData));
                }
            }
        }

        // SE UN PROIETTILE DELLA RANA COLPISCE UN COCCODRILLO
        for (i = 0; i < N_CROCODILE; i++)
        {
            // se il proiettile sta sul coccodrillo corrente
            if (frog_bullet.frog_bulletisactive && (crocodile[i].y + 1 == frog_bullet.y) && (frog_bullet.x >= crocodile[i].x && frog_bullet.x <= crocodile[i].x + CROCODILE_W))
            {
                for(int y = 0; y < N_CROCODILE; y++ )
                    write(pipe_crocodile_is_shot[1], &crocodile[i],sizeof(objectData));
                // comunica al frog bullet di distruggere il proiettile
                frog_bullet.frog_bulletisactive = false;
                frog.frog_canshoot = true;
                write(pipe_destroy_frog_bullet[1], &frog_bullet, sizeof(objectData));
            }
        }

        // SE UN PROIETTILE DELLA RANA COLPISCE UN PROIETTILE DELLA PIANTA
        for (i = 0; i < N_PLANT_BULLETS; i++)
        {

            if (plant_bullet[i].plant_bulletisactive && frog_bullet.frog_bulletisactive)
            {

                // se collide col proiettile della rana
                if ((plant_bullet[i].y == frog_bullet.y) && (plant_bullet[i].x == frog_bullet.x))
                {

                    // disattiva entrambi i proiettili
                    plant_bullet[i].plant_bulletisactive = false;
                    frog_bullet.frog_bulletisactive = false;
                    frog.frog_canshoot = true;

                    // comunica al frog bullet di distruggere il proiettile
                    write(pipe_destroy_frog_bullet[1], &frog_bullet, sizeof(objectData));
                    write(pipe_destroy_plant_bullet[1], &plant_bullet, sizeof(objectData));
                }
            }
        }

        // SE LA RANA E' SULLA SCHIENA DI UN COCCODRILLO
        // SE LA RANA E' NEL FIUME
        bool onCrocodile = false;
        // Se la rana si trova nel fiume
        if (frog.y < SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + (RIVER_LANES_NUMBER * 2) && frog.y > SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT)
        {
            // per ogni coccodrillo
            for (i = 0; i < N_CROCODILE; i++)
            {
                // se la rana si trova su almeno un coccodrillo, aggiorna la variabile ed esce dal ciclo
                if (frog.frog_candie && frog.y == crocodile[i].y && (frog.x > crocodile[i].x + 1 - 1 * crocodile[i].direction && frog.x < (crocodile[i].x + CROCODILE_W - 1 - crocodile[i].direction)))
                {  
                    onCrocodile = true;
                    if (!crocodile[i].crocodile_is_good)
                    {
                        crocodile_immersion_timer--;
                        switch (gamedata.difficulty)
                        {
                        case EASY:
                            if (crocodile_immersion_timer <= (CROCODILE_IMMERSION_TIME_EASY / 2))
                                crocodile[i].is_crocodile_immersing = true;
                            break;
                        case NORMAL:
                            if (crocodile_immersion_timer <= (CROCODILE_IMMERSION_TIME_NORMAL / 2))
                                crocodile[i].is_crocodile_immersing = true;
                            break;
                        case HARD:
                            if (crocodile_immersion_timer <= (CROCODILE_IMMERSION_TIME_HARD / 2))
                                crocodile[i].is_crocodile_immersing = true;
                            break;
                        }
                        if (crocodile_immersion_timer <= 0)
                        {
                            frog.frog_candie = false;
                            gamedata.game_lost = true;
                        };
                    }
                    else
                        crocodile_immersion_timer = getCrocodileTimer(100, gamedata.difficulty);
                    break;      
                }
                else
                    onCrocodile = false;
            }
            if (!onCrocodile)
            {
                frog.frog_candie = false;
                gamedata.game_lost = true;
            }
        }

        // SE LA RANA E' NELLA ZONA DELLE TANE
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

        /*
            Morti:
            - Rana per out of bounds
            - Rana per caduta in acqua
            - Rana per tempo
        */

        // MORTE RANA PER OUT OF BOUNDS
        if (frog.y < SCORE_ZONE_HEIGHT)
        {
            gamedata.player_score -= DEATH_SCORE;
            frog.frog_candie = false;
            gamedata.game_lost = true;
        }

        // MORTE RANA PER TEMPO
        if (time.time_left <= 0 && frog.frog_candie)
        {
            frog.frog_candie = false;
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

    // restituisce la condizione alla fine della manche
    return gamedata;
}

/* ----------------------------------------------
         INIZIALIZZAZIONE COCCODRILLI
   ----------------------------------------------*/
void crocodiles_inizializer(GameData gamedata, objectData crocodiles[])
{
    int crocodileIndex = 0;

    // Itera su ciascun fiume
    for (int riverIndex = 0; riverIndex < RIVER_LANES_NUMBER; riverIndex++)
    {
        // Assegna 3 coccodrilli a ciascun fiume
        for (int i = 0; i < CROCODILES_PER_RIVER; i++)
        {
            crocodiles[crocodileIndex].id = CROCODILE_ID_0 + crocodileIndex;
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
void initialize_river_flows(RiverFlow river_flows[], GameData gamedata)
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
int getCrocodileTimer(int min, int difficulty)
{
    int randomTimer;

    switch (difficulty)
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
