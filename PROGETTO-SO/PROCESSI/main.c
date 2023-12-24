
/*------------------------------------------------------------------
   LIBRERIE INCLUSE ALL'INTERNO DEL PROGRAMMA
   ---------------------------------------------------------------*/
#include "include.h"



/* ----------------------------------------------------------------
				MAIN
   --------------------------------------------------------------*/

int main(){

    initializeScr(); 
    
    /* rende  disponibile l'input dell'utente  */
    cbreak(); 

    /* funzione per il menu principale */
    mainMenu();

    /* Ripristino la modalità di funzionamento normale del terminale */
    endwin();

    /* Termino il gioco ed esco dal Pogramma */
    printf("\n\n Uscito dal programma.\n\n");

    return 0;
}
