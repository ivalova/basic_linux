#include "execute_regime.h"
#include "common.h"
#include "ioctl_interface.h"

#include <stdio.h>
#include <stdbool.h>

//todo add description
bool is_mode_valid(uint8_t input);
bool is_custom_msg_valid(uint8_t input);

void user_handler(void)
{       
        int             user_input;
        int             user_input_dec;
        const int       ascii_to_dec = 0x30;

        printf("Always enter one character followed by enter for each command.\n");
        while(1)
        {
        user_input = 0;
        user_input_dec = 0;

        if (sem_wait(&semGetInput) == 0) {
        printf("Select program mode: ");

        while((user_input = getchar()) == '\n');

        user_input_dec = user_input - ascii_to_dec;

        if(!is_mode_valid(user_input_dec))
        {
        printf("Invalid mode selected! Valid options are [0-4]. You have selected ASCII value: %d \n", user_input); 
               continue;
        }

        PRINT_DEBUG("You have selected mode: %d \n", user_input_dec);

        switch(user_input_dec){
                case MODE_NORMAL:
                        PRINT_DEBUG("MODE_NORMAL\n");

                        pthread_mutex_lock(&program_mutex);
                        program_mode = user_input_dec;
                        pthread_mutex_unlock(&program_mutex);

                        sem_post(&semStart);
                        break;

                case MODE_STOP_SENDING:
                        PRINT_DEBUG("MODE_STOP_SENDING\n");

                        pthread_mutex_lock(&program_mutex);
                        program_mode = user_input_dec;
                        pthread_mutex_unlock(&program_mutex);

                        sem_post(&semTerminate);
                        sem_post(&semStart);
                        break;

                case MODE_CUSTOM_MSG:
                        PRINT_DEBUG("MODE_CUSTOM_MSG\n");

                        pthread_mutex_lock(&program_mutex);
                        program_mode = user_input_dec;
                        pthread_mutex_unlock(&program_mutex);

                        printf("Select predefined message: ");

                        while((user_input = getchar()) == '\n');
                        user_input_dec = user_input - ascii_to_dec;

                        if(!is_custom_msg_valid(user_input_dec))
                        {
                                printf("Invalid custom message selected! Valid options are [0-%d]. You have selected ASCII value: %d \n", TEST_MODE_COUNT-1, user_input); 

                                continue;
                        }

                        pthread_mutex_lock(&program_mutex);
                        msg_option = user_input_dec;
                        pthread_mutex_unlock(&program_mutex);

                        sem_post(&semStart);
                        PRINT_DEBUG("sem posted for MODE_CUSTOM_MSG\n");
                        break;
                case MODE_CUSTOM_MSG_ERR:
                        PRINT_DEBUG("MODE_CUSTOM_MSG\n");

                        pthread_mutex_lock(&program_mutex);
                        program_mode = user_input_dec;
                        pthread_mutex_unlock(&program_mutex);

                        printf("Select predefined message: ");

                        while((user_input = getchar()) == '\n');
                        user_input_dec = user_input - ascii_to_dec;

                        if(!is_custom_msg_valid(user_input_dec))
                        {
                                printf("Invalid custom message selected! Valid options are [0-%d]. You have selected ASCII value: %d \n", CUSTOM_MSG_COUNT-1, user_input); 
                                pthread_mutex_unlock(&program_mutex);
                                continue;
                        }

                        pthread_mutex_lock(&program_mutex);
                        msg_option = user_input_dec;
                        pthread_mutex_unlock(&program_mutex);

                        sem_post(&semStart);
                        PRINT_DEBUG("sem posted for MODE_CUSTOM_MSG\n");

                        break;
                case MODE_EXIT:
                        sem_post(&semFinishSignal);
                        PRINT_DEBUG("terminate user_handler.\n");
                        return;
                        break;
        }

        }
        }
       
   
}

bool is_mode_valid(uint8_t input)
{
        bool    valid = false;
        
        if(input < TEST_MODE_COUNT){
                valid = true;
        }

        return valid;
}

bool is_custom_msg_valid(uint8_t input)
{
        bool    valid = false;
        
        if(input < CUSTOM_MSG_COUNT){
                valid = true;
        }
        
        return valid;
}