#include "include.h"


/* ----------------------------------------------   
		  PLANT
   ----------------------------------------------*/ 
void plant_process(int id, int pipe[2], int pipe_frog_on_plant[2], int pipe_destroy_plant_bullet[2], int difficulty){

    // Gestione pipe
    close(pipe[0]);
    close(pipe_frog_on_plant[0]);

    srand(getpid());
    // Posizione oggetti di gioco
    Plant plant;
    Position plant_data;
    Position frog;
    Position frog_data;
    pid_t plant_bullet;
    int plant_bullet_timer;
    int i;


    plant.id = id;
    plant.x = //...;
    plant.y = 0; //Modificato solo per compilazione
    plant.plant_canshoot = true;
    plant_bullet_timer = plant_bullet_timer + rand() % 70; //Da rivedere
    
    while (1) {
        
        if(1){ //Al posto di 1 ci andrebbe la condizione alive della pianta. Se è viva, allora può sparare
            
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
        write(pipe[1], &plant, sizeof(Position));
        // Comunicazione con frog
        write(pipe_frog_on_plant[1], &plant, sizeof(Position));

        usleep(1000); //Non so se 1000 va bene, ma è un valore che ho visto in altri processi
    }
    }
}

void plant_bullet_process(int p[2], Plant plant, int pipe_destroy_plant_bullet[2], int difficulty){

    // Gestione pipe
    close(p[0]);

    Plant plant_bullet;
    Plant plant_bullet_data;
    int plant_bullet_delay;

    // Inizializzazione proiettile
    plant_bullet.id = plant.id + 5;
    plant_bullet.x = plant.x + 5;
    plant_bullet.y = plant.y + 1;
    plant_bullet.plant_bullet_isactive = true;

    // comunica con display per la stampa e le collisioni
    write(p[1], &plant_bullet, sizeof(Position));

    // Finché il proiettile è attivo e non è uscito dall'area di gioco
    while(plant_bullet.plant_bullet_isactive && plant_bullet.y < TOTAL_HEIGHT - 1){
        // Aggiorna lo stato
        if(read(pipe_destroy_plant_bullet[0], &plant_bullet_data, sizeof(Position)) != -1){
            plant_bullet.plant_bullet_isactive = false;
        }

        // Sposta il proiettile
        plant_bullet.y += 1;
        // comunica con display per la stampa e le collisioni
        write(p[1], &plant_bullet, sizeof(Position));
        // delay di avanzamento del proiettile
        usleep(plant_bullet_delay);
    }

    // disattiva il bullet quando supera l'area di gioco
    plant_bullet.plant_bullet_isactive = false;
    // comunica con display per la stampa e le collisioni
    write(p[1], &plant_bullet, sizeof(Position));
    
    // distruzione del processo
    _exit(0);
}










