#include "include.h"
#include <math.h>

/* ----------------------------------------------   
		  CROCODILE
   ----------------------------------------------*/ 
// Funzione per la gestione del processo crocodile
void crocodile_process(int id, int pipe[2], int pipe_crocodile_position[2], int pipe_frog_on_crocodile[2], int pipe_crocodile_is_shot[2], int difficulty,RiverFlow river_flows[]){
    
    // Gestione pipe
    close(pipe[0]);
    close(pipe_crocodile_position[1]);
    close(pipe_frog_on_crocodile[0]);
    srand(getpid());

    // Definizione variabili
    objectData crocodile;
    objectData frog;
    objectData frog_data;

    // Inizializzazione oggetto crocodile
    read(pipe_crocodile_position[0], &crocodile, sizeof(objectData));
    crocodile.id = id;
    crocodile.is_crocodile_immersing = false;
    crocodile.is_crocodile_alive = true;


    // Comunica stato di crocodile dopo l'inizializzazione
    write(pipe[1], &crocodile, sizeof(objectData));
    
    // Ciclo di esecuzione di crocodile
    while(1){
        
        // Se crocodile supera i margini dello schermo, muore
        if(crocodile.x <= 1 || crocodile.x+CROCODILE_W >= MAXX +CROCODILE_W){
            crocodile.is_crocodile_alive = false;
        }

        // Se crocodile è vivo
	    if(crocodile.is_crocodile_alive){
                    // Se crocodile è stato colpito, diventa buono
                    if(read(pipe_crocodile_is_shot[0], &crocodile, sizeof(objectData)) != -1)crocodile.crocodile_is_good = true;

		    // Aggiornamento posizione di crocodile
		    if (crocodile.direction == RIGHT)
		        crocodile.x += 1;
		    else
		        crocodile.x -= 1;
            }else{
                // Se crocodile è morto, viene inizializzato a una nuova corsia
                if(crocodile.flow_number == 0 || crocodile.flow_number % 2 == 0){
            	    crocodile.y += 2;
            	    crocodile.flow_number ++;
                }else if(crocodile.flow_number % 2 == 1){
            	    crocodile.y -= 2;
            	    crocodile.flow_number --;
                }
            // Inizializzazione delle nuove variabili di crocodile
            crocodile.crocodile_speed = river_flows[crocodile.flow_number].speed;
            crocodile.direction = river_flows[crocodile.flow_number].direction;
            crocodile.crocodile_is_good = rand()%2;
            crocodile.is_crocodile_alive=true;
            crocodile.is_crocodile_immersing = false;
            if(crocodile.direction == LEFT){
                crocodile.x=MAXX-2;
            }else{
                crocodile.x=2;
            }
        }
        
        // Comunica stato di crocodile dopo l'esecuzione del ciclo
        write(pipe[1], &crocodile, sizeof(objectData));
        write(pipe_frog_on_crocodile[1], &crocodile, sizeof(objectData));
        usleep(crocodile.crocodile_speed);
       
    }
}






