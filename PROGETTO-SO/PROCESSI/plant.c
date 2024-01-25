#include "include.h"


/* ----------------------------------------------   
		  PLANT
   ----------------------------------------------*/ 
void plant_process(int id, int pipe[2], int pipe_frog_on_plant[2], int pipe_can_plant_spawn[2], int pipe_plant_is_dead[2], int pipe_destroy_plant_bullet[2], int difficulty){

    // Gestione pipe
    close(pipe[0]);
    close(pipe_frog_on_plant[0]);
    close(pipe_can_plant_spawn[1]);
    close(pipe_plant_is_dead[1]);

    srand(getpid());
    // Posizione oggetti di gioco
    objectData plant;
    objectData plant_data;
    objectData frog;
    objectData frog_data;
    pid_t plant_bullet;
    int plant_bullet_timer;
    int i;


    plant.id = id;
    plant.x = //...;
    plant.y = 0; //Modificato solo per compilazione
    plant.plant_canshoot = true;
    plant_bullet_timer = plant_bullet_timer + rand() % 70; //Da rivedere
    plant.plant_isalive = true;
    
    while (1) {
        if(read(pipe_plant_is_dead[0], &plant_data, sizeof(objectData)) != -1){
            plant = plant_data;     
        }

        if(read(pipe_can_plant_spawn[0], &frog_data, sizeof(objectData)) != -1){        
            frog = frog_data;
        }

        if(plant.plant_isalive){
            plant_bullet_timer--;
            // Se il timer è scaduto
            if(plant_bullet_timer <= 0){
                // Se non ci sono proiettili attivi
                if(waitpid(-1, NULL, WNOHANG) != 0){
                    // Crea un nuovo proiettile
                    plant_bullet = fork();
                    // Se è il processo figlio
                    if(plant_bullet == 0){
                        // Chiama la funzione per il proiettile
                        plant_bullet_process(pipe, plant, pipe_destroy_plant_bullet, difficulty);
                    }
                    // Resetta il timer
                    plant_bullet_timer = plant_bullet_timer + rand() % 70;
                }
            }

        // Comunicazione con display
        write(pipe[1], &plant, sizeof(objectData));
        // Comunicazione con frog
        write(pipe_frog_on_plant[1], &plant, sizeof(objectData));

        usleep(1000); //Non so se 1000 va bene, ma è un valore che ho visto in altri processi
    }
    }
}

void plant_bullet_process(int p[2], objectData plant, int pipe_destroy_plant_bullet[2], int difficulty){

    // Gestione pipe
    close(p[0]);

    objectData plant_bullet;
    objectData plant_bullet_data;
    int plant_bullet_delay;

    // Inizializzazione proiettile
    plant_bullet.id = plant.id + 5;
    plant_bullet.x = plant.x + 5;
    plant_bullet.y = plant.y + 1;
    plant_bullet.plant_bulletisactive = true;

    // comunica con display per la stampa e le collisioni
    write(p[1], &plant_bullet, sizeof(objectData));

    // Finché il proiettile è attivo e non è uscito dall'area di gioco
    while(plant_bullet.plant_bulletisactive && plant_bullet.y < TOTAL_HEIGHT - 1){
        // Aggiorna lo stato
        if(read(pipe_destroy_plant_bullet[0], &plant_bullet_data, sizeof(objectData)) != -1){
            plant_bullet.plant_bulletisactive = false;
        }

        // Sposta il proiettile
        plant_bullet.y += 1;
        // comunica con display per la stampa e le collisioni
        write(p[1], &plant_bullet, sizeof(objectData));
        // delay di avanzamento del proiettile
        usleep(plant_bullet_delay);
    }

    // disattiva il bullet quando supera l'area di gioco
    plant_bullet.plant_bulletisactive = false;
    // comunica con display per la stampa e le collisioni
    write(p[1], &plant_bullet, sizeof(objectData));
    
    // distruzione del processo
    _exit(0);
}










