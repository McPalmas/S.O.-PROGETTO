#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <curses.h>
#include <ncurses.h>
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

/*-----------------------------------------------------------------------
   MACRO UTILIZZATE COME TASTI PER IL MOVIMENTO VERTICALE DI FROG
   -----------------------------------------------------------------------*/
#define UP 65   /* Tasto 'Freccia su'  */
#define DOWN 66 /* Tasto 'Freccia giù' */

/*-----------------------------------------------------------------------
   MACRO UTILIZZATE PER LA GESTIONE DEI TASTI GIOCO
   -----------------------------------------------------------------------*/
#define QUIT 113 /* Tasto 'q' */
#define SPACE 32 /* Tasto 'SPACE' */

/*----------------------------------------------------------------------
   MACRO UTILIZZATE PER DEFINIRE LE DIMENSIONI MASSIME DELLO SCHERMO
   ----------------------------------------------------------------------*/
#define MINX 10
#define MAXX MINX + 60 /* Dimensione dello schermo di output (colonne) */
#define MAXY 40        /* Dimensione dello schermo di output (righe)   */

/*----------------------------------------------------------------------
   MACRO UTILIZZATE PER DEFINIRE LE DIMENSIONI DEGLI OGGETTI
   ----------------------------------------------------------------------*/
#define FROG_W 5 /* Larghezza della rana */
#define FROG_H 2 /* Altezza della rana */

#define CROCODILE_W 10 /* Larghezza del coccodrillo */
#define CROCODILE_H 2  /* Altezza del coccodrillo */

/*----------------------------------------------------------------------
   MACRO UTILIZZATE PER DEFINRIE LE POSIZIONI DEGLI OGGETTI
   ----------------------------------------------------------------------*/
#define FROG_START 40

#define PLANT_0_START 20
#define PLANT_1_START 34
#define PLANT_2_START 54

#define SCORE_X 20
#define PAUSE_X 50
#define LIFES_X 22
#define TIME_X 33

/*----------------------------------------------------------------------
   MACRO UTILIZZATE PER DEFINRIE LE DIMENSIONI DEL CAMPO DI GIOCO
   ----------------------------------------------------------------------*/
#define SCORE_ZONE_HEIGHT 3                                                                                                   /* altezza zona blue con i punteggi ecc.*/
#define DENS_ZONE_HEIGHT 3                                                                                                    /* altezza zona tane */
#define PLANTS_ZONE_HEIGHT 4                                                                                                  /* altezza zona piante */
#define START_ZONE_HEIGHT 3                                                                                                   /* altezza zona di partenza */
#define RIVER_LANES_NUMBER 8                                                                                                  /* numero corsie fiume*/
#define TOTAL_HEIGHT SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + (RIVER_LANES_NUMBER * 2) + START_ZONE_HEIGHT /* altezza totale del campo di gioco */

/*----------------------------------------------------------------------
               COPPIE DI COLORI
   ----------------------------------------------------------------------*/
#define WHITE_GREEN 1
#define WHITE_RED 2
#define BLACK_BLACK 3
#define WHITE_WHITE 4
#define GREEN_GREEN 5
#define CYAN_CYAN 6
#define MAGENTA_MAGENTA 7
#define BLUE_BLUE 8
#define GREEN_BLACK 9
#define GREEN_YELLOW 10
#define RED_YELLOW 11
#define BLACK_RED 12
#define BLACK_GREEN 13
#define RED_GREEN 14
#define YELLOW_GREEN 15
#define GREEN_MAGENTA 16
#define BLACK_WHITE 17
#define WHITE_BLUE 18

/*----------------------------------------------------------------------
               PARAMETRI DI GIOCO
   ----------------------------------------------------------------------*/
// Velocità proiettile rana
#define FROG_BULLET_DELAY 50000
// Velocità movimento crocodile (inversamente proporzionale)
#define CROCODILE_DELAY_EASY 80000000
#define CROCODILE_DELAY_NORMAL 60000000
#define CROCODILE_DELAY_HARD 40000000
// Probabilità che un crocodile nasca cattivo
#define CROCODILE_IS_BAD_PROBABILITY_EASY 0.2
#define CROCODILE_IS_BAD_PROBABILITY_NORMAL 0.35
#define CROCODILE_IS_BAD_PROBABILITY_HARD 0.5
// Tempo di immersione del coccodrillo
#define CROCODILE_IMMERSION_TIME_EASY 24
#define CROCODILE_IMMERSION_TIME_NORMAL 18
#define CROCODILE_IMMERSION_TIME_HARD 12
#define CROCODILE_IMMERSION_TIME_MIN 12
// Velocità dei proiettili delle piante
#define PLANT_BULLET_DELAY_EASY 100000
#define PLANT_BULLET_DELAY_NORMAL 80000
#define PLANT_BULLET_DELAY_HARD 60000
// Tempo di ricarica dei proiettili delle piante
#define PLANT_BULLET_RELOAD_MIN 5
#define PLANT_BULLET_RELOAD_EASY 20
#define PLANT_BULLET_RELOAD_NORMAL 15
#define PLANT_BULLET_RELOAD_HARD 10
// Tempo di respawn delle piante
#define PLANT_RESPAWN_MIN 10
#define PLANT_RESPAWN_MAX 20
// Velocità del fiume
#define MIN_RIVER_SPEED_EASY 600000
#define MIN_RIVER_SPEED_NORMAL 300000
#define MIN_RIVER_SPEED_HARD 400000
#define MAX_RIVER_SPEED_EASY 12500
#define MAX_RIVER_SPEED_NORMAL 10000
#define MAX_RIVER_SPEED_HARD 1500
// Tempo di gioco
#define TIMELIMIT_EASY 200
#define TIMELIMIT_NORMAL 150
#define TIMELIMIT_HARD 100
// Difficoltà
#define DIFFICULTIES 3 // possibili difficoltà di gioco
// Numero di oggetti
#define N_DENS 5          // numero di tane
#define N_PLANTS 3        // numero di vite
#define N_LIVES 3         // numero di vite
#define N_CROCODILE 24    // numero di coccodrilli
#define N_PLANT_BULLETS 3 // numero di proiettili per pianta
#define N_FROG_BULLETS 3  // numero di proiettili per rana
#define CROCODILES_PER_RIVER 3
// Punteggi
#define DEN_SCORE_EASY 50
#define DEN_SCORE_NORMAL 100
#define DEN_SCORE_HARD 150
#define DEATH_SCORE 50
#define MAX_BONUS_SCORE 100 // punteggio bonus in base al tempo di completamento da aggiungere al punteggio di base

/*----------------------------------------------------------------------
               ID OGGETTI
   ----------------------------------------------------------------------*/
// Frog
#define FROG_ID 1
#define FROG_BULLET_ID 2
// Crocodile
#define CROCODILE_ID_0 3
#define CROCODILE_ID_1 4
#define CROCODILE_ID_2 5
#define CROCODILE_ID_3 6
#define CROCODILE_ID_4 7
#define CROCODILE_ID_5 8
#define CROCODILE_ID_6 9
#define CROCODILE_ID_7 10
#define CROCODILE_ID_8 11
#define CROCODILE_ID_9 12
#define CROCODILE_ID_10 13
#define CROCODILE_ID_11 14
#define CROCODILE_ID_12 15
#define CROCODILE_ID_13 16
#define CROCODILE_ID_14 17
#define CROCODILE_ID_15 18
#define CROCODILE_ID_16 19
#define CROCODILE_ID_17 20
#define CROCODILE_ID_18 21
#define CROCODILE_ID_19 22
#define CROCODILE_ID_20 23
#define CROCODILE_ID_21 24
#define CROCODILE_ID_22 25
#define CROCODILE_ID_23 26
// Plant
#define PLANT_ID_0 27
#define PLANT_ID_1 28
#define PLANT_ID_2 29
// Plant bullet
#define PLANT_BULLET_ID_0 30
#define PLANT_BULLET_ID_1 31
#define PLANT_BULLET_ID_2 32
// Time
#define TIME_ID 33

#define DIMBUFFER 100000 /* Capacità massima del Buffer Condiviso */

/*----------------------------------------------------------------------
               ENUMERAZIONI
   ----------------------------------------------------------------------*/
// enum dati della difficoltà
enum Difficulty
{
   EASY,
   NORMAL,
   HARD
};

// enumerazione dati della direzione
enum Direction
{
   LEFT,
   RIGHT
};

/*----------------------------------------------------------------------
               STRUTTURE DATI
   ----------------------------------------------------------------------*/

// Dati partita
typedef struct
{
   _Bool game_lost;
   _Bool game_won;
   _Bool dens[5];
   int player_score;
   int player_lives;
   int difficulty;
   int score;
} GameData;

// Dati flusso fiume
typedef struct
{
   enum Direction direction; // Direzione del flusso: 0 per sinistra, 1 per destra
   int speed;                // Velocità del flusso
} RiverFlow;

// Dati oggetto
typedef struct
{
   // generali
   int id;
   int x;
   int y;
   // frog
   bool frog_canshoot;
   bool frog_candie;
   bool frog_bulletisactive;
   // plant
   bool plant_isalive;
   bool plant_canshoot;
   bool plant_bulletisactive;
   bool plant_bullet_timer;
   bool plant_bullet_delay;
   // crocodile
   enum Direction direction;
   int crocodile_speed;
   bool crocodile_is_good;
   float crocodile_is_bad_probability;
   bool is_crocodile_immersing;
   bool is_crocodile_alive;
   int flow_number;
   int flow_speed;
   // time
   int time_left;

   pid_t thread_id;
} objectData;

// Dati posizione proiettili
typedef struct
{
   int x, y, id, timer;
} bulletData;

typedef struct
{
   objectData object;
   objectData riverFlow[];
} args;

/*----------------------------------------------------------------------
               VARIABILI GLOBALI -> Queste dovremmo rimuoverle?
   ----------------------------------------------------------------------*/

// Thread
extern bool should_not_exit;
extern bool block;

extern objectData buffer[DIMBUFFER];
extern objectData consumedObject;
extern GameData dataBuffer[DIMBUFFER];
extern GameData consumedData;

extern pthread_mutex_t mutexBuffer; /* Mutex per il buffer */

extern sem_t semaphoreSlotFull;  /* Semaforo che conta quanti slot del buffer sono pieni */
extern sem_t semaphoreSlotEmpty; /* Semaforo che conta quanti slot del buffer sono vuoti */
extern int in, out;              /* Indici per la gestione del buffer */
extern int data_in, data_out;    /* Indici per la gestione del buffer */
/*----------------------------------------------------------------------
               FUNZIONI
   ----------------------------------------------------------------------*/

// menu.c
void mainMenu();            // visualizzazione del menu principale
void menuDifficulty();      // visualizzazione del menu per la scelta della difficoltà
void endGameMenu(bool win); // menu di fine partita in base a se si è vinto o meno

// graphic.c
void initializeScr();             // inizializzazione dello schermo per ncurses
void gameField();                 // disegna il terreno di gioco
void printDens(bool dens[]);      // stampa delle tane
void frogBody(int x, int y);      // disegna lo sprite della rana
void frogBullett(int y, int x);   // disegna il proiettile della rana
void crocodileBody(objectData c); // disegna lo sprite del coccodrillo
void plantBody(objectData p);     // stampa della pianta
void plantBullett(int y, int x);  // stampa il proiettile della pianta
void printAll();
void pausePrint(); // stampa la scritta pausa

void initialize_game(GameData gamedata);
void analyze_data();

// frog.c
void *frog_thread(void *a);                      // thread per la gestione della rana
void *frog_bullet_thread(void *frogBullet_data); // thread per la gestione del proiettile della rana

// crocodile.c
void *crocodile_thread(void *args); // thread per la gestione del coccodrillo

// plant.c
void *plant_thread(void *id);          // thread per la gestione della pianta
void *plant_bullet_thread(void *data); // thread per la gestione del proiettile della pianta

// time.c
void *time_thread(void *time_data); // thread per la gestione del tempo di gioco

// logic.c
void *gameManche_thread(void *game_data); // thread per la gestione della partita
void initialize_frog(objectData *frogData);
void crocodiles_inizializer(objectData crocodiles[], GameData gamedata, objectData river_flow[]); // inizializzazione dei coccodrilli
void initialize_river_flows(GameData gamedata, objectData RiverFlow[]);                           // inizializzazione del flusso del fiume
void plants_initializer(objectData plants[], objectData plant_bullets[], int difficulty);         // inizializzazione delle piante
void initialize_time(objectData *time, int difficulty);                                           // inizializzazione del tempo di gioco
void initialize_plants(objectData plants[], objectData plant_bullets[], int difficulty);
// funzioni di supporto
int getPlantReloadTimer(int min, int difficulty); // restituisce il tempo di ricarica del proiettile della pianta
int getCrocodileTimer();                          // restituisce un timer per l'inabissamento dei coccodrilli
bool getRandomBoolean(float probability);         // restituisce un booleano in base alla difficoltà
int getRandomTimer(int min);                      // restituisce un timer per la ricarica dei proiettili delle piante

// funzioni di supporto per la gestione del buffer
void removeObject();               // rimuove un oggetto dal buffer
void insertObject(objectData obj); // inserisce un oggetto nel buffer
void destroyFrogBullet(objectData frog_bulletData);