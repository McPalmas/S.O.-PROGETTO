
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
    init_pair(4, COLOR_BLACK, COLOR_BLACK);
    init_pair(5, COLOR_WHITE, COLOR_WHITE);
    init_pair(6, COLOR_GREEN, COLOR_GREEN);
    init_pair(7, COLOR_CYAN, COLOR_CYAN);
    init_pair(8, COLOR_MAGENTA, COLOR_MAGENTA);
    init_pair(9, COLOR_BLUE, COLOR_BLUE);
    init_pair(10, COLOR_WHITE, COLOR_BLACK);

}




void gameField(){
    int i; 
    int y = 0;

    bkgd(COLOR_PAIR(4)); /* Setta il background color dello schermo */

    // stampa spazio dello score
    for(i = 0; i < 4; i++){
        attron(COLOR_PAIR(9));
            mvhline(y, 0, ' ', MAXX);
        attroff(COLOR_PAIR(9));
        y++;
    }

    
    // stampa zona tane
    for(i = 0; i < 4; i++){
        attron(COLOR_PAIR(6));
            mvhline(y, 0, ' ', MAXX);
        attroff(COLOR_PAIR(6));
        y++;
    }


    // stampa zona piante
    for(i = 0; i < 4; i++){
        attron(COLOR_PAIR(8));
            mvhline(y, 0, ' ', MAXX);
        attroff(COLOR_PAIR(8));
        y++;
    }


    // stampa zona piante
    for(i = 0; i < (3*8); i++){
        attron(COLOR_PAIR(7));
            mvhline(y, 0, ' ', MAXX);
        attroff(COLOR_PAIR(7));
        y++;
    }
    


    // stampa zona di partenza
    for(i = 0; i < 3; i++){
        attron(COLOR_PAIR(8));
            mvhline(y, 0, ' ', MAXX);
        attroff(COLOR_PAIR(8));
        y++;
    }
    

    
    // stampa spazio delle vite e del tempo di gioco
    for(i = 0; i < 3; i++){
        attron(COLOR_PAIR(9));
            mvhline(y, 0, ' ', MAXX);
        attroff(COLOR_PAIR(9));
        y++;
    }
    
    refresh
    
    bool exit=false;
    int c=0;
    while(!exit){
    	getch();
    	if(KEY_RIGHT)exit=true;
    }

}



