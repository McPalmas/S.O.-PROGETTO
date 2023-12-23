
/*-----------------------------------------------------------------------
   LIBRERIE INCLUSE ALL'INTERNO DEL PROGRAMMA
   -----------------------------------------------------------------------*/

#include "include.h"


/*----------------------------------------------------------------------
   PROCEDURA CHE INIZIALIZZA LO SCHERMO PER NCURSES
   ----------------------------------------------------------------------*/
void initializeScr(){

    initscr(); /* Inizializza lo schermo */
    start_color(); /* Inizializza i colori in ncurses */
    noecho(); /* I caratteri non verranno visualizzati a schermo */
    curs_set(0); /* Nasconde il cursore */
    keypad(stdscr, 1);  /* abilita l'utilizzo delle frecce direzionali */

    init_pair(1,COLOR_WHITE,COLOR_GREEN); /* MENU */
    init_pair(2,COLOR_WHITE,COLOR_GREEN); /* TITLE VICTORY */
    init_pair(3,COLOR_WHITE,COLOR_RED);  /* TITLE GAMEOVER */

}




