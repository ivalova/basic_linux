#include "execute_regime.h"
#include "common.h"

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

        while(1)
        {
        user_input = 0;
        user_input_dec = 0;

        printf("Select program mode: ");
        
        //user_input = getchar();
        while((user_input = getchar()) == '\n');

        user_input_dec = user_input - ascii_to_dec;

        if(!is_mode_valid(user_input_dec))
        {
        printf("Invalid mode selected! Valid options are [0-4]. You have selected: %d \n", user_input_dec); 
               continue;
        }

        printf("You have selected mode: %d \n", user_input_dec);

        switch(user_input_dec){
                case MODE_NORMAL:
                        /*break intentionally ommited */
                case MODE_STOP_SENDING:
                        pthread_mutex_lock(&program_mutex);
                        printf("program mode updated\n");
                        program_mode = user_input_dec;
                        pthread_mutex_unlock(&program_mutex);
                        sem_post(&semStart);
                        break;
                case MODE_CUSTOM_MSG:
                        pthread_mutex_lock(&program_mutex);
                        program_mode = user_input_dec;

                        printf("Select predefined message: ");

                        while((user_input = getchar()) == '\n');
                        user_input_dec = user_input - ascii_to_dec;

                        if(!is_custom_msg_valid(user_input_dec))
                        {
                                printf("Invalid custom message selected! Valid options are [0-4]. You have selected: %d \n", user_input_dec); 
                                continue;
                        }
                        msg_option = user_input_dec;
                        
                        
                        pthread_mutex_unlock(&program_mutex);
                        sem_post(&semStart);
                        printf("sem posted for MODE_CUSTOM_MSG");
                        /*break intentionally ommited */
                case MODE_CUSTOM_MSG_ERR:
                        break;
                case MODE_EXIT:
                        sem_post(&semFinishSignal);
                        printf("terminate user_handler.\n");
                        return;
                        break;
        }

        }
       
   
}

bool is_mode_valid(uint8_t input)
{
        bool            valid = false;
        
        if(input < TEST_MODE_COUNT){
                valid = true;
        }

        return valid;
}

bool is_custom_msg_valid(uint8_t input)
{
        bool            valid = false;
        
        if(input < CUSTOM_MSG_COUNT){
                valid = true;
        }
        
        return valid;
}
