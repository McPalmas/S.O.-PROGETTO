#include "include.h"

/* ----------------------------------------------   
          FROG
 ----------------------------------------------*/

Frog frog;
FrogBullet frog_bullet;


/* ----------------------------------------------   
          PLANT
 ----------------------------------------------*/
Plant plant[N_PLANTS];
PlantBullet plant_bullet[N_PLANTS];


/* ----------------------------------------------   
          CROCODILE
 ----------------------------------------------*/
Crocodile crocodile[N_CROCODILE];


/* ----------------------------------------------   
          SEMAPHORE
 ----------------------------------------------*/
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


/* ----------------------------------------------   
          INIZIALIZZAZIONE VARIABILI
 ----------------------------------------------*/

bool should_not_exit = true;

 // Da creare