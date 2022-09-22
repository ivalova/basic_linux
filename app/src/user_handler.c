#include "execute_regime.h"
#include "common.h"

#include <stdio.h>
#include <stdbool.h>

//todo add description
bool is_mode_valid(uint8_t input);
bool is_custom_msg_valid(uint8_t input);
bool is_error_index_valid(uint8_t input);
bool is_error_value_valid(uint8_t input);
bool is_error_value_valid(uint8_t input);

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
        printf("Invalid mode selected! Valid options are [0-4]. You have selected ASCII value: %d \n", user_input); 
               continue;
        }

        printf("You have selected mode: %d \n", user_input_dec);

        switch(user_input_dec){
                case MODE_NORMAL:
                        /*break intentionally ommited */
                case MODE_STOP_SENDING:
                        pthread_mutex_lock(&program_mutex);
                        PRINT_DEBUG("program mode updated\n");
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
                                printf("Invalid custom message selected! Valid options are [0-%d]. You have selected ASCII value: %d \n", TEST_MODE_COUNT-1, user_input); 
                                continue;
                        }
                        msg_option = user_input_dec;
                        
                        pthread_mutex_unlock(&program_mutex);
                        sem_post(&semStart);
                        PRINT_DEBUG("sem posted for MODE_CUSTOM_MSG\n");
                        break;
                case MODE_CUSTOM_MSG_ERR:
                        pthread_mutex_lock(&program_mutex);
                        program_mode = user_input_dec;

                        printf("Select predefined message: ");

                        while((user_input = getchar()) == '\n');
                        user_input_dec = user_input - ascii_to_dec;

                        if(!is_custom_msg_valid(user_input_dec))
                        {
                                printf("Invalid custom message selected! Valid options are [0-%d]. You have selected ASCII value: %d \n", CUSTOM_MSG_COUNT-1, user_input); 
                                continue;
                        }
                        msg_option = user_input_dec;

                        printf("Select index of test string where error character will be injected:");
                        while((user_input = getchar()) == '\n');
                        user_input_dec = user_input - ascii_to_dec;

                        if(!is_error_index_valid(user_input_dec))
                        {
                                printf("Invalid error index selected! Valid options are [0-%d]. You have selected ASCII value: %d \n", CUSTOM_MSG_LENGTH-1, user_input); 
                                continue;
                        }
                        error_index  = user_input_dec;

                        printf("Select ASCII character for error injection:");
                        while((user_input = getchar()) == '\n');

                        if(!is_error_value_valid(user_input))
                        {
                                printf("Invalid character selected! You have selected: %c \n",user_input); 
                                continue;
                        }
                        error_index  = user_input_dec;


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

bool is_error_index_valid(uint8_t input)
{
        bool            valid = false;
        
        if(input < CUSTOM_MSG_LENGTH){
                valid = true;
        }
        
        return valid;
}

bool is_error_value_valid(uint8_t input)
{
        bool            valid = false;
        
        if((input < 127) && (input > 31)){
                valid = true;
        }
        
        return valid;
}