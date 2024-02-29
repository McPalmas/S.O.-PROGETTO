#include "include.h"


/* ----------------------------------------------   
          INIZIALIZZAZIONE VARIABILI
 ----------------------------------------------*/

objectData buffer[DIMBUFFER];
objectData consumedObject;

pthread_mutex_t mutexBuffer = PTHREAD_MUTEX_INITIALIZER;
sem_t semaphoreSlotFull;
sem_t semaphoreSlotEmpty;


int in,out;

bool block = false;
enum Difficulty difficulty;
bool should_not_exit = true;


