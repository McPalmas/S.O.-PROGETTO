#include "include.h"


//* LOG ----------------------------------------------

void crocodile_process(int id, int pipe[2], int pipe_crocodile_position[2], int pipe_frog_on_crocodile[2], int difficulty){

    // Gestione pipe
    close(pipe[0]);
    close(pipe_crocodile_position[1]);
    close(pipe_frog_on_crocodile[0]);

    srand(getpid());

    Crocodile crocodile;
    Position crocodile_data;
    Position frog;
    Position frog_data;


    int i;
    // Velocità di spostamento del crocodile
    int crocodile_delay;

    switch (difficulty)
    {
    case EASY:
        crocodile_delay == CROCODILE_DELAY_EASY;
        break;
    case NORMAL:
        crocodile_delay == CROCODILE_DELAY_NORMAL;
        break;
    case HARD:
        crocodile_delay == CROCODILE_DELAY_HARD;
        break;
    default:
        break;
    }

    // inizializzazione crocodile
    crocodile.id = id;
    crocodile.x = 1 + rand() % (MAXX - CROCODILE_W - 2);
    crocodile.y = DENS_ZONE_HEGHT + ((id - CROCODILE_ID_0) * 2);
    crocodile.is_good = rand() % 2;
    crocodile.direction = rand() % 2 ? RIGHT : LEFT;

    // tempo minimo di attesa del crocodile prima di diventare malvagio
    int frog_on_crocodile_min_time = 80;
    // tempo minimo di attesa del crocodile prima di sparire
    int crocodile_vanish_min_time =  50;
    
    // tempo  effettivo di attesa del crocodile prima di diventare malvagio
    int frog_on_crocodile_timer = frog_on_crocodile_min_time + rand() % 130;
    // tempo effettivo di attesa del crocodile prima di sparire
    int crocodile_switch_timer = crocodile_vanish_min_time + rand() % 100;



    while(1){
        
        // se il crocodile tocca il bordo sinistro dell'area di gioco
        if(crocodile.x <= 0){
            // cambia lato
            crocodile.x=  MAXX - CROCODILE_W;
        }
        else if(crocodile.x>= MAXX - CROCODILE_W){
            // cambia lato
            crocodile.x = 0;
        }

        // spostamento del crocodile
        crocodile.x += crocodile.direction; 

        
        // Comunica stato di crocodile
        write(pipe[1], &crocodile, sizeof(Position));
        write(pipe_frog_on_crocodile[1], &crocodile, sizeof(Position));

        usleep(crocodile_delay);
    }
}
