#include "include.h"


/* ----------------------------------------------  
		  MENU PRINCIPALE
   ----------------------------------------------*/


void mainMenu(){
    int selection = 0; /* Tiene traccia dell'opzione correntemente selezionata */
    int c; /* Carattere della Getch, per le varie opzioni del menù */
    bool exitMenu = false;

    bkgd(COLOR_PAIR(WHITE_GREEN)); /* Setta il background color dello schermo */
    
    mvprintw(MAXY/3-5, 5,"     _____  _____   _____   _____  _____  ____  _____  \n");
    mvprintw(MAXY/3-4, 5,"    |_____ |_____] |     | |   _  |   _  |____ |_____] \n");
    mvprintw(MAXY/3-3, 5,"    |      |   \\\\  |_____| |____| |____| |____ |   \\\\  \n");

    mvprintw(MAXY/3-1, 5,"           _____         _____         ____   _    _____  _____ \n");
    mvprintw(MAXY/3, 6,  "  \\  /   |____         |_____] |     | __ | |\\  |   |   |____  \n");
    mvprintw(MAXY/3+1, 6,"   \\/    _____|        |       |____ |    | | \\ |   |   _____| \n");


    mvprintw(MAXY/3+6, 23, " Play             Quit");

    mvprintw(MAXY/3+8, 13, " SPACE to select, ARROW KEYS to move ");
    
     while(!exitMenu) { /* Finché non si è scelto una delle due opzioni per poter uscire dal menù */
        mvprintw(MAXY/3+6, 23, " ");
        mvprintw(MAXY/3+6, 40, " ");
        
        if(selection==0)
                mvprintw(MAXY/3+6, 23, ">");
        else
                mvprintw(MAXY/3+6, 40, ">");

        refresh();
        c = getch();

        switch(c) {

            case KEY_RIGHT: /* L'UTENTE PREME LA FRECCIA VERSO DESTRA */

                if(selection == 1) selection = 0;
                else selection++;
                break;

            case KEY_LEFT: /* L'UTENTE PREME LA FRECCIA VERSO SINISTRA */

                if(selection == 0) selection = 1;
                else selection--;
                break;

            case SPACE: /* L'UTENTE PREME LA BARRA SPAZIATRICE PER CONFERMARE L'OPZIONE SCELTA */
                if(selection == 0) { /* Passa al menu selezione della difficoltà*/
                    exitMenu = true;
                }
                if(selection == 1){  /* Esci dal gioco */
                    endwin();
                    exit(0);
                }
                break;
            default:
                break;
        }
    }

    mvprintw(MAXY/3+6, 12, "										");

    mvprintw(MAXY/3+8, 13, " 										");
    
    menuDifficulty();

}



/*----------------------------------------------------------------------
   		      MENU SCELTA DELLA DIFFICOLTÀ
   ----------------------------------------------------------------------*/
void menuDifficulty(){
    int selection = 0; /* Tiene traccia dell'opzione correntemente selezionata */
    int c; /* Carattere della Getch, per le varie opzioni del menù */
    bool exitMenu = false;

    mvprintw(MAXY/3+6, 12, " Easy             Medium            Hard ");

    mvprintw(MAXY/3+8, 13, " SPACE to select, ARROW KEYS to move ");
    

    /* In questo While viene gestito il menù d'inizio */
    while(!exitMenu) { /* Finché non si è scelto una delle due opzioni per poter uscire dal menù */
        mvprintw(MAXY/3+6, 11, " ");

        mvprintw(MAXY/3+6, 28, " ");

        mvprintw(MAXY/3+6, 46, " ");

        switch(selection) { /* Visualizza la freccia accanto all'opzione selezionata */
            case 0:
                mvprintw(MAXY/3+6, 11, ">");
                break;
            case 1:
                mvprintw(MAXY/3+6, 28, ">");
                break;
            case 2:
                mvprintw(MAXY/3+6, 46, ">");
                break;
            default:
                break;

        }

        refresh();
        c = getch();

        switch(c) {

            case KEY_RIGHT: /* L'UTENTE PREME LA FRECCIA VERSO DESTRA */

                if(selection == 2) { /* Cicla le opzioni del menù */

                    selection = 0;
                }
                else {
                    selection++;
                }
                break;

            case KEY_LEFT: /* L'UTENTE PREME LA FRECCIA VERSO SINISTRA */

                if(selection == 0) { /* Cicla le opzioni del menù */
                    selection = 2;
                }
                else {
                    selection--;
                } break;

            case SPACE: /* L'UTENTE PREME LA BARRA SPAZIATRICE PER CONFERMARE L'OPZIONE SCELTA */
                if(selection == 0) { /* Play: Avvia il gioco in modalita easy */
                    exitMenu = true;
                }
                if(selection == 1) { /* Play: Avvia il gioco in modalita medium  */
                    exitMenu = true;
                }
                if(selection == 2) { /* Play: Avvia il gioco in modalita hard  */
                    exitMenu = true;
                }
                break;
            default:
                break;
        }
    }
    
    refresh();
    gameField();
    Crocodile cr;			// righe di prova per le stampe da non prendere sul serio
    cr.x=(MAXX)/2; cr.y=32; cr.is_good=false;
    frogBody((MAXX)/2, 35);
    crocodileBody(cr);
    getch();
}




/*----------------------------------------------------------------------
   		      MENU DI FINE PARTITA
   ----------------------------------------------------------------------*/
void endGameMenu(bool win){
    int selection = 0;
    int c; 
    bool exitMenu = false;
    
    if(win){ 	/* in base alla vatiabile win si sceglie la scritta e il colore */
	    bkgd(COLOR_PAIR(BLACK_GREEN)); /* Setta il background color dello schermo */
	    
	    mvprintw(MAXY/3-5, 5,"     _____  _____   _     _____  _____   ____  _____  ____\n");
	    mvprintw(MAXY/3-4, 5,"    |      |     | |\\\\ | |   _  |_____] | __ |   |   |____\n");
	    mvprintw(MAXY/3-3, 5,"    |_____ |_____| | \\\\| |____| |   \\\\  |    |   |   _____|\n");

	    mvprintw(MAXY/3-1, 5,"   \\  / _____                        _____   _   \n");
	    mvprintw(MAXY/3, 6,  "   \\/ |     | |     |       \\    / |     | |\\\\ |\n");
	    mvprintw(MAXY/3+1, 6,"   /  |_____| |_____|        \\/\\/  |_____| | \\\\| \n");

    }else{
    	    bkgd(COLOR_PAIR(BLACK_RED)); /* Setta il background color dello schermo */
	    
	    mvprintw(MAXY/3-5, 20,"     _____  ____   _  _   ____\n");
	    mvprintw(MAXY/3-4, 20,"    |   _  | __ | | \\/ | |___ \n");
	    mvprintw(MAXY/3-3, 20,"    |____| |    | |    | |____\n");

	    mvprintw(MAXY/3-1, 20,"    _____        ____  _____  \n");
	    mvprintw(MAXY/3, 20,  "   |     | \\  / |___  |_____] \n");
	    mvprintw(MAXY/3+1, 20,"   |_____|  \\/  |____ |   \\\\\n");

    }
    
    mvprintw(MAXY/3+6, 23, " Play Again             Quit");

    mvprintw(MAXY/3+8, 17, " SPACE to select, ARROW KEYS to move ");
    
     while(!exitMenu) { /* Finché non si è scelto una delle due opzioni per poter uscire dal menù */
        mvprintw(MAXY/3+6, 23, " ");
        mvprintw(MAXY/3+6, 46, " ");
        
        if(selection==0)
                mvprintw(MAXY/3+6, 23, ">");
        else
                mvprintw(MAXY/3+6, 46, ">");

        refresh();
        c = getch();

        switch(c) {

            case KEY_RIGHT: /* L'UTENTE PREME LA FRECCIA VERSO DESTRA */

                if(selection == 1) selection = 0;
                else selection++;
                break;

            case KEY_LEFT: /* L'UTENTE PREME LA FRECCIA VERSO SINISTRA */

                if(selection == 0) selection = 1;
                else selection--;
                break;

            case SPACE: /* L'UTENTE PREME LA BARRA SPAZIATRICE PER CONFERMARE L'OPZIONE SCELTA */
                if(selection == 0) { /* Passa al menu selezione della difficoltà*/
                    exitMenu = true;
                }
                if(selection == 1){  /* Esci dal gioco */
                    endwin();
                    exit(0);
                }
                break;
            default:
                break;
        }
    }
    
    menuDifficulty();
	
	
}





