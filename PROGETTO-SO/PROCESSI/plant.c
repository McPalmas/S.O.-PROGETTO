#include "include.h"


/* ----------------------------------------------   
		  PLANT
   ----------------------------------------------*/ 
void plant_process(int id, int pipe[2], int pipe_plant_position[2], int difficulty){

    // Gestione pipe
    close(pipe[0]);
    close(pipe_plant_position[1]);

    srand(getpid());

    Plant plant;
    Position plant_data;
    Position frog;
    Position frog_data;


    int i;

    // inizializzazione pianta
    plant.id = id;
    plant.x = //...;
    plant.y = //...;
    plant.plant_can_shot = true;




}
