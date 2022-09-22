#include "execute_regime.h"
#include "common.h"

const char* test_input1 = "BORNA123";
const char* test_input2 = "IVAN690";
const char* test_input3 = "DMIJIC456";
const char* test_input4 = "MA773OB";
const char* test_input5 = "FILE70";

const char* test_output1 = "-... --- .-. -. .- .---- ..--- ...--";
const char* test_output2 = ".. ...- .- -. -.... ----. -----";
const char* test_output3 = "-.. -- .. .--- .. -.-. ....- ..... -....";
const char* test_output4 = "-- .- --... --... ...-- --- -...";
const char* test_output5 = "..-. .. .-.. . --... -----";

sem_t semFinishSignal;
sem_t semStart;

test_pairs_t test_vectors[CUSTOM_MSG_COUNT];

pthread_mutex_t mutex;
pthread_mutex_t program_mutex;
enum mode       program_mode;
uint8_t         error_index; //0-n
uint8_t         error_value; // ASCII char
uint8_t         led; // green, red
uint16_t        unit_duration; //ms

static char* rand_string(void)
{
        const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        uint8_t size;

        do {
                size = rand() % 51;
        }while(size<=0);

        char* str = malloc(size + 1);

        for (uint8_t n = 0; n < size; ++n) {
                uint8_t key = rand() % (uint8_t) (sizeof charset - 1);
                str[n] = charset[key];
        }

        str[size] = '\0';

        return str;
}

void test_setup(void)
{
        const test_pairs_t test_pair1 = {
                .test_input = test_input1,
                .test_output = test_output1
        };

        const test_pairs_t test_pair2 = {
                .test_input = test_input2,
                .test_output = test_output2
        };

        const test_pairs_t test_pair3 = {
                .test_input = test_input3,
                .test_output = test_output3
        };

        const test_pairs_t test_pair4 = {
                .test_input = test_input4,
                .test_output = test_output4
        };

        const test_pairs_t test_pair5 = {
                .test_input = test_input5,
                .test_output = test_output5
        };

        test_vectors[0] = test_pair1;
        test_vectors[1] = test_pair2;
        test_vectors[2] = test_pair3;
        test_vectors[3] = test_pair4;
        test_vectors[4] = test_pair5;

        return;
}

void normal_regime(const char *path)
{
        char *random_string;
        uint8_t res;
        uint64_t fd;

        while (1) {
                random_string = rand_string();
                PRINT_DEBUG("%s\n", random_string);

                pthread_mutex_lock(&mutex);
                fd = open(path, O_WRONLY);
                res = write(fd, random_string, strlen(random_string));
                if (-1 == res) {
                        printf("Write failed!\n");
                }
                pthread_mutex_unlock(&mutex);
                PRINT_DEBUG("Written %s to %s\n", random_string, path);

                usleep(2000000); //2s sleep
        }

        return;
}

void test_regime(const uint8_t msg_option, const char* path)
{
        uint8_t res;
        uint64_t fd;
        char *encoded_string;

        pthread_mutex_lock(&mutex);
        fd = open(path, O_WRONLY);
        res = write(fd, test_vectors[msg_option - 1].test_input, strlen(test_vectors[msg_option - 1].test_input));
        if (-1 == res) {
                printf("Write failed!\n");
        }
        pthread_mutex_unlock(&mutex);
        PRINT_DEBUG("Written %s to %s\n", test_vectors[msg_option - 1].test_input, path);


        pthread_mutex_lock(&mutex);
        res = read(fd, encoded_string, 250); //250 should be max number because max input string is 50 and max morse chars per letter is 5
        if (-1 == res) {
                printf("Read failed!\n");
        }
        pthread_mutex_unlock(&mutex);

        if (!strcmp(test_vectors[msg_option - 1].test_input, encoded_string)){
                printf("Encoder test succesfull!\n");
        }

        return;
}
