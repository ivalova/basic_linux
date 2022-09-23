#include "device_com.h"
#include "user_handler.h"
#include "execute_regime.h"

char* device_path;
uint8_t msg_option; //1,2,3,4,5
sem_t semFinishSignal;
sem_t semGetInput;
sem_t semTerminate;
sem_t semStart;
pthread_mutex_t  program_mutex;

enum mode program_mode;

int main (int argc, char *argv[])
{
        if (argc != 2) {
                printf("This application supports one command line argument: character device driver path\n");
                return -1;
        }

        device_path = argv[1];

        pthread_t user_handler_thread;
        pthread_t execute_regime_thread;

        sem_init(&semFinishSignal,0,0);
        sem_init(&semGetInput,0,1);
        sem_init(&semTerminate,0,0);
        sem_init(&semStart,0,0);

        pthread_create(&user_handler_thread, NULL, user_handler, 0);
        pthread_create(&execute_regime_thread, NULL, execute_regime, 0);

        pthread_join(user_handler_thread, NULL);
        pthread_join(execute_regime_thread, NULL);

        printf("Exiting...\n");

        return 0;
}
