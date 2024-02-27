#include "include.h"

/* ----------------------------------------------   
          FROG
 ----------------------------------------------*/

objectData frog;
objectData frog_bullet;


/* ----------------------------------------------   
          PLANT
 ----------------------------------------------*/
objectData plants[N_PLANTS];
objectData plant_bullets[N_PLANTS];


/* ----------------------------------------------   
          CROCODILE
 ----------------------------------------------*/
objectData crocodiles[N_CROCODILE];


/* ----------------------------------------------   
          SEMAPHORE
 ----------------------------------------------*/
pthread_mutex_t mutexBuffer = PTHREAD_MUTEX_INITIALIZER;


/* ----------------------------------------------   
          INIZIALIZZAZIONE VARIABILI
 ----------------------------------------------*/

objectData buffer[DIMBUFFER];
objectData consumedObject;

sem_t semaphoreSlotFull;
sem_t semaphoreSlotEmpty;

bool block = false;

int time_left = 0;

RiverFlow river_flows[RIVER_LANES_NUMBER];

bool should_not_exit = true;

GameData gamedata = {
    .game_lost = 0,
    .game_won = 0,
    .dens = {0, 0, 0, 0, 0},
    .player_score = 0,
    .player_lives = 3,
    .difficulty = 0,
    .score = 0
};
