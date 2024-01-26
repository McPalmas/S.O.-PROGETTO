#include "include.h"


/* ----------------------------------------------   
		  FROG
   ----------------------------------------------*/ 
void frog_process(int pipe[2], int pipe_shoot[2], int pipe_canshoot[2], int pipe_frogoncrocodile[2], int pipe_plantcanspawn[2], int difficulty){

    // Gestione pipe
    close(pipe[0]);
    close(pipe_shoot[0]);
    close(pipe_canshoot[1]);
    close(pipe_frogoncrocodile[1]);

    // Posizione oggetti di gioco
    objectData frog;
    objectData frog_temp;
    objectData frog_bullet;
    objectData crocodile;

    // Contatore
    int i;

    // Posizione Frog iniziale
    int frog_start_y = SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + (RIVER_LANES_NUMBER * 2) + START_ZONE_HEIGHT - 3;
    int frog_start_x = (MAXX / 2) - 3;

    // Parametri Frog
    frog.id = FROG_ID;
    frog.y = frog_start_y;
    frog.x = frog_start_x;
    frog.frog_canshoot = true;
    frog.frog_candie = true;

    // Update della posizione
    write(pipe[1],&frog, sizeof(objectData));

    while(1){

        // Se è presente il dato
        if(read(pipe_canshoot[0], &frog_bullet, sizeof(objectData)) != -1){
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
                if(frog.y > SCORE_ZONE_HEIGHT+1)
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
                if(frog.x < MAXX - 1)
                    frog.x += 1;     
                break;
            //Proiettile
            case SPACE:
                if(frog.frog_canshoot){
                    //Aggiorna lo stato
                    frog.frog_canshoot = false;
                    //Trasmette lo stato alla pipe
                    write(pipe_shoot[1], &frog, sizeof(objectData));
                }
                break;
        }

        
        // Legge la posiione di Crocodile dalla pipe
        if(read(pipe_frogoncrocodile[0], &crocodile, sizeof(objectData)) != -1){
            if(frog.y == crocodile.y && (frog.x > crocodile.x+2 && frog.x < crocodile.x + CROCODILE_W-1)){
                if(crocodile.direction==LEFT)frog.x -= 1;
                else frog.x += 1;
            }
        }
        
        // Aggiorna la posizione della rana in funzione dell'input
        if(!areFrogsEqual(frog, frog_temp)){
            // Comunica il nuovo stato della rana
            write(pipe[1],&frog, sizeof(objectData));
            //write(pipe_plantcanspawn[1],&frog, sizeof(objectData));
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
    objectData frog_bullet;
    objectData frog_bullet_data;
    objectData frog;

    // Parametri Frog Bullet
    frog_bullet.id = FROG_BULLET_ID;
    frog_bullet.x = frog.x + 2;
    frog_bullet.y = frog.y;
    frog_bullet.frog_bulletisactive = false;
    

    while(1){ 
        
        // Se è presente il dato (Frog ha sparato)
        read(p_shoot[0], &frog, sizeof(objectData)); 

        if(frog.frog_canshoot == false){
            // Posizione  Bullet
            frog_bullet.x = frog.x + 2;
            frog_bullet.y = frog.y;
            // Inizializzazione
            frog_bullet.frog_bulletisactive = true;

            while(frog_bullet.frog_bulletisactive == true){

                // Aggiorna lo stato
                if(read(p_destroy_frog_bullet[0], &frog_bullet_data, sizeof(objectData)) != -1){
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
                write(p[1], &frog_bullet, sizeof(objectData));

                // Delay di Frog Bullet
                usleep(FROG_BULLET_DELAY);
            }

            // Aggiorna lo stato
            frog_bullet.frog_bulletisactive = false;     
        }

        // Comunica lo stato
        write(p_can_shoot[1], &frog_bullet, sizeof(objectData));
    
    }   

}

/**
 * Controlla se due Frog sono equivalenti
*/
// controlla se due frogs passate come parametro sono uguali
int areFrogsEqual(objectData frog1, objectData frog2){
    return (frog1.x == frog2.x) && (frog1.y == frog2.y) && (frog1.frog_candie == frog2.frog_candie) && (frog1.frog_canshoot == frog2.frog_canshoot);
}
