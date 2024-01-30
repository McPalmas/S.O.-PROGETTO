#include "include.h"
#include <math.h>

/* ----------------------------------------------   
		  CROCODILE
   ----------------------------------------------*/ 
void crocodile_process(int id, int pipe[2], int pipe_crocodile_position[2], int pipe_frog_on_crocodile[2], int difficulty){

    // Gestione pipe
    close(pipe[0]);
    close(pipe_crocodile_position[1]);
    close(pipe_frog_on_crocodile[0]);

    srand(getpid());

    objectData crocodile;
    objectData frog;
    objectData frog_data;

    // inizializzazione crocodile

    read(pipe_crocodile_position[0], &crocodile, sizeof(objectData));
	
    crocodile.id = id;
    crocodile.is_crocodile_immersed = false;
    crocodile.is_crocodile_alive = true;

    // ogni crocodile viene inviato a display
    write(pipe[1], &crocodile, sizeof(objectData));
    
    while(1){
        // se il crocodile tocca il bordo sinistro o destro dell'area di gioco, il suo processo viene terminato
        if(crocodile.x <= 1 || crocodile.x+CROCODILE_W >= MAXX +CROCODILE_W){
            // Termina il processo
            crocodile.is_crocodile_alive = false;
        }

	    if(crocodile.is_crocodile_alive){
		    // spostamento del crocodile
		    if (crocodile.direction == RIGHT)
		        crocodile.x += 1;
		    else
		        crocodile.x -= 1;
        }else{
            // Il coccodrillo viene spostato nella corsia inferiore (o nella prima se si trova nell'ultima corsia)
            crocodile.y = SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + (crocodile.y - SCORE_ZONE_HEIGHT - DENS_ZONE_HEIGHT - PLANTS_ZONE_HEIGHT + 2)%(RIVER_LANES_NUMBER*2);
            // Posizione relativa al fiume
            int crocodile_y_relative_to_river = crocodile.y - SCORE_ZONE_HEIGHT - DENS_ZONE_HEIGHT - PLANTS_ZONE_HEIGHT;
            // Indice della corsia del fiume
            int crocodile_river_lane = ceil(crocodile_y_relative_to_river/2);
            
            //crocodile.crocodile_speed = river_flows[crocodile_river_lane].speed; // <-- Fa impazzire tutto
            crocodile.direction = river_flows[crocodile_river_lane].direction;
            if(river_flows[crocodile_river_lane].direction == LEFT){
                crocodile.x=MAXX-2;
            }else{
                crocodile.x=2;
            }

            crocodile.is_crocodile_alive=true;
            
            // Vecchio test
            //write(pipe[1], &crocodile, sizeof(objectData));
            //write(pipe_frog_on_crocodile[1], &crocodile, sizeof(objectData));
            //write(pipe_crocodile_position[1], &crocodile, sizeof(objectData));
            // Termina il processo
            //exit(0);
        }
        
        // Comunica stato di crocodile
        write(pipe[1], &crocodile, sizeof(objectData));
        write(pipe_frog_on_crocodile[1], &crocodile, sizeof(objectData));

        usleep(crocodile.crocodile_speed);
    }
}






