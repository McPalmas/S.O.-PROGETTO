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

// è necessario sto file non si possono inizializzare a inizio game ??

