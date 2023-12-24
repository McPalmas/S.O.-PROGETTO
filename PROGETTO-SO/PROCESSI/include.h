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
#define MAXX    80    /* Dimensione dello schermo di output (colonne) */
#define MAXY    40    /* Dimensione dello schermo di output (righe)   */









/*----------------------------------------------------------------------
   				FUNZIONI
   ----------------------------------------------------------------------*/

//graphic.c
void initializeScr();	// inizializzazione dello schermo per ncurses

// menu.c
void mainMenu();    // visualizzazione del menu principale

// menu.c
void menuDifficulty();     // visualizzazione del menu per la scelta della difficoltà

//graphic.c
void gameField();   //disegna il terreno di gioco



