#include "include.h"

/* ----------------------------------------------   
          LOGIC PARTITA E PROCESSI
   ----------------------------------------------*/
void initialize_game(GameData gamedata){

    /*int dens[5]={0,1,0,1,0};
    Crocodile cr;			// righe di prova per le stampe da non prendere sul serio
    cr.x=(MAXX)/2; cr.y=SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + (RIVER_LANES_NUMBER * 2) + START_ZONE_HEIGHT - 5; cr.is_good=false; cr.direction=LEFT;
    Position plnt;
    plnt.x=(MAXX)/2; plnt.y=SCORE_ZONE_HEIGHT+DENS_ZONE_HEIGHT;
    
	while(1){				// prova da non considerare
	
		gameField();
    		crocodileBody(cr);
    		frogBody((MAXX)/2, SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + (RIVER_LANES_NUMBER * 2) + START_ZONE_HEIGHT - 2);
		plantBody(plnt);
		printDens(dens);
        	getch();
	}*/

	//implementazione della funzione per definire gli oggetti a inizio di ogni manche incompleta e da rivedere
	
    pid_t frog;
    pid_t frog_bullet;
    pid_t plant[N_PLANTS];
    pid_t crocodile[N_CROCODILE];
    pid_t time;

    // Inizializza i flussi del fiume con direzioni e velocità casuali
    RiverFlow river_flows[RIVER_LANES_NUMBER];
    for (int i = 0; i < RIVER_LANES_NUMBER; ++i) {
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
    
    /***
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

    // Comunicazione fra frog e plant
    int pipe_frog_on_plant[2];
    pipe(pipe_frog_on_plant);
    fcntl(pipe_frog_on_plant[0], F_SETFL, fcntl(pipe_frog_on_plant[0], F_GETFL) | O_NONBLOCK);

    // Comunicazione fra frog e bullet (Bullet ha avuto una collisione)
    int pipe_destroy_frog_bullet[2];
    pipe(pipe_destroy_frog_bullet);
    fcntl(pipe_destroy_frog_bullet[0], F_SETFL, fcntl(pipe_destroy_frog_bullet[0], F_GETFL) | O_NONBLOCK);


    // Comunicazione fra frog e plant (Plant può spawnare)
    int pipe_can_plant_spawn[2];
    pipe(pipe_can_plant_spawn);
    fcntl(pipe_can_plant_spawn[0], F_SETFL, fcntl(pipe_can_plant_spawn[0], F_GETFL) | O_NONBLOCK);

    // Comunicazione fra plant e plant_bullet (Bullet ha avuto una collisione)
    int pipe_destroy_plant_bullet[3][2];
    /*pipe(pipe_destroy_plant_bullet);
    fcntl(pipe_destroy_plant_bullet[0], F_SETFL, fcntl(pipe_destroy_plant_bullet[0], F_GETFL) | O_NONBLOCK);*/
    for(int i = 0; i < 3; i++){
        pipe(pipe_destroy_plant_bullet[i]);
        fcntl(pipe_destroy_plant_bullet[i][0], F_SETFL, fcntl(pipe_destroy_plant_bullet[i][0], F_GETFL) | O_NONBLOCK);
    }

    // display comunica a crocodile la posizione iniziale dei coccodrilli
    int pipe_crocodile_position[N_CROCODILE][2];
    for(int i = 0; i < N_CROCODILE; i++){
        pipe(pipe_crocodile_position[i]);
    }

    // display comunica alla pianta quando è morta
    int pipe_plant_is_dead[N_PLANTS][2];
    for(int i = 0; i < N_PLANTS; i++){
        pipe(pipe_plant_is_dead[i]);
        fcntl(pipe_plant_is_dead[i][0], F_SETFL, fcntl(pipe_plant_is_dead[i][0], F_GETFL) | O_NONBLOCK);
    }

    
    /*
    //  creazione processi  
    frog = fork();
    
    if (frog == 0){
        frog_process(pip, pipe_shoot, pipe_canshoot, pipe_frog_on_crocodile, gamedata.difficulty);    
    }
    else{
        frog_bullet = fork();

        if(frog_bullet == 0){
            frog_bullet_process(pip, pipe_shoot, pipe_canshoot, pipe_destroy_frog_bullet);
        }
        else{
            time = fork();

            if(time == 0){
                time_process(pip, gamedata.difficulty);
            }
            else{
                for(int i=0;i<N_PLANTS;i++){
                    plant[i] = fork();
                    if(plant[i] == 0)plant_process(pip, pipe_shoot, pipe_canshoot, pipe_destroy_plant_bullet, pipe_frog_on_plant, pipe_can_plant_spawn, gamedata.difficulty);
                }
            }
    
            for(int i=0;i<N_CROCODILE;i++){
                crocodile[i] = fork();
                if(crocodile[i] == 0)crocodile_process(pip, pipe_crocodile_position, pipe_frog_on_crocodile, gamedata.difficulty);
            }
        }
    }

    for(int i=0;i<N_CROCODILE;i++){
        crocodile[i] = fork();
        if(crocodile[i] == 0)crocodile_process(pip, pipe_crocodile_position, pipe_frog_on_crocodile, gamedata.difficulty);
    }

	//if(i == N_CROCODILE){
				    
	//ottenimento dati della manche
    

    // kill dei processi
    kill(frog, 1);
    kill(frog_bullet, 1);
    kill(time, 1);
    for(int i = 0; i < N_PLANTS; i++){
        kill(plant[i], 1);
    }
    for(int i = 0; i < N_CROCODILE; i++){
        kill(crocodile[i], 1);
    }

    // se si è vinta o persa la manche allora richiamare questa stessa funzione per iniziare una nuova manche ma con i dati iniziali diversi
    if(gamedata.game_lost || gamedata.game_won){
        game(gamedata);
    }
    // altrimenti se si è proprio vinta o persa la partita richiamare il menu di endgame per uscire o rigiocare da capo
    if(gamedata.game_lost || gamedata.game_won){
        endgame_menu(gamedata);
    }
*/


    frog = fork();

    if (frog == 0){
        frog_process(pip, pipe_shoot, pipe_canshoot, pipe_frog_on_crocodile, pipe_can_plant_spawn, gamedata.difficulty);    
    }
    else{
        frog_bullet = fork();
        if(frog_bullet == 0){
            frog_bullet_process(pip, pipe_shoot, pipe_canshoot, pipe_destroy_frog_bullet);
        }
        else{
            time = fork();
            if(time == 0){
                time_process(pip, gamedata.difficulty);
            }
            else{
                crocodile[0] = fork();
                if(crocodile[0] == 0){
                    crocodile_process(CROCODILE_ID_0, pip, pipe_crocodile_position[0], pipe_frog_on_crocodile, gamedata.difficulty);
                }
                else{
                    crocodile[1] = fork();
                    if(crocodile[1] == 0){
                        crocodile_process(CROCODILE_ID_1, pip, pipe_crocodile_position[1], pipe_frog_on_crocodile, gamedata.difficulty);
                        }
                    else{
                        crocodile[2] = fork();
                        if(crocodile[2] == 0){
                        crocodile_process(CROCODILE_ID_2, pip, pipe_crocodile_position[2], pipe_frog_on_crocodile, gamedata.difficulty);                   
                        }
                        else{
                            crocodile[3] = fork();
                            if(crocodile[3] == 0){
                               crocodile_process(CROCODILE_ID_3, pip, pipe_crocodile_position[3], pipe_frog_on_crocodile, gamedata.difficulty);                    
                                }
                            else{
                                crocodile[4] = fork();
                                if(crocodile[4] == 0){
                                    crocodile_process(CROCODILE_ID_4, pip, pipe_crocodile_position[4], pipe_frog_on_crocodile, gamedata.difficulty);              
                                }
                                //Qua ci andrebbero altri coccodrilli
                                else{
                                    plant[0] = fork();
                                    if(plant[0] == 0){
                                        plant_process(PLANT_ID_0, pip, pipe_frog_on_plant, pipe_can_plant_spawn, pipe_plant_is_dead[0], pipe_destroy_plant_bullet[0], gamedata.difficulty);
                                    }
                                    else{ 
                                        plant[1] = fork();
                                        if(plant[1] == 0){
                                            plant_process(PLANT_ID_1, pip, pipe_frog_on_plant, pipe_can_plant_spawn, pipe_plant_is_dead[1], pipe_destroy_plant_bullet[1], gamedata.difficulty);
                                        }
                                        else{
                                            plant[2] = fork();
                                            if(plant[2] == 0){
                                                plant_process(PLANT_ID_2, pip, pipe_frog_on_plant, pipe_can_plant_spawn, pipe_plant_is_dead[2], pipe_destroy_plant_bullet[2], gamedata.difficulty);
                                            }
                                            else{
                                                // stampa e collisioni
                                                // la funzione restituisce il riepilogo della partita
                                                gamedata = gameManche(pip, pipe_plant_is_dead, pipe_destroy_frog_bullet,pipe_destroy_plant_bullet, pipe_crocodile_position, gamedata);
                                                // se la manche è stata vinta
                                                if(gamedata.game_won){
                                                    gamedata.game_lost = false;
                                                }
                                                    // se la manche è stata persa
                                                else{
                                                        gamedata.game_won = false;
                                                        gamedata.game_lost = true;
                                                }
                                                // Qua ci va la parte delle tane

                                                // kill dei processi
                                                kill(frog, 1);
                                                kill(frog_bullet, 1);
                                                kill(time, 1);
                                                for(int i = 0; i < N_CROCODILE; i++){
                                                    kill(crocodile[i], 1);
                                                    }
                                                for(int i = 0; i < N_PLANTS; i++){
                                                    kill(plant[i], 1);
                                                }
                                                
                                                // se il player vince o perde la partita o continua alla manche successiva
                                                analyze_data(gamedata);
                                            }
                                        }
                                    }
                                }
                            } 
                        } 
                    } 
                } 
            } 
        }
    }
}





/* ----------------------------------------------   
          CONTINUA O TERMINA LA PARTITA 
   ----------------------------------------------*/
void analyze_data(GameData gamedata){

	int taken_dens = 0;
	
	erase();
	
	// conta il numero di tane occupate
	for(int i = 0; i < N_DENS; i++)
		if(gamedata.available_dens[i] == false)
		        taken_dens++;
		        
		        

	if(gamedata.game_won){
		// se ha occupato tutte le tane si va al menu della vittoria
		if(taken_dens >= 5){
		    endGameMenu(1);
		}
		else{	// altrimenti stampa relativa alle tane occupate e inizio manche successiva
		    mvprintw(MAXY/2, MAXX/2-8, "Tane occupate: %d", taken_dens);
		    mvprintw(MAXY/2 +1, MAXX/2-8, "Vite rimanenti: %d", gamedata.player_lives);
		    refresh();
		    
		    // tempo di attesa prima del caricamento della schermata successiva
		    sleep(2);
		
		    initialize_game(gamedata);
		} 

	       
		
	}else if (gamedata.game_lost){
	
		gamedata.player_lives--;
		
		if(gamedata.player_lives <= 0) // se ha esaurito le vite si va al menu della sconfitta
            		endGameMenu(0);
            	else{      // altrimenti stampa relativa al numero di vite rimanenti         
            		mvprintw(MAXY/2, MAXX/2-8, "Vite rimanenti: %d", gamedata.player_lives);
            		mvprintw(MAXY/2 +1, MAXX/2-8, "Tane occupate: %d", taken_dens);
            		refresh();
		    
		    	// tempo di attesa prima del caricamento della schermata successiva
		    	sleep(2);
		
		    	initialize_game(gamedata);
        	}
        }
	
}




            

/* ----------------------------------------------   
         GESTIONE MANCHE, STAMPE E COLLISIONI
   ----------------------------------------------*/
GameData gameManche(int pip[2], int pipe_plant_is_dead[N_PLANTS][2], int pipe_destroy_frog_bullet[2], int pipe_destroy_plant_bullet[N_PLANT_BULLETS][2], int pipe_crocodile_position[N_CROCODILE][2], GameData gamedata){

    int i, j;

    _Bool should_not_exit = true;

    // Gestione pipe
    close(pip[1]);
    close(pipe_destroy_frog_bullet[0]);
    for (i = 0; i < N_PLANTS; i++) {
        close(pipe_plant_is_dead[i][0]);
    }
    for(i = 0; i < N_CROCODILE; i++){
        close(pipe_crocodile_position[i][0]);
    }

    srand(getpid());


    struct DataPacket receivedPacket; // dove ricevo i dati letti
    Position frog;
    Position frog_bullet;
    Plant plant[N_PLANTS];
    Plant plant_bullet[N_PLANT_BULLETS];
    Crocodile crocodile[N_CROCODILE];
    Position time;


    // posizone di partenza della rana
    int frog_start_y = SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + (RIVER_LANES_NUMBER * 2) + START_ZONE_HEIGHT - 2;
    int frog_start_x = (MAXX / 2) - 3;
    
    // bool ausiliaria per controllare se una tana è occupata o meno
    _Bool frog_in_a_den = false;
    
    //posizione y dei dati relativi al player, ovvero score, time e vite
    int top_score_height =  0 ;
    int bottom_score_height = TOTAL_HEIGHT -1 ;
    
    // inizializzazioni per evitare collisioni iniziali

    // frog
    frog.frog_candie = false;
    frog.x = frog_start_x;
    frog.y = frog_start_y;

    // frog bullet
    frog_bullet.frog_bulletisactive = false;

    // Plant
    for(i = 0; i < N_PLANTS; i++){
        plant[i].y = DENS_ZONE_HEIGHT + (i * 2); //Da rivedere
    }

    // Plant bullet
    for(i = 0; i < N_PLANT_BULLETS; i++){
        plant_bullet[i].plant_bulletisactive = false;
    }

    // time
    time.time_left = TIMELIMIT_EASY;

    // inizializzazione delle macchine (car.c)
    //cars_inizializer(car);

    // l'inizializzazione delle macchine viene comunicata a car
    for(i = 0; i < N_CROCODILE; i++){
        write(pipe_crocodile_position[i][1], &crocodile[i], sizeof(Position));
    }

    
    while (should_not_exit) {


        //* STAMPA SFONDO DI GIOCO ----------------------------------------

        erase();
        // stampa dello sfondo di gioco
        gameField();
        // stampa tane
        //caves_screen(); errore

        // stampa di una rana in tana
        for(i = 0; i < N_DENS; i++){
            if(!gamedata.available_dens[i]){
                //stampa
            }
        }

              
        //* LETTURA E ASSEGNAMENTO DATI ----------------------------------------
        
        // lettura dei dati di tutti gli oggetti di gioco
        read(pip[0], &receivedPacket, sizeof(Position));

        // assegnamento del dato al rispettivo elemento
        /*if(data.id == FROG_ID){
            frog = data;
        }
        else if(data.id == FROG_BULLET_ID){
            frog_bullet = data;
        } 
        else if(data.id >= CROCODILE_ID_0 && data.id <= CROCODILE_ID_16){
            for(i = 0; i < N_CROCODILE; i++){
                if(data.id == i + CROCODILE_ID_0){
                    crocodile[i] = data;
                }
            }
        }
        else if(data.id >= PLANT_BULLET_ID_0 && data.id <= PLANT_BULLET_ID_2){
            for(i = 0; i < N_PLANT_BULLETS; i++){
                if(data.id == i + PLANT_BULLET_ID_0){
                    plant_bullet[i] = data;
                }
            }
        }
        else if(data.id >= PLANT_ID_0 && data.id <= PLANT_ID_2){
            for(i = 0; i < N_PLANTS; i++){
                if(data.id == i + PLANT_ID_0){
                    plant[i] = data;
                }
            }
        }
        else if(data.id == TIME_ID){
            time = data;
        }*/
        
        
        switch (receivedPacket.type) {
    	    case TYPE_POSITION:
		// I dati successivi sono di tipo Position
		if(receivedPacket.data.positionData.id == FROG_ID){
		    frog = receivedPacket.data.positionData;
		}
		else if(receivedPacket.data.positionData.id == FROG_BULLET_ID){
		    frog_bullet = receivedPacket.data.positionData;
		}else 
		    time = receivedPacket.data.positionData;
		break;
	    case TYPE_PLANT:
		// I dati successivi sono di tipo Plant
		for(i = 0; i < N_PLANTS; i++){
                	if(receivedPacket.data.plantData.id == i + PLANT_ID_0){
                    	    plant[i] = receivedPacket.data.plantData;
               		}
            	}
		break;
	    case TYPE_CROCODILE:
		// I dati successivi sono di tipo Crocodile
		for(i = 0; i < N_CROCODILE; i++){
                	if(receivedPacket.data.crocodileData.id == i + CROCODILE_ID_0){
                    	    crocodile[i] = receivedPacket.data.crocodileData;
               		}
            	}
		break;
}

        


        // STAMPA ELEMENTI ----------------------------------------

        // stampa della rana
        if(frog_bullet.frog_bulletisactive){
            //stampa
        }
        else{
            //stampa
        }
        // stampa del proiettile della rana
        if(frog_bullet.frog_bulletisactive == true){    
            //stampa
        }

        // stampa dei coccodrilli
        for(i = 0; i < N_CROCODILE; i++){
            //stampa
        }

        // stampa piante
        for (i = 0; i < N_PLANTS; i++){
            //stampa
        }
        // stampa dei proiettili delle piante
        for(i = 0; i < N_PLANT_BULLETS; i++){
            if(plant_bullet[i].plant_bulletisactive){
                //stampa
            }   
        }


        // stampa dello score a schermo
        mvprintw(top_score_height, 3, "Score: %d", gamedata.player_score);
        // stampa delle vite a schermo
        mvprintw(bottom_score_height, MAXX/3, "Vite: %d", gamedata.player_lives);
        // stampa del tempo a schermo
        mvprintw(bottom_score_height, MAXX/3 + 10, "Time: %d", time.time_left);

        refresh();


        // COLLISIONI E MORTI --------------------------------------------------------------------------------------

        // RANA - TANA --------------------------------------------------------------------------------------

        // se la rana passa nella zona delle tane
        if(frog.y < DENS_ZONE_HEIGHT){
            
            int i;
            // carattere di partenza nell'asse delle x per le tane
            int start_x_caves = 5;

            frog_in_a_den = false;

            // per ogni tana
            for(i = 0; i < N_DENS; i++){
                
                // se la rana si trova in una tana
                if((frog.frog_candie && frog.x >= start_x_caves && frog.x < start_x_caves + FROG_W) && (gamedata.available_dens[i])){

                    // aumenta il punteggio
                    if(gamedata.difficulty == EASY){
                        gamedata.player_score += DEN_SCORE_EASY;
                    }
                    else if(gamedata.difficulty == NORMAL){
                        gamedata.player_score += DEN_SCORE_NORMAL;
                    }
                    else{
                        gamedata.player_score += DEN_SCORE_HARD;
                    }

                    frog.frog_candie = false;
                    // Chiudi la tana e setta win a true per il reload del game
                    gamedata.available_dens[i] = false;
                    // setta a true frog in a cave se la tana era libera
                    frog_in_a_den = true;
                    gamedata.game_won = true;
                }

                // ogni inizio tana dista 20 caratteri dall'inizio tana precedente
                start_x_caves += 20;
            }
            // se frog in a cave non è stata settata a true nel for
            if(frog.frog_candie && frog_in_a_den == false){

                // perdi la manche
                frog.frog_candie = false;
                gamedata.game_lost = true; 
            }
        }

        // RANA NEL FIUME --------------------------------------------------------------------------------------

        // per ogni coccodrillo
        for(i = 0; i < N_CROCODILE; i++){

            // se la rana non si trova sopra il coccodrillo, perdi una vita
            if(frog.frog_candie && frog.y == crocodile[i].y && (!(frog.x >= crocodile[i].x && frog.x + 5 <= crocodile[i].x + CROCODILE_W))){

                frog.frog_candie = false;
                gamedata.game_lost = true;
            }
            // se la rana si trova sopra il coccodrillo, ma lui si immerge
            //...
        }


        // PROIETTILI PIANTE - RANA --------------------------------------------------------------------------------------

        // per ogni proiettile
        for(i = 0; i < N_PLANT_BULLETS; i++){

            // se il proeittile è attivo e la rana può morire
            if(frog.frog_candie && plant_bullet[i].plant_bulletisactive){

                // se un proiettile dei nemici collide con la rana, perdi la manche
                if(plant_bullet[i].plant_bulletisactive && (plant_bullet[i].y == frog.y || plant_bullet[i].y == frog.y + 1) && (plant_bullet[i].x >= frog.x && plant_bullet[i].x <= frog.x + 4)) {

                    frog.frog_candie = false;
                    gamedata.game_lost = true;

                    // comunica a log bullet che il proiettile deve essere disattivato
                    write(pipe_destroy_plant_bullet[i][1], &plant_bullet, sizeof(Position));         
                }
            }
        }


        // PROIETTILI RANA - PIANTE --------------------------------------------------------------------------------------

        // per ogni pianta
        for(i = 0; i < N_PLANTS; i++){

            // se è presente un nemico
            if(plant[i].plant_isalive){
                // se il proiettile della rana collide col nemico
                if(frog_bullet.frog_bulletisactive && (frog_bullet.y == plant[i].y || frog_bullet.y == plant[i].y + 1) && (frog_bullet.x >= plant[i].x + 4 && frog_bullet.x <= plant[i].x + 6)) {  
                    
                    // aumenta lo score
                    if(gamedata.difficulty == EASY){
                        gamedata.player_score += DEN_SCORE_EASY;
                    }
                    else if(gamedata.difficulty == NORMAL){
                        gamedata.player_score += DEN_SCORE_NORMAL;
                    }
                    else{
                        gamedata.player_score += DEN_SCORE_HARD;
                    }

                    // disattiva il proiettile e uccidi il nemico
                    frog_bullet.frog_bulletisactive = false;
                    frog.frog_canshoot = true;
                    plant[i].plant_isalive = false;

                    // comunica al tronco che il nemico è morto
                    write(pipe_plant_is_dead[i][1], &plant[i], sizeof(Position));
                    // comunica al frog bullet di distruggere il proiettile
                    write(pipe_destroy_frog_bullet[1], &frog_bullet, sizeof(Position));
                }
            }
        }


        // PROIETTILI RANA - PROIETTILI NEMICI --------------------------------------------------------------------------------------

        // per ogni proiettile dei nemici
        for(i = 0; i < N_PLANT_BULLETS; i++){
            
            if(plant_bullet[i].plant_bulletisactive && frog_bullet.frog_bulletisactive){

                // se collide col proiettile della rana
                if((plant_bullet[i].y == frog_bullet.y) && (plant_bullet[i].x == frog_bullet.x)) {

                    // disattiva entrambi i proiettili
                    plant_bullet[i].plant_bulletisactive = false;
                    frog_bullet.frog_bulletisactive = false;
                    frog.frog_canshoot = true;

                    // comunica al frog bullet di distruggere il proiettile
                    write(pipe_destroy_frog_bullet[1], &frog_bullet, sizeof(Position));
                    write(pipe_destroy_plant_bullet[i][1], &plant_bullet, sizeof(Position));
                }
            }
        }

        // MORTE RANA PER TEMPO --------------------------------------------------------------------------------------

        // se il tempo scende a zero perdi la manche
        if(time.time_left <= 0  && frog.frog_candie){
            frog.frog_candie = false;
            gamedata.game_lost = true;
        }


        // se la manche è stata vinta o persa, fai terminare tutti i processi cambiando il valore di questa variabile
        if(gamedata.game_lost || gamedata.game_won){
            should_not_exit = false;
        }

    }  

    if(gamedata.game_lost){
        gamedata.player_score -= DEATH_SCORE; 

    }
        
    if(gamedata.player_score <= 0){
        gamedata.player_score = 0;
    }

    // restituisce la condizione alla fine della manche
    return gamedata;
}

















