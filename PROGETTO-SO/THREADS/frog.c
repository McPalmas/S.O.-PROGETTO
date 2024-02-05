#include "include.h"


/* ----------------------------------------------   
		  FROG
   ----------------------------------------------*/ 
// Funzione per la gestione del processo frog
void* frog_thread(void *a){

    // posizione di partenza della rana
    int frog_start_y = SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + (RIVER_LANES_NUMBER * 2) + START_ZONE_HEIGHT - 3;
    int frog_start_x = (MAXX / 2) - 3;

    // inizializzazione posizione e possibilità di sparare e morire della rana
    pthread_mutex_lock(&mutex);
        frog.y = frog_start_y;
        frog.x = frog_start_x;
        frog.can_shot = true;
        frog.can_die = true;
    pthread_mutex_unlock(&mutex);

    // Ciclo di esecuzione di Frog
    while(should_not_exit){
        // stampa di tutto lo schermo di gioco    
        pthread_mutex_lock(&mutex);
            display_all_screen();        
        pthread_mutex_unlock(&mutex);

        // Attende l'input, altrimenti continua senza bloccare il gioco
        timeout(1);
        // Acquisizione input
        int c = getch();

        pthread_mutex_lock(&mutex);
        
        // Comandi Frog
        switch(c){
            //Movimenti
            case KEY_UP: 
                    frog.y -= 2; 
                    system("aplay ../SUONI/jump.wav > /dev/null 2>&1 &");
                break;
            case KEY_DOWN:
                if(frog.y < frog_start_y){
                    frog.y += 2; 
                    system("aplay ../SUONI/jump.wav > /dev/null 2>&1 &");
                }
                break;
            case KEY_LEFT:
                    frog.x -= 1; 
                break;
            case KEY_RIGHT:
                    frog.x += 1;     
                break;
            //Proiettile
            case SPACE:
                if(frog.frog_canshoot){
                    frog.frog_canshoot = false;
                    frog_bullet.bulletisactive = true;
                    system("aplay ../SUONI/lasershot.wav > /dev/null 2>&1");
                }
                break;
        }

        
        // Rimossa la parte relativa al movimento della rana sopra il coccodrillo.

        pthread_mutex_unlock(&mutex);

        // Rimosso usleep
    }
}   


//* FROG BULLET ----------------------------------------------

// Funzione per la gestione del processo frog_bullet
void* frog_bullet_thread(void *a){

        // inizialmente il proiettile non è attivo
    pthread_mutex_lock(&mutex);
        frog_bullet.bulletisactive = false;
    pthread_mutex_unlock(&mutex);
    
    // Ciclo di esecuzione di Frog Bullet
    while(should_not_exit){ 

        // se il proiettile è attivo
        if(frog_bullet.is_active == true){

            // posizione di partenza del proiettile
            pthread_mutex_lock(&mutex);
                frog_bullet.x = frog.x;
                frog_bullet.y = frog.y;
            pthread_mutex_unlock(&mutex);
        
            // fino a che il proiettile non supera il limite dell'area di gioco o fino a che non viene disattivato
            while(frog_bullet.is_active == true && frog_bullet.y > SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT){

                // sposta il proiettile verso l'alto
                pthread_mutex_lock(&mutex);
                    frog_bullet.y -= 1;
                pthread_mutex_unlock(&mutex);

                // velocità del proiettile (più è bassa, più è veloce)
                usleep(FROG_BULLET_DELAY);
            }

            // quando esce dal while disattiva il proiettile e permette alla rana di sparare 
            pthread_mutex_lock(&mutex);
                frog_bullet.bulletisactive = false;
                frog.canshoot = true;
            pthread_mutex_unlock(&mutex);
        
        }
    }   

}
