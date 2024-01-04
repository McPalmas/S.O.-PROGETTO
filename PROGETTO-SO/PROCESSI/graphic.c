
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

    init_pair(WHITE_GREEN,COLOR_WHITE,COLOR_GREEN); 
    init_pair(WHITE_RED,COLOR_WHITE,COLOR_RED);
    init_pair(BLACK_BLACK, COLOR_BLACK, COLOR_BLACK);
    init_pair(WHITE_WHITE, COLOR_WHITE, COLOR_WHITE);
    init_pair(GREEN_GREEN, COLOR_GREEN, COLOR_GREEN);
    init_pair(CYAN_CYAN, COLOR_CYAN, COLOR_CYAN);
    init_pair(MAGENTA_MAGENTA, COLOR_MAGENTA, COLOR_MAGENTA);
    init_pair(BLUE_BLUE, COLOR_BLUE, COLOR_BLUE);
    init_pair(GREEN_BLACK, COLOR_GREEN, COLOR_BLACK); 
    init_pair(GREEN_YELLOW, COLOR_GREEN, COLOR_YELLOW);
    init_pair(RED_YELLOW, COLOR_RED, COLOR_YELLOW);
    init_pair(BLACK_RED, COLOR_BLACK, COLOR_RED); 
    init_pair(BLACK_GREEN, COLOR_BLACK, COLOR_GREEN); 
    init_pair(RED_GREEN, COLOR_RED, COLOR_GREEN); 
    init_pair(YELLOW_GREEN, COLOR_YELLOW, COLOR_GREEN); 
    init_pair(GREEN_MAGENTA, COLOR_GREEN, COLOR_MAGENTA); 
    init_pair(BLACK_WHITE, COLOR_BLACK, COLOR_WHITE);
}


/*----------------------------------------------------------------------
   PROCEDURA CHE STAMPA LE VARIE AREE DEL TERRENO DI GIOCO
   ----------------------------------------------------------------------*/
void gameField(){
    int i; 
    int y = 0;

    bkgd(COLOR_PAIR(BLACK_BLACK)); /* Setta il background color dello schermo */

    // stampa spazio dello score
    for(i = 0; i < SCORE_ZONE_HEIGHT; i++){
        attron(COLOR_PAIR(BLUE_BLUE));
            mvhline(y, 0, ' ', MAXX);
        attroff(COLOR_PAIR(BLUE_BLUE));
        y++;
    }

    
    // stampa zona tane
    for(i = 0; i < DENS_ZONE_HEIGHT; i++){
        attron(COLOR_PAIR(GREEN_GREEN));
            mvhline(y, 0, ' ', MAXX);
        attroff(COLOR_PAIR(GREEN_GREEN));
        y++;
    }


    // stampa zona piante
    for(i = 0; i < PLANTS_ZONE_HEIGHT; i++){
        attron(COLOR_PAIR(MAGENTA_MAGENTA));
            mvhline(y, 0, ' ', MAXX);
        attroff(COLOR_PAIR(MAGENTA_MAGENTA));
        y++;
    }


    // stampa zona piante
    for(i = 0; i < (2*RIVER_LANES_NUMBER); i++){
        attron(COLOR_PAIR(CYAN_CYAN));
            mvhline(y, 0, ' ', MAXX);
        attroff(COLOR_PAIR(CYAN_CYAN));
        y++;
    }
    

    // stampa zona di partenza
    for(i = 0; i < START_ZONE_HEIGHT; i++){
        attron(COLOR_PAIR(MAGENTA_MAGENTA));
            mvhline(y, 0, ' ', MAXX);
        attroff(COLOR_PAIR(MAGENTA_MAGENTA));
        y++;
    }
    

    // stampa spazio delle vite e del tempo di gioco
    for(i = 0; i < SCORE_ZONE_HEIGHT; i++){
        attron(COLOR_PAIR(BLUE_BLUE));
            mvhline(y, 0, ' ', MAXX);
        attroff(COLOR_PAIR(BLUE_BLUE));
        y++;
    }
    
    refresh();

}





/*----------------------------------------------------------------------
   	    PROCEDURA CHE STAMPA LE TANE
   ----------------------------------------------------------------------*/
void printDens(int dens[]){
	int start_dens[5] = {6,17,28,39,50};
	for(int i=0; i < 5; i++)
		if(dens[i]==0){
			attron(COLOR_PAIR(WHITE_WHITE));
			mvaddch(SCORE_ZONE_HEIGHT, start_dens[i], ' ');
			mvaddch(SCORE_ZONE_HEIGHT, start_dens[i]+1, ' ');
			mvaddch(SCORE_ZONE_HEIGHT, start_dens[i]+2, ' ');
			mvaddch(SCORE_ZONE_HEIGHT, start_dens[i]+3, ' ');
			mvaddch(SCORE_ZONE_HEIGHT, start_dens[i]+4, ' ');
			mvaddch(SCORE_ZONE_HEIGHT+1, start_dens[i], ' ');
			mvaddch(SCORE_ZONE_HEIGHT+1, start_dens[i]+1, ' ');
			mvaddch(SCORE_ZONE_HEIGHT+1, start_dens[i]+2, ' ');
			mvaddch(SCORE_ZONE_HEIGHT+1, start_dens[i]+3, ' ');
			mvaddch(SCORE_ZONE_HEIGHT+1, start_dens[i]+4, ' ');	
			mvaddch(SCORE_ZONE_HEIGHT+2, start_dens[i], ' ');
			mvaddch(SCORE_ZONE_HEIGHT+2, start_dens[i]+1, ' ');
			mvaddch(SCORE_ZONE_HEIGHT+2, start_dens[i]+2, ' ');
			mvaddch(SCORE_ZONE_HEIGHT+2, start_dens[i]+3, ' ');
			mvaddch(SCORE_ZONE_HEIGHT+2, start_dens[i]+4, ' ');	
			attroff(COLOR_PAIR(WHITE_WHITE));	
		}else{
			attron(COLOR_PAIR(BLACK_WHITE));
			mvaddch(SCORE_ZONE_HEIGHT, start_dens[i], ' ');
			mvaddch(SCORE_ZONE_HEIGHT, start_dens[i]+1, '\\');
			mvaddch(SCORE_ZONE_HEIGHT, start_dens[i]+2, ' ');
			mvaddch(SCORE_ZONE_HEIGHT, start_dens[i]+3, '/');
			mvaddch(SCORE_ZONE_HEIGHT, start_dens[i]+4, ' ');
			mvaddch(SCORE_ZONE_HEIGHT+1, start_dens[i], ' ');
			mvaddch(SCORE_ZONE_HEIGHT+1, start_dens[i]+1, ' ');
			mvaddch(SCORE_ZONE_HEIGHT+1, start_dens[i]+2, 'X');
			mvaddch(SCORE_ZONE_HEIGHT+1, start_dens[i]+3, ' ');
			mvaddch(SCORE_ZONE_HEIGHT+1, start_dens[i]+4, ' ');	
			mvaddch(SCORE_ZONE_HEIGHT+2, start_dens[i], ' ');
			mvaddch(SCORE_ZONE_HEIGHT+2, start_dens[i]+1, '/');
			mvaddch(SCORE_ZONE_HEIGHT+2, start_dens[i]+2, ' ');
			mvaddch(SCORE_ZONE_HEIGHT+2, start_dens[i]+3, '\\');
			mvaddch(SCORE_ZONE_HEIGHT+2, start_dens[i]+4, ' ');	
			attroff(COLOR_PAIR(BLACK_WHITE));
		
		}


}





/*----------------------------------------------------------------------
   	   PROCEDURA CHE STAMPA LO SPRITE DELLA RANA
   ----------------------------------------------------------------------*/
void frogBody(int x, int y){				   
	// 	\0 0/
	//	/(_)\						
    
    attron(COLOR_PAIR(GREEN_BLACK));				
    mvaddch(y, x - 2, '\\');
    attroff(COLOR_PAIR(GREEN_BLACK));
    
    attron(COLOR_PAIR(RED_GREEN));
    mvaddch(y, x - 1, '0');
    mvaddch(y, x, ' ');
    mvaddch(y, x + 1, '0');
    attroff(COLOR_PAIR(RED_GREEN));
    
    attron(COLOR_PAIR(GREEN_BLACK));
    mvaddch(y, x + 2, '/');
    attroff(COLOR_PAIR(GREEN_BLACK));
        
    attron(COLOR_PAIR(GREEN_BLACK));    
    mvaddch(y+1, x - 2, '/');
    attroff(COLOR_PAIR(GREEN_BLACK));
    
    attron(COLOR_PAIR(GREEN_GREEN)); 
    mvaddch(y+1, x - 1, '(');
    mvaddch(y+1, x, '_');
    mvaddch(y+1, x + 1, ')');
    attroff(COLOR_PAIR(GREEN_GREEN)); 
    
    attron(COLOR_PAIR(GREEN_BLACK));
    mvaddch(y+1, x + 2, '\\');
    attroff(COLOR_PAIR(GREEN_BLACK));
}




/*----------------------------------------------------------------------
   	    PROCEDURA CHE STAMPA LO SPRITE DEL COCCODRILLO 
   ----------------------------------------------------------------------*/
void crocodileBody(Crocodile c){

    /* se il coccodrillo è buono è verde senno è rosso*/
    if(c.is_good){
        attron(COLOR_PAIR(BLACK_GREEN));
        if(c.direction == RIGHT){  /* stampa del coccodrillo in base alla direzione */
            mvprintw(c.y, c.x,   "       0__");
            mvprintw(c.y+1, c.x, " |/  |/   ");
        }else{
            mvprintw(c.y, c.x,   "__0       ");
            mvprintw(c.y+1, c.x, "   \\|  \\| ");
        }
        attroff(COLOR_PAIR(BLACK_GREEN));
        
    }else{
    	attron(COLOR_PAIR(BLACK_RED));
    	if(c.direction == RIGHT){
            mvprintw(c.y, c.x,  "       0__");
            mvprintw(c.y+1, c.x," |/  |/   ");
        }else{
            mvprintw(c.y, c.x,   "__0       ");		
            mvprintw(c.y+1, c.x, "   \\|  \\| ");
        }
        attroff(COLOR_PAIR(BLACK_RED));
    }
}




/*----------------------------------------------------------------------
   	    PROCEDURA CHE STAMPA LO SPRITE DELLA PIANTA
   ----------------------------------------------------------------------*/
void plantBody(Position p){

        attron(COLOR_PAIR(GREEN_YELLOW));
            mvprintw(p.y, p.x+1, "0 ");
        attroff(COLOR_PAIR(YELLOW_GREEN));
        attron(COLOR_PAIR(GREEN_MAGENTA));
        mvprintw(p.y+1, p.x, "_|_");
        attroff(COLOR_PAIR(GREEN_MAGENTA));
       
}

