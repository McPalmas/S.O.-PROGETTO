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
    int crocodile_switch_min_time= 80;
    // tempo minimo di attesa del crocodile prima di immergersi
    int crocodile_vanish_min_time =  50;
    
    // tempo  effettivo di attesa del crocodile prima di diventare malvagio
    int crocodile_switch_timer = crocodile_switch_min_time + rand() % 130;
    // tempo effettivo di attesa del crocodile prima di immergersi
    int crocodile_vanish_timer = crocodile_vanish_min_time + rand() % 100;



    while(1){
        // Se è presente il dato (Frog è su di lui)
        if(read(pipe_frog_on_crocodile[0], &frog_data, sizeof(Position)) != -1){
            // Se il crocodile è buono, diventa malvagio dopo il tempo definito da crocodile_switch_timer
            if(crocodile.is_good == true){
                crocodile_switch_timer--;
                if(crocodile_switch_timer == 0){
                    crocodile.is_good = false;
                }
            }
            // Se il crocodile è malvagio, si immerge dopo il tempo definito da crocodile_vanish_timer
            else{
                crocodile_vanish_timer--;
                if(crocodile_vanish_timer == 0){
                    // si immerge
                }
            }
        }
        


        // se il crocodile tocca il bordo sinistro o destro dell'area di gioco, il suo processo viene terminato
        if(crocodile.x < 0 || crocodile.x >= MAXX - CROCODILE_W){
            exit(0);
        }

        // spostamento del crocodile
        crocodile.x += crocodile.direction; 

        
        // Comunica stato di crocodile
        write(pipe[1], &crocodile, sizeof(Position));
        write(pipe_frog_on_crocodile[1], &crocodile, sizeof(Position));

        usleep(crocodile_delay);
    }
}
