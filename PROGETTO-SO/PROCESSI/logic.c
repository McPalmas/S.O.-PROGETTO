#include "include.h"

/* ----------------------------------------------   
          LOGIC PARTITA E PROCESSI
   ----------------------------------------------*/
void game(GameData gamedata){

    Crocodile cr;			// righe di prova per le stampe da non prendere sul serio
    cr.x=(MAXX)/2; cr.y=SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + (RIVER_LANES_NUMBER * 2) + START_ZONE_HEIGHT - 5; cr.is_good=false; cr.direction=LEFT;
    Plant p;
    p.x=(MAXX)/2; p.y=SCORE_ZONE_HEIGHT+DENS_ZONE_HEIGHT;
    
	while(1){				// prova da non considerare
	
		gameField();
    		crocodileBody(cr);
    		frogBody((MAXX)/2, SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + (RIVER_LANES_NUMBER * 2) + START_ZONE_HEIGHT - 2);
		plantBody(p);
        	getch();
	}
	
	
	
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

    /*
    
    
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
   
	
    */
    
   
   
}




