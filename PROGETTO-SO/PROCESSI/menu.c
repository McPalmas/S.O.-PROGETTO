#include "include.h"


/* ----------------------------------------------  
			MENU
   ----------------------------------------------*/


void menu(){
    int selection = 0; /* Tiene traccia dell'opzione correntemente selezionata */
    int c; /* Carattere della Getch, per le varie opzioni del menù */
    bool exitMenu = false;

    bkgd(COLOR_PAIR(1)); /* Setta il background color dello schermo */

    mvprintw(MAXY/3-5, 5,"     _____  _____   _____   _____  _____  ____  _____  \n");
    mvprintw(MAXY/3-4, 5,"    |_____ |_____] |     | |   _  |   _  |____ |_____] \n");
    mvprintw(MAXY/3-3, 5,"    |      |   \\\\  |_____| |____| |____| |____ |   \\\\  \n");

    mvprintw(MAXY/3-1, 5,"           _____         _____         ____   _    _____  _____ \n");
    mvprintw(MAXY/3, 6,  "  \\  /   |____         |_____] |     | __ | |\\  |   |   |____  \n");
    mvprintw(MAXY/3+1, 6,"   \\/    _____|        |       |____ |    | | \\ |   |   _____| \n");


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
    attroff(COLOR_PAIR(3));

    /* QUESTA MVPRINTW CANCELLA ' EASY/MEDIUM/HARD ' */
    mvprintw(MAXY/3+6, 12, "                                                                                   \n");

    /* QUESTA MVPRINTW CANCELLA ' SPACE TO SELECT, ARROW KEYS TO CYCLE ' */
    mvprintw(MAXY/3+8, 15, "                                                                                   \n");
    bkgd(COLOR_PAIR(0));

    refresh();


}










