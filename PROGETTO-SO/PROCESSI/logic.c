#include "include.h"

/* ----------------------------------------------   
          LOGIC PARTITA E PROCESSI
   ----------------------------------------------*/
void game(GameData gamedata){

    int dens[5]={0,1,0,1,0};
    Crocodile cr;			// righe di prova per le stampe da non prendere sul serio
    cr.x=(MAXX)/2; cr.y=SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + (RIVER_LANES_NUMBER * 2) + START_ZONE_HEIGHT - 5; cr.is_good=false; cr.direction=LEFT;
    Plant p;
    p.x=(MAXX)/2; p.y=SCORE_ZONE_HEIGHT+DENS_ZONE_HEIGHT;
    
	while(1){				// prova da non considerare
	
		gameField();
    		crocodileBody(cr);
    		frogBody((MAXX)/2, SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + (RIVER_LANES_NUMBER * 2) + START_ZONE_HEIGHT - 2);
		plantBody(p);
		printDens(dens);
        	getch();
	}
	}
/*	
	
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

    
    
    
    // creazione pipes 
    
    ....
    
    
    //  creazione processi  
    frog = fork();
    
    
    
    if (frog == 0){
        frog_process(p, p_shoot, p_can_shot, p_frog_on_crocodile, p_can_enemy_spawn, game_data.difficulty);    
    }
    else{
        frog_bullet = fork();

        if(frog_bullet == 0){
            frog_bullet_process(p, p_shoot, p_can_shot, p_destroy_frog_bullet);
        }
        else{
            time = fork();

            if(time == 0){
                time_process(p, game_data.difficulty);
            }
            else{
                plant[0] = fork();

                if(plant[0] == 0){
                    plant_process();
                }
                else{
                    plant[1] = fork();

                    if(plant[1] == 0){
                        plant_process();
                    }else{
                        plant[2] = fork();
 
                        if(plant[2] == 0){
                            plant_process();
                        }else{
                            crocodile[0] = fork();
 
                            if(crocodile[0] == 0){
                                crocodile_process();
                            }else{
                            
                            //... fino all'ultimo coccodrillo. Ma se usassi un ciclo invece di scrivere mille if & else ?? quello di sotto non so se funge
                            // Sono d'accordo sull'uso del ciclo

		                    for(int i = 0; i < N_CROCODILE; i++){
		                    	crocodile[i] = fork();
		                    	if(crocodile[i] == 0){
		                    	    crocodile_process();
		                    	    break;
		                    	}
				    }
				    
				    if(i == N_CROCODILE){
				    
				    
				    
				    
				    //ottenimento dati della manche
				    ....
				    
				    // kill dei processi
                                        kill(frog, 1);
                                        kill(frog_bullet, 1);
                                        kill(time, 1);
                                        for(i = 0; i < N_PLANTS; i++){
                                             kill(plant[i], 1);
                                        }
                                        for(i = 0; i < N_CROCODILES; i++){
                                             kill(crocodile[i], 1);
                                       }
                                       
                                       // se si è vinta o persa la manche allora richiamare questa stessa funzione per iniziare una nuova manche ma con i dati iniziali diversi
                                       // altrimenti se si è proprio vinta o persa la partita richiamare il menu di endgame per uscire o rigiocare da capo
                                   }    
			    }    
                        }
		   }
               }
           }
       }
   }
   
	
      
   
}







// FUNZIONE PER LA GESTIONE DELLA MANCHE (COLLLISIONI E STAMPE) nell'altro progetto era in un file a parte però era una sola funzione quindi lasciamola pure qui anche per diversificare,

GameData gameManche(int p[2], int p_enemy_is_dead[N_LOGS][2], int p_destroy_frog_bullet[2], int p_destroy_log_bullet[N_LOG_BULLETS][2], int p_cars_position[N_CROCODILES][2], GameData game_data){

    int i, j;

    _Bool should_not_exit = true;

    // chiusura del lato delle pipe non utilizzato
    close(p[1]);
    close(p_destroy_frog_bullet[0]);
    for (i = 0; i < N_LOGS; i++) {
        close(p_enemy_is_dead[i][0]);
    }
    for(i = 0; i < N_CROCODILES; i++){
        close(p_cars_position[i][0]);
    }

    srand(getpid());


    Position data;
    Position frog;
    Position frog_bullet;
    Position logs[N_LOGS];
    Position log_bullet[N_LOG_BULLETS];
    Position car[N_VEHICLES];
    Position time;


    // posizone di partenza della rana
    int frog_start_y = SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + (RIVER_LANES_NUMBER * 2) + START_ZONE_HEIGHT - 2;
    int frog_start_x = (MAXX / 2) - 3;
    
    // bool ausiliaria per controllare se una tana è occupata o meno
    _Bool frog_in_a_cave = false;
    
    //posizione y dei dati relativi al player, ovvero score, time e vite
    int top_score_height =  0 ;
    int bottom_score_height = TOTAL_HEIGHT -1 ;
    
    // inizializzazioni per evitare collisioni iniziali

    // frog
    frog.frog_can_die = false;
    frog.x = frog_start_x;
    frog.y = frog_start_y;

    // frog bullet
    frog_bullet.frog_bullet_is_active = false;

    // logs
    for(i = 0; i < N_LOGS; i++){
        logs[i].y = CAVE_HEIGHT + (i * 2);
    }

    // log bullet
    for(i = 0; i < N_LOG_BULLETS; i++){
        log_bullet[i].log_bullet_is_active = false;
    }

    // time
    time.time_left = TIME_E;

    // inizializzazione delle macchine (car.c)
    cars_inizializer(car);

    // l'inizializzazione delle macchine viene comunicata a car
    for(i = 0; i < N_VEHICLES; i++){
        write(p_cars_position[i][1], &car[i], sizeof(Position));
    }

    
    while (should_not_exit) {


        //* STAMPA SFONDO DI GIOCO ----------------------------------------

        erase();
        // stampa dello sfondo di gioco
        gameField();
        // stampa tane
        caves_screen();

        // stampa di una rana nelle tane occupate
        for(i = 0; i < N_CAVES; i++){
            if(!game_data.free_cave[i]){
                frog_screen_normal(2, 5 + (i * 20) + 2);
            }
        }

              
        //* LETTURA E ASSEGNAMENTO DATI ----------------------------------------
        
        // lettura dei dati di tutti gli oggetti di gioco
        read(p[0], &data, sizeof(Position));

        // assegnamento del dato al rispettivo elemento
        if(data.id == FROG_ID){
            frog = data;
        }
        else if(data.id == FROG_BULLET_ID){
            frog_bullet = data;
        } 
        else if(data.id >= LOG_ID_0 && data.id <= LOG_ID_4){
            for(i = 0; i < N_LOGS; i++){
                if(data.id == i + LOG_ID_0){
                    logs[i] = data;
                }
            }
        }
        else if(data.id >= LOG_BULLET_ID_0 && data.id <= LOG_BULLET_ID_4){
            for(i = 0; i < N_LOG_BULLETS; i++){
                if(data.id == i + LOG_BULLET_ID_0){
                    log_bullet[i] = data;
                }
            }
        }
        else if(data.id >= CAR_ID_0 && data.id <= CAR_ID_9){
            for(i = 0; i < N_VEHICLES; i++){
                if(data.id == i + CAR_ID_0){
                    car[i] = data;
                }
            }
        }
        else if(data.id == TIME_ID){
            time = data;
        }


        //* STAMPA ELEMENTI ----------------------------------------

        // stampa dei tronchi
        for(i = 0; i < N_LOGS; i++){
            logs_screen(logs[i]);
        }

        // stampa dei proiettili dei nemici
        for(i = 0; i < N_LOG_BULLETS; i++){
            if(log_bullet[i].log_bullet_is_active){
                log_bullet_screen(log_bullet[i]);
            }   
        }

        // stampa della rana
        if(frog_bullet.frog_bullet_is_active){
            frog_screen_shooting(frog.y, frog.x);
        }
        else{
            frog_screen_normal(frog.y, frog.x);
        }

        // stampa del proiettile della rana
        if(frog_bullet.frog_bullet_is_active == true){    
            frog_bullet_screen(frog_bullet.y, frog_bullet.x);
        }

        // stampa veicoli
        for (i = 0; i < N_VEHICLES; i++){
            car_screen(car[i]);
        }


        // stampa dello score a schermo
        mvprintw(top_score_height, 3, "Score: %d", game_data.player_score);
        // stampa delle vite a schermo
        mvprintw(bottom_score_height, MAXX/3, "Vite: %d", game_data.n_lives_player);
        // stampa del tempo a schermo
        mvprintw(bottom_score_height, MAXX/3 + 10, "Time: %d", time.time_left);

        refresh();


        //* COLLISIONI E MORTI --------------------------------------------------------------------------------------

        //* RANA - TANA --------------------------------------------------------------------------------------

        // se la rana passa nella zona delle tane
        if(frog.y < CAVE_HEIGHT){
            
            int i;
            // carattere di partenza nell'asse delle x per le tane
            int start_x_caves = 5;

            frog_in_a_cave = false;

            // per ogni tana
            for(i = 0; i < N_CAVES; i++){
                
                // se la rana si trova in una tana
                if((frog.frog_can_die && frog.x >= start_x_caves && frog.x < start_x_caves + FROG_WIDTH) && (game_data.free_cave[i])){

                    // aumenta il punteggio
                    if(game_data.difficulty == EASY){
                        game_data.player_score += TAKEN_CAVE_SCORE_E;
                    }
                    else if(game_data.difficulty == NORMAL){
                        game_data.player_score += TAKEN_CAVE_SCORE_M;
                    }
                    else{
                        game_data.player_score += TAKEN_CAVE_SCORE_H;
                    }

                    frog.frog_can_die = false;
                    // Chiudi la tana e setta win a true per il reload del game
                    game_data.free_cave[i] = false;
                    // setta a true frog in a cave se la tana era libera
                    frog_in_a_cave = true;
                    game_data.win = true;
                }

                // ogni inizio tana dista 20 caratteri dall'inizio tana precedente
                start_x_caves += 20;
            }
            // se frog in a cave non è stata settata a true nel for
            if(frog.frog_can_die && frog_in_a_cave == false){

                // perdi la manche
                frog.frog_can_die = false;
                game_data.game_over = true; 
            }
        }

        //* RANA NEL FIUME --------------------------------------------------------------------------------------

        // per ogni tronco
        for(i = 0; i < N_LOGS; i++){

            // se la rana non si trova sopra il tronco, perdi una vita
            if(frog.frog_can_die && frog.y == logs[i].y && (!(frog.x >= logs[i].x && frog.x + 5 <= logs[i].x + LOG_WIDTH))){

                frog.frog_can_die = false;
                game_data.game_over = true;
            }
            // se la rana si trova sopra il tronco ma c'è il nemico, perdi una vita
            else if (frog.frog_can_die && frog.y == logs[i].y && ((frog.x >= logs[i].x && frog.x + 5 <= logs[i].x + LOG_WIDTH) && logs[i].log_has_enemy)){

                frog.frog_can_die = false;
                game_data.game_over = true;
            }           
        }


        //* PROIETTILI NEMICI - RANA --------------------------------------------------------------------------------------

        // per ogni proiettile
        for(i = 0; i < N_LOG_BULLETS; i++){

            // se il proeittile è attivo e la rana può morire
            if(frog.frog_can_die && log_bullet[i].log_bullet_is_active){

                // se un proiettile dei nemici collide con la rana, perdi la manche
                if(log_bullet[i].log_bullet_is_active && (log_bullet[i].y == frog.y || log_bullet[i].y == frog.y + 1) && (log_bullet[i].x >= frog.x && log_bullet[i].x <= frog.x + 4)) {

                    frog.frog_can_die = false;
                    game_data.game_over = true;

                    // comunica a log bullet che il proiettile deve essere disattivato
                    write(p_destroy_log_bullet[i][1], &log_bullet, sizeof(Position));         
                }
            }
        }


        //* PROIETTILI RANA - NEMICI --------------------------------------------------------------------------------------

        // per ogni tronco
        for(i = 0; i < N_LOGS; i++){

            // se è presente un nemico
            if(logs[i].log_has_enemy){

                // se il proiettile della rana collide col nemico
                if(frog_bullet.frog_bullet_is_active && (frog_bullet.y == logs[i].y || frog_bullet.y == logs[i].y + 1) && (frog_bullet.x >= logs[i].x + 4 && frog_bullet.x <= logs[i].x + 6)) {  
                    
                    // aumenta lo score
                    if(game_data.difficulty == EASY){
                        game_data.player_score += TAKEN_CAVE_SCORE_E;
                    }
                    else if(game_data.difficulty == NORMAL){
                        game_data.player_score += TAKEN_CAVE_SCORE_M;
                    }
                    else{
                        game_data.player_score += TAKEN_CAVE_SCORE_H;
                    }

                    // disattiva il proiettile e uccidi il nemico
                    frog_bullet.frog_bullet_is_active = false;
                    frog.frog_can_shot = true;
                    logs[i].log_has_enemy = false;
                    logs[i].log_is_enemy_dead = true;

                    // comunica al tronco che il nemico è morto
                    write(p_enemy_is_dead[i][1], &logs[i], sizeof(Position));
                    // comunica al frog bullet di distruggere il proiettile
                    write(p_destroy_frog_bullet[1], &frog_bullet, sizeof(Position));
                }
            }
        }



        //* RANA - VEICOLI --------------------------------------------------------------------------------------

        // per ogni veicolo
        for(i = 0; i < N_VEHICLES; i++){

            // se la rana si trova nella stessa lane del veicolo
            if(frog.y == car[i].y && frog.frog_can_die){

                // se il veiocolo è un camion e la rana collide con esso, la rana muore
                if(car[i].car_is_camion == false && (frog.x > car[i].x - 5 && frog.x < car[i].x + CAR_WIDTH)){

                    frog.frog_can_die = false;
                    game_data.game_over = true;
                }
                // se il veiocolo è una macchina e la rana collide con essa, la rana muore
                else if (car[i].car_is_camion == true && (frog.x > car[i].x - 5 && frog.x < car[i].x + CAMION_WIDTH)){

                    frog.frog_can_die = false;
                    game_data.game_over = true;
                } 
            }
        }


        //* PROIETTILI RANA - VEICOLI --------------------------------------------------------------------------------------

        // per ogni veicolo
        for(i = 0; i < N_VEHICLES; i++){  
            
            // se il proiettile della rana è attivo
            if(frog_bullet.frog_bullet_is_active){

                // se il proiettile collide con il veicolo, disattiva il proiettile
                if((frog_bullet.y == car[i].y || frog_bullet.y == car[i].y + 1)) {

                    // controllo per le macchine
                    if(car[i].car_is_camion == false  && (frog_bullet.x >= car[i].x && frog_bullet.x < car[i].x + CAR_WIDTH)){
                        frog_bullet.frog_bullet_is_active = false;  
                        frog.frog_can_shot = true;
                        // comunica al frog bullet di distruggere il proiettile
                        write(p_destroy_frog_bullet[1], &frog_bullet, sizeof(Position));
                    }
                    // controllo per i camion
                    else if (car[i].car_is_camion == true  && (frog_bullet.x >= car[i].x && frog_bullet.x < car[i].x + CAMION_WIDTH))
                    {
                        frog_bullet.frog_bullet_is_active = false;    
                        frog.frog_can_shot = true;
                        // comunica al frog bullet di distruggere il proiettile
                        write(p_destroy_frog_bullet[1], &frog_bullet, sizeof(Position));
                    }
                }
            }
        }


        //* PROIETTILI NEMICI - VEICOLI --------------------------------------------------------------------------------------

        // per ogni veicolo
        for(i = 0; i < N_VEHICLES; i++){
            
            // per ogni proiettile dei nemici
            for(j = 0; j < N_LOG_BULLETS; j++){

                if(log_bullet[j].log_bullet_is_active){
                    
                    // se il proiettile collide con il veicolo, disattiva il proiettile
                    if((log_bullet[j].y == car[i].y || log_bullet[j].y == car[i].y + 1)) {

                        // controllo per le macchine
                        if(car[i].car_is_camion == false  && (log_bullet[j].x >= car[i].x && log_bullet[j].x < car[i].x + CAR_WIDTH)){
                            log_bullet[j].log_bullet_is_active = false;
                            // comunica a log bullet che il proiettile deve essere disattivato
                            write(p_destroy_log_bullet[j][1], &log_bullet, sizeof(Position));
                        }
                        // controllo per i camion
                        else if (car[i].car_is_camion == true  && (log_bullet[j].x >= car[i].x && log_bullet[j].x < car[i].x + CAMION_WIDTH))
                        {
                            log_bullet[j].log_bullet_is_active = false;
                            // comunica a log bullet che il proiettile deve essere disattivato
                            write(p_destroy_log_bullet[j][1], &log_bullet, sizeof(Position));
                        }
                        
                    }
                    
                }
            }
        }




        //* PROIETTILI RANA - PROIETTILI NEMICI --------------------------------------------------------------------------------------

        // per ogni proiettile dei nemici
        for(i = 0; i < N_LOG_BULLETS; i++){
            
            if(log_bullet[i].log_bullet_is_active && frog_bullet.frog_bullet_is_active){

                // se collide col proiettile della rana
                if((log_bullet[i].y == frog_bullet.y) && (log_bullet[i].x == frog_bullet.x)) {

                    // disattiva entrambi i proiettili
                    log_bullet[i].log_bullet_is_active = false;
                    frog_bullet.frog_bullet_is_active = false;
                    frog.frog_can_shot = true;

                    // comunica al frog bullet di distruggere il proiettile
                    write(p_destroy_frog_bullet[1], &frog_bullet, sizeof(Position));
                    write(p_destroy_log_bullet[i][1], &log_bullet, sizeof(Position));
                }
            }
        }

        //* MORTE RANA PER TEMPO --------------------------------------------------------------------------------------

        // se il tempo scende a zero perdi la manche
        if(time.time_left <= 0  && frog.frog_can_die){
            frog.frog_can_die = false;
            game_data.game_over = true;
        }


        // se la manche è stata vinta o persa, fai terminare tutti i processi cambiando il valore di questa variabile
        if(game_data.game_over || game_data.win){
            should_not_exit = false;
        }

    }  

    if(game_data.game_over){
        game_data.player_score -= LOST_LIFE_SCORE; 

    }
        
    if(game_data.player_score <= 0){
        game_data.player_score = 0;
    }

    // restituisce la condizione alla fine della manche
    return game_data;
}*/
















