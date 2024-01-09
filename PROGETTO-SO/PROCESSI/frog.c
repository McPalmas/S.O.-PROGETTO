#include "include.h"


/* ----------------------------------------------   
		  FROG
   ----------------------------------------------*/ 
void frog_process(int pipe[2], int pipe_shoot[2], int pipe_canshoot[2], int pipe_frogoncrocodile[2], int pipe_enemycanspawn[2], int difficulty){

    // Gestione pipe
    close(pipe[0]);
    close(pipe_shoot[0]);
    close(pipe_canshoot[1]);
    close(pipe_frogoncrocodile[1]);
    close(pipe_enemycanspawn[0]);

    // Posizione oggetti di gioco
    Position frog;
    Position frog_temp;
    Position frog_bullet;
    Crocodile crocodile;

    // Contatore
    int i;



    // Posizione Frog iniziale
    int frog_start_y = SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + (RIVER_LANES_NUMBER * 2) + START_ZONE_HEIGHT - 2;
    int frog_start_x = (MAXX / 2) - 3;

    // Parametri Frog
    frog.id = FROG_ID;
    frog.y = frog_start_y;
    frog.x = frog_start_x;
    frog.frog_canshoot = true;
    frog.frog_candie = true;

    // Update della posizione
    write(pipe[1],&frog, sizeof(Position));

    while(1){

        // Se è presente il dato
        if(read(pipe_canshoot[0], &frog_bullet, sizeof(Position)) != -1){
            // e un altro proiettile non è attivo
            if(frog_bullet.frog_bulletisactive == false){
                // Frog può sparare
                frog.frog_canshoot = true;
            }
        }

        // Attende l'input, altrimenti continua senza bloccare il gioco
        timeout(1);
        // Acquisizione input
        int c = getch();

        // Comandi Frog
        switch(c){
            //Movimenti
            case KEY_UP: 
                if(frog.y > FROG_H)
                    frog.y -= 2; 
                break;
            case KEY_DOWN:
                if(frog.y < frog_start_y)
                    frog.y += 2; 
                break;
            case KEY_LEFT: 
                if(frog.x > 0)
                    frog.x -= 1; 
                break;
            case KEY_RIGHT:
                if(frog.x < MAXX - FROG_W)
                    frog.x += 1;     
                break;
            //Proiettile
            case SPACE:
                if(frog.frog_canshoot){
                    //Aggiorna lo stato
                    frog.frog_canshoot = false;
                    //Trasmette lo stato alla pipe
                    write(pipe_shoot[1], &frog, sizeof(Position));
                }
                break;
        }

        
        // Legge la posiione di Crocodile dalla pipe
        if(read(pipe_frogoncrocodile[0], &crocodile, sizeof(Crocodile)) != -1){
            
            if(frog.y == crocodile.y){
                
                frog.x += crocodile.direction;
            }
        }
        
        // Aggiorna la posizione della rana in funzione dell'input
        if(!areFrogsEqual(frog, frog_temp)){
            // Comunica il nuovo stato della rana
            write(pipe[1],&frog, sizeof(Position));
            write(pipe_enemycanspawn[1],&frog, sizeof(Position));
        }

        // Salva lo stato precedente
        frog_temp = frog;

        usleep(1000);
    }
}   


//* FROG BULLET ----------------------------------------------


void frog_bullet_process(int p[2], int p_shoot[2], int p_can_shoot[2], int p_destroy_frog_bullet[2]){

    // Gestione pipe
    close(p[0]);
    close(p_shoot[1]);
    close(p_can_shoot[0]);
    close(p_destroy_frog_bullet[1]);

    // Posizione oggetti di gioco
    Position frog_bullet;
    Position frog_bullet_data;
    Position frog;

    // Parametri Frog Bullet
    frog_bullet.id = FROG_BULLET_ID;
    frog_bullet.x = frog.x + 2;
    frog_bullet.y = frog.y;
    frog_bullet.frog_bulletisactive = false;
    

    while(1){ 
        
        // Se è presente il dato (Frog ha sparato)
        read(p_shoot[0], &frog, sizeof(Position)); 

        if(frog.frog_canshoot == false){
            // Posizione  Bullet
            frog_bullet.x = frog.x + 2;
            frog_bullet.y = frog.y;
            // Inizializzazione
            frog_bullet.frog_bulletisactive = true;

            while(frog_bullet.frog_bulletisactive == true){

                // Aggiorna lo stato
                if(read(p_destroy_frog_bullet[0], &frog_bullet_data, sizeof(Position)) != -1){
                    frog_bullet.frog_bulletisactive = false;
                }

                if(frog_bullet.y <= DENS_ZONE_HEIGHT){
                    frog_bullet.frog_bulletisactive = false;
                }
                else if(frog_bullet.frog_bulletisactive){
                    // Movimento Bullet
                    frog_bullet.y -= 1;
                }
                
                // Comunica lo stato
                write(p[1], &frog_bullet, sizeof(Position));

                // Delay di Frog Bullet
                usleep(FROG_BULLET_DELAY);
            }

            // Aggiorna lo stato
            frog_bullet.frog_bulletisactive = false;     
        }

        // Comunica lo stato
        write(p_can_shoot[1], &frog_bullet, sizeof(Position));
    
    }   

}

/**
 * Controlla se due Frog sono equivalenti
*/
// controlla se due frogs passate come parametro sono uguali
int areFrogsEqual(Position frog1, Position frog2){
    return (frog1.x == frog2.x) && (frog1.y == frog2.y) && (frog1.frog_candie == frog2.frog_candie) && (frog1.frog_canshoot == frog2.frog_canshoot);
}
