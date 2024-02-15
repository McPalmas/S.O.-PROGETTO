
#include "include.h"

/* ----------------------------------------------------------------
            MAIN
   --------------------------------------------------------------*/
int main()
{
   // Inizializzazione
   initializeScr();
   cbreak();

   // Gioco
   mainMenu();

   // Uscita
   endwin();
   printf("\n\n Uscito dal programma.\n\n");
   return 0;
}
