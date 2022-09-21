#include "user_handler.h"
#include "execute_regime.h"

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


void* execute_regime(void* args)
{
    printf("Hello from execute_regime\n");
}

int main (int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("This application supports one command line argument: character device driver path\n");
        return -1;
    }

    pthread_t user_handler_thread;
    pthread_t execute_regime_thread;

    pthread_create(&user_handler_thread, NULL, user_handler, 0);
    pthread_create(&execute_regime_thread, NULL, execute_regime, 0);

    pthread_join(user_handler_thread, NULL);
    pthread_join(execute_regime_thread, NULL);

    printf("Exiting...\n");

    return 0;
}
