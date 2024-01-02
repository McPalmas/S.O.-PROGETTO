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




/*-----------------------------------------------------------------------
   MACRO UTILIZZATE COME TASTI PER IL MOVIMENTO VERTICALE DELLO SHUTTLE
   -----------------------------------------------------------------------*/
#define UP      65    /* Cursore sopra */
#define DOWN      66    /* Cursore sotto */


/*-----------------------------------------------------------------------
   MACRO UTILIZZATE PER LA GESTIONE DEI TASTI GIOCO
   -----------------------------------------------------------------------*/
#define QUIT      113    /* Tasto 'q' */
#define SPACE      32    /* Tasto 'SPACE' */


/*----------------------------------------------------------------------
   MACRO UTILIZZATE PER DEFINIRE LE DIMENSIONI MASSIME DELLO SCHERMO
   ----------------------------------------------------------------------*/
#define MAXX    60    /* Dimensione dello schermo di output (colonne) */
#define MAXY    40    /* Dimensione dello schermo di output (righe)   */

/*----------------------------------------------------------------------
   MACRO UTILIZZATE PER DEFINIRE LE DIMENSIONI DEGLI OGGETTI
   ----------------------------------------------------------------------*/
#define FROG_W 3    /* Larghezza della rana */
#define FROG_H 2    /* Altezza della rana */

#define CROCODILE_W 6    /* Larghezza del coccodrillo */
#define CROCODILE_H 2    /* Altezza del coccodrillo */

/*----------------------------------------------------------------------
   			   GREEN
   ----------------------------------------------------------------------*/
#define SCORE_ZONE_HEIGHT 3          /* altezza zona blue con i punteggi ecc.*/
#define DENS_ZONE_HEIGHT 3       /* altezza zona tane */
#define PLANTS_ZONE_HEIGHT 4    /* altezza zona piante */
#define START_ZONE_HEIGHT 3    /* altezza zona di partenza */
#define RIVER_LANES_NUMBER 8    /* numero corsie fiume*/


/*----------------------------------------------------------------------
   			   STRUTTURE
   ----------------------------------------------------------------------*/
// Struttura dati di gioco
typedef struct{
    _Bool game_lost;
    _Bool game_won;
    _Bool available_dens[5];
    int player_score;
    int player_lives;
    int difficulty;
} GameData;

// Struttura dati della direzione
enum Direction {
    LEFT,
    RIGHT
};

typedef struct {
    Direction direction; // Direzione del flusso: 0 per sinistra, 1 per destra
    int speed;     // Velocità del flusso
} RiverFlow;

// Struttura dati della difficoltà
enum Difficulty {
    EASY,
    NORMAL,
    HARD
};

// Struttura dati della rana
typedef struct 
{
   int id;
   int x;
   int y;
   bool frog_canshoot;
   bool frog_candie;
   bool frog_bulletisactive;
   int time_left;
} Position;

// Struttura dati del coccodrillo
typedef struct {
    int id;
    int x;
    int y;

    enum Direction direction;
    bool is_good;
    //int direction;
} Crocodile;

// Struttura dati della pianta
typedef struct {
    int id;
    int x;
    int y;

   bool plant_canshoot;
} Plant;


extern RiverFlow river_flows[]; // Dichiarazione della variabile esterna


/*----------------------------------------------------------------------
   			   PARAMETRI DI GIOCO
   ----------------------------------------------------------------------*/
#define FROG_BULLET_DELAY 50000

#define CROCODILE_DELAY_EASY 10000
#define CROCODILE_DELAY_NORMAL 8000
#define CROCODILE_DELAY_HARD 6000

#define MIN_RIVER_SPEED_EASY 10000
#define MIN_RIVER_SPEED_NORMAL 15000
#define MIN_RIVER_SPEED_HARD 20000

#define MAX_RIVER_SPEED_EASY 15000
#define MAX_RIVER_SPEED_NORMAL 20000
#define MAX_RIVER_SPEED_HARD 30000

#define TIMELIMIT_EASY 60
#define TIMELIMIT_NORMAL 45
#define TIMELIMIT_HARD 30

#define DIFFICULTIES 3 //possibili difficoltà di gioco

#define N_DENS 5 //numero di tane
#define N_PLANTS 3 //numero di vite
#define N_LIVES 3 //numero di vite
#define N_CROCODILE 24 //numero di coccodrilli se ne mettiamo 3 per corsia (eventualmente aumentabile a 4)


/*----------------------------------------------------------------------
   			   ID OGGETTI
   ----------------------------------------------------------------------*/

#define FROG_ID 1
#define FROG_BULLET_ID 2

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
#define CROCODILE_ID_24 27

#define PLANT_ID_0 28
#define PLANT_ID_1 29
#define PLANT_ID_2 30

 #define TIME_ID 31
 
 
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



/*----------------------------------------------------------------------
   				FUNZIONI
   ----------------------------------------------------------------------*/

//graphic.c
void initializeScr();	// inizializzazione dello schermo per ncurses

// menu.c
void mainMenu();    // visualizzazione del menu principale

// menu.c
void menuDifficulty();     // visualizzazione del menu per la scelta della difficoltà

// menu.c
void endGameMenu(bool win);     // menu di fine partita in base a se si è vinto o meno

//graphic.c
void gameField();   //disegna il terreno di gioco

//graphic.c
void frogBody(int y, int x);   //disegna lo sprite della rana

//graphic.c
void crocodileBody(Crocodile c);	//disegna lo sprite del coccodrillo

//graphic.c
void plantBody(Plant p);


void game(GameData gamedata);    // creazione e comunicazione tra processi


