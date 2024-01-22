#include "include.h"


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

    // ogni crocodile viene inviato a display
    write(pipe[1], &crocodile, sizeof(objectData));
    
    while(1){
        // se il crocodile tocca il bordo sinistro o destro dell'area di gioco, il suo processo viene terminato
        if(crocodile.x < 0 || crocodile.x >= MAXX - CROCODILE_W){
            // Termina il processo
            exit(0);
        }


        // spostamento del crocodile
        if (crocodile.direction == RIGHT)
            crocodile.x += 1;
        else
            crocodile.x -= 1;

        
        // Comunica stato di crocodile
        write(pipe[1], &crocodile, sizeof(objectData));
        write(pipe_frog_on_crocodile[1], &crocodile, sizeof(objectData));

        usleep(crocodile.crocodile_speed);
    }
}
