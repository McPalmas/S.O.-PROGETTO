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
   			   GREEN
   ----------------------------------------------------------------------*/
#define SCORE_ZONE_HEIGHT 3          /* altezza zona blue con i punteggi ecc.*/
#define DENS_ZONE_HEGHT 3       /* altezza zona tane */
#define PLANTS_ZONE_HEIGHT 4    /* altezza zona piante */
#define START_ZONE_HEIGHT 3    /* altezza zona di partenza */
#define RIVER_LANES_NUMBER 8    /* numero corsie fiume*/


/*----------------------------------------------------------------------
   			   STRUTTURE
   ----------------------------------------------------------------------*/

enum Direction {
    LEFT,
    RIGHT
};

typedef struct {
    int id;
    int x;
    int y;

    enum Direction direction;
    bool is_good;


} Crocodile;

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

