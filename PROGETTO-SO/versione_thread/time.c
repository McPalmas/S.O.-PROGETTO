#include "include.h"

/* ----------------------------------------------
          TIMER PARTITA
   ----------------------------------------------*/
void *time_thread(void *data, void *gameData)
{
    objectData *timeData = (objectData *)data;
    GameData *gamedata = (GameData *)gameData;

    objectData time;
    time.time_left = timeData->time_left;
    int difficulty;
    difficulty = gamedata->difficulty;

    // Tempo massimo in base alla difficoltà
    if (difficulty == EASY)
    {
        time.time_left = TIMELIMIT_EASY;
    }
    else if (difficulty == NORMAL)
    {
        time.time_left = TIMELIMIT_NORMAL;
    }
    else
    {
        time.time_left = TIMELIMIT_HARD;
    }

    insertObject(time);
    // Invio del dato
    while (should_not_exit)
    {
        while (block)
        {
        }

        sleep(1);
        // Aggiorna IL TIMER -> Probabilmente anche lui va gestito con produttore consumatore e non con globale
        time.time_left--;
        insertObject(time);
    }
}
