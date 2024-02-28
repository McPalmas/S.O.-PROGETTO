#include "include.h"

/* ----------------------------------------------
          FROG
   ----------------------------------------------*/

pthread_mutex_t frogBulletMutex;
pthread_cond_t cond_var;
int thread_created = 0;

// Funzione per la gestione del thread frog
void *frog_thread(void *data)
{
    args *receivedData = (args *)data;
    objectData frog = receivedData->object;
    frog.thread_id = pthread_self();
    objectData river_flowData[RIVER_LANES_NUMBER];
    int laneY[RIVER_LANES_NUMBER];
    for (int i = 0; i < RIVER_LANES_NUMBER; ++i)
    {
        river_flowData[i].flow_speed = receivedData->riverFlow[i].flow_speed;
        river_flowData[i].direction = receivedData->riverFlow[i].direction;
        laneY[i] = SCORE_ZONE_HEIGHT + DENS_ZONE_HEIGHT + PLANTS_ZONE_HEIGHT + i * 2;
    }
    
    int timer_counter = 0;
    int last_frog_y = frog.y;

    pthread_t frog_bullet_thread_t;

    int frog_start_x = frog.x;
    int frog_start_y = frog.y;

    // Inizializza il mutex e la variabile di condizione
    pthread_mutex_init(&frogBulletMutex, NULL);
    pthread_cond_init(&cond_var, NULL);

    // Ciclo di esecuzione di Frog
    while (should_not_exit)
    {
        while (block)
        {
            int c = getch();
            if (c == 'q')
            {
                block = false;
                system("aplay ../SUONI/riverSound.wav > /dev/null 2>&1 &");
            }
        }

        // Attende l'input, altrimenti continua senza bloccare il gioco
        timeout(1);
        // Acquisizione input
        int c = getch();

        // Comandi Frog
        switch (c)
        {
        // Movimenti
        case KEY_UP:
            frog.y -= 2;
            system("aplay ../SUONI/jump.wav > /dev/null 2>&1 &");
            break;
        case KEY_DOWN:
            if (frog.y < frog_start_y)
            {
                frog.y += 2;
                system("aplay ../SUONI/jump.wav > /dev/null 2>&1 &");
            }
            break;
        case KEY_LEFT:
            frog.x -= 1;
            break;
        case KEY_RIGHT:
            frog.x += 1;
            break;
        // Proiettile
        case SPACE:
            if (!thread_created)
            {
                pthread_mutex_lock(&frogBulletMutex);
                // Inizializzazione proiettile
                objectData *frogBullet = (objectData *)malloc(sizeof(frogBullet));
                objectData frog_bullet = *frogBullet;
                frog_bullet.x = frog.x;
                frog_bullet.y = frog.y - 1;
                frog_bullet.id = FROG_BULLET_ID;
                // Creazione thread
                if (pthread_create(&frog_bullet_thread_t, NULL, &frog_bullet_thread, &frog_bullet) != 0)
                {
                    _exit(1);
                }
                thread_created = 1;
                pthread_mutex_unlock(&frogBulletMutex);

                system("aplay ../SUONI/lasershot.wav > /dev/null 2>&1 &");
            }
            break;
        case 'q':
            if (!block)
            {
                block = true;
                system("killall aplay");
                pausePrint();
            }
            break;
        }

        bool inRiver = false;
        int delay = 0;
        for(int i = 0; i < RIVER_LANES_NUMBER; i++)
        {
            if (frog.y == laneY[i])
            {   
                frog.flow_speed = river_flowData[i].flow_speed;
                frog.direction = river_flowData[i].direction;
                delay = frog.flow_speed;
                inRiver = true;
                //frog.x += (frog.direction == RIGHT) ? 1 : -1;               
                break;
            }
        }
        
        if(inRiver){
            timer_counter++;   
            if (timer_counter == delay/2000 - 27)
                {
            	    if(last_frog_y==frog.y)frog.x += (frog.direction == RIGHT) ? 1 : -1;
            	    timer_counter = 0; // Reimposta il contatore
        	}
        }
        	
        insertObject(frog);
        last_frog_y = frog.y;
        usleep(1000);//ogni millisecondo
    }
    // Rilascia il mutex e la variabile di condizione
    pthread_mutex_destroy(&frogBulletMutex);
    pthread_cond_destroy(&cond_var);
}

/* ----------------------------------------------
          FROG BULLET
   ----------------------------------------------*/
// Funzione per la gestione del processo frog_bullet
void *frog_bullet_thread(void *a)
{
    pthread_cleanup_push(bulletDeletion, NULL);
    objectData *frogBullet = (objectData *)a;
    objectData frog_bullet = *frogBullet;
    frog_bullet.thread_id = pthread_self();
    frog_bullet.frog_bulletisactive = true;
    insertObject(frog_bullet);
    while (should_not_exit)
    {
        while (block)
        {
        }
        pthread_testcancel();
        frog_bullet.y -= 1;

        insertObject(frog_bullet);
        usleep(FROG_BULLET_DELAY);
    }
    pthread_cleanup_pop(1);
}

void bulletDeletion()
{
    pthread_mutex_lock(&frogBulletMutex);
    thread_created = 0;             // Il thread è terminato, quindi possiamo creare un altro thread
    pthread_cond_signal(&cond_var); // Notifica il main thread che il thread è terminato
    pthread_mutex_unlock(&frogBulletMutex);
    pthread_exit(0);
}
