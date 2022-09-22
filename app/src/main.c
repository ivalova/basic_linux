#include "user_handler.h"
#include "execute_regime.h"

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

char*           path;
uint8_t         msg_option; //1,2,3,4,5

void* execute_regime(void* args)
{
        printf("Hello from execute_regime\n");
        test_setup();

        while (1) {
                if (sem_trywait(&semFinishSignal) == 0) {
                    break;
                }

                if (sem_trywait(&semStart) == 0) {
                        switch (program_mode) {
                                case MODE_NORMAL:
                                        normal_regime(path);
                                        break;
                                case MODE_CUSTOM_MSG:
                                        test_regime(msg_option, path);
                                        break;
                                case MODE_CUSTOM_MSG_ERR:
                                        //error_regime();
                                        break;
                                case MODE_STOP_SENDING:
                                        //idle_regime();
                                        break;
                                default:
                                        printf("Invalid mode selected, please select a valid mode!\n");
                                        break;
                        }
                }
        }

        printf("Shutting down!\n");
        return 0;
}

int main (int argc, char *argv[])
{
        if (argc != 2) {
                printf("This application supports one command line argument: character device driver path\n");
                return -1;
        }

        pthread_t user_handler_thread;
        pthread_t execute_regime_thread;

        sem_init(&semFinishSignal,0,0);
        sem_init(&semStart,0,0);

        pthread_create(&user_handler_thread, NULL, user_handler, 0);
        pthread_create(&execute_regime_thread, NULL, execute_regime, 0);

        pthread_join(user_handler_thread, NULL);
        pthread_join(execute_regime_thread, NULL);

        printf("Exiting...\n");

        return 0;
}
