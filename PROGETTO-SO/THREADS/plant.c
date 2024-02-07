#include "include.h"


/* ----------------------------------------------   
		  PLANT
   ----------------------------------------------*/ 
void* plant_thread(void *id){

    srand(getpid());

    // estrazione dell'id passato alla funzione
    int plantIndex = *((int *) id);
    // srand sulla base del thread
    unsigned int thread_id = (unsigned int)(size_t)pthread_self();
    srand(thread_id);

    int plant_bullet_timer;
    int plant_respawn_timer;
    int i;

    pthread_mutex_lock(&mutex);
    plant[plantIndex].plant_canshoot = true;
    plant[plantIndex].plant_isalive = true;
    plant[plantIndex].y = SCORE_ZONE_HEIGHT+DENS_ZONE_HEIGHT;
    plant_bullet_timer = getRandomTimer(PLANT_BULLET_RELOAD_MIN, difficulty);
    plant_respawn_timer = PLANT_RESPAWN_MIN + rand() % (PLANT_RESPAWN_MAX - PLANT_RESPAWN_MIN + 1);
    //posizioni iniziali delle piante
    pthread_mutex_unlock(&mutex);
    

    
    while (should_not_exit) {
        if(plant[plantIndex].plant_isalive){
            plant_bullet_timer--;
            // Se il timer è scaduto
            pthread_mutex_lock(&mutex);
            if(plant_bullet_timer <= 0){
                plant_bullet[plantIndex].bulletisactive = true;
                plant_bullet_timer = getRandomTimer(PLANT_BULLET_RELOAD_MIN, difficulty);
            }
            pthread_mutex_unlock(&mutex);
        }else{ // se la rana è morta 
            plant_respawn_timer --;
            if(plant_respawn_timer <= 0){   // a timer scaduto la pianta deve rinascere
            	plant.plant_isalive = true;
            	plant_respawn_timer = PLANT_RESPAWN_MIN + rand() % (PLANT_RESPAWN_MAX - PLANT_RESPAWN_MIN + 1);
      	    }
        }
        sleep(1); // 1 secondo
    }
}

void* plant_bullet_thread(void *id){

    // estrazione dell'id passato alla funzione
    int plantBulletIndex = *((int *) id); 
    
    // srand sulla base del thread
    unsigned int thread_id = (unsigned int)(size_t)pthread_self();
    srand(thread_id);
    int plant_bullet_delay;

    pthread_mutex_lock(&mutex);
    plant_bullet[plantBulletIndex].plant_bulletisactive = false;
    pthread_mutex_unlock(&mutex);
    // Inizializzazione proiettile
    plant_bullet[plantBulletIndex].x = plant[plantBulletIndex].x + 1;
    plant_bullet[plantBulletIndex].y = plant[plantBulletIndex].y + 1;
    plant_bullet[plantBulletIndex].plant_bulletisactive = true;

    switch (difficulty)
    {
    case EASY:
        plant_bullet_delay = PLANT_BULLET_DELAY_EASY;
        break;
    case NORMAL:
        plant_bullet_delay = PLANT_BULLET_DELAY_NORMAL;
        break;
    case HARD:
        plant_bullet_delay = PLANT_BULLET_DELAY_HARD;
        break;
    default:
        break;
    }


    system("aplay ../SUONI/lasershot.wav > /dev/null 2>&1");
    
    // Finché il proiettile è attivo e non è uscito dall'area di gioco
    while(should_not_exit){
        if(plant_bullet[plantBulletIndex].plant_bulletisactive){
            pthread_mutex_lock(&mutex);
                plant_bullet[plantBulletIndex].x = plant[plantBulletIndex].x + 1;
                plant_bullet[plantBulletIndex].y = plant[plantBulletIndex].y + 1;
            pthread_mutex_unlock(&mutex);
        }
        // Sposta il proiettile
        plant_bullet.y += 1;

        while(plant_bullet[plantBulletIndex].y < MAXY-12){
            pthread_mutex_lock(&mutex);
                plant_bullet[plantBulletIndex].y += 1;
            pthread_mutex_unlock(&mutex);

            delay(plant_bullet_delay);
        }

        pthread_mutex_lock(&mutex);
            plant_bullet[plantBulletIndex].plant_bulletisactive = false;
        pthread_mutex_unlock(&mutex);
    }
    
}

int getRandomTimer(int min, int difficulty){
    int randomTimer;

    switch (difficulty)
    {
    case (EASY):
        randomTimer = rand() % (PLANT_BULLET_RELOAD_EASY + 1) + min;
        break;
    case (NORMAL):
        randomTimer = rand() % (PLANT_BULLET_RELOAD_NORMAL + 1) + min;
        break;
    case(HARD):
        randomTimer = rand() % (PLANT_BULLET_RELOAD_HARD + 1) + min;
        break;
    default:
        break;
    }
    
    return randomTimer;
}



