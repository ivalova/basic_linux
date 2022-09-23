#include "execute_regime.h"
#include "timer_event.h"
#include "device_com.h"

static const char* test_input1 = "BORNA12345";
static const char* test_input2 = "IVAN690LOV";
static const char* test_input3 = "D MIJIC456";
static const char* test_input4 = "MA773OBRIS";
static const char* test_input5 = "FILE70GARM";

static const char* test_output1 = "-...*---*.-.*-.*.-*.----*..---*...--*....-*.....";
static const char* test_output2 = "..*...-*.-*-.*-....*----.*-----*.-..*---*...-";
static const char* test_output3 = "-.._--*..*.---*..*-.-.*....-*.....*-....";
static const char* test_output4 = "--*.-*--...*--...*...--*---*-...*.-.*..*...";
static const char* test_output5 = "..-.*..*.-..*.*--...*-----*--.*.-*.-.*--";

static struct test_pairs test_vectors[CUSTOM_MSG_COUNT];
static timer_event_t hNormalRegimeTimer;

static char* rand_string(void)
{
        const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ";
        uint8_t size;
        char* str;

        do {
                size = rand() % 51;
        }while(size<=0);

        str = malloc(size + 1);

        for (uint8_t n = 0; n < size; ++n) {
                uint8_t key = rand() % (uint8_t) (sizeof charset - 1);
                str[n] = charset[key];
        }

        str[size] = '\0';

        return str;
}

static void test_setup(void)
{
        const struct test_pairs test_pair1 = {
                .test_input = test_input1,
                .test_output = test_output1
        };

        const struct test_pairs test_pair2 = {
                .test_input = test_input2,
                .test_output = test_output2
        };

        const struct test_pairs test_pair3 = {
                .test_input = test_input3,
                .test_output = test_output3
        };

        const struct test_pairs test_pair4 = {
                .test_input = test_input4,
                .test_output = test_output4
        };

        const struct test_pairs test_pair5 = {
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

static void* normal_regime(void* args)
{
        char *random_string;
        char *done;
        uint8_t res;
        uint64_t fd;

        fd = open(device_path, O_WRONLY);
        if (fd<0) {
                printf("Opening failed!\n");
        }

        random_string = rand_string();
        PRINT_DEBUG("%s\n", random_string);

        res = write(fd, random_string, strlen(random_string));
        if (-1 == res) {
                printf("Write failed!\n");
        }

        PRINT_DEBUG("Written %s to %s\n", random_string, device_path);

        do{
                if (sem_trywait(&semTerminate) == 0) {
                        printf("Terminating normal regime!\n");
                        sem_post(&semGetInput);
                        goto terminate;
                }
                read(fd, done, 1);
        }while(strcmp(done, "1"));

        timer_event_set(&hNormalRegimeTimer, 
        MSG_SEQ_TIMEOUT_MS, normal_regime, 
        0, TE_KIND_ONCE);

terminate:
        free(random_string);
        close(fd);

        return 0;
}

static void test_regime(void)
{
        uint8_t res;
        uint64_t fd;
        char *encoded_string;

        fd = open(device_path, O_WRONLY);
        if (fd<0) {
                printf("Opening failed!\n");
        }

        res = write(fd, test_vectors[msg_option].test_input, 
                strlen(test_vectors[msg_option].test_input));

        if (-1 == res) {
                printf("Write failed!\n");
        }

        PRINT_DEBUG("Written %s to %s\n", 
                test_vectors[msg_option].test_input, 
                device_path);

        encoded_string = malloc(1001);
        res = read(fd, encoded_string, 1000);
        if (-1 == res) {
                printf("Read failed!\n");
        }

        if (!strcmp(test_vectors[msg_option].test_output, encoded_string)){
                printf("Encoder test succesfull!\nExpected:\n%s\nGot:%s\n",
                        test_vectors[msg_option].test_output, 
                        encoded_string);
        }
        else {
                printf("Encoder test unsuccesfull!\nExpected:\n%s\nGot:%s\n",
                        test_vectors[msg_option].test_output, 
                        encoded_string);
        }

        free(encoded_string);
        close(fd);
        return;
}

void* execute_regime(void* args)
{
        test_setup();

        while (1) {
                if (sem_trywait(&semFinishSignal) == 0) {
                    break;
                }

                if (sem_trywait(&semStart) == 0) {
                        switch (program_mode) {
                                case MODE_NORMAL:
                                        PRINT_DEBUG("Entering normal regime\n");
                                        sem_post(&semGetInput);
                                        normal_regime(NULL);
                                        break;
                                case MODE_CUSTOM_MSG:
                                        PRINT_DEBUG("Entering test regime\n");
                                        test_regime();
                                        sem_post(&semGetInput);
                                        break;
                                case MODE_CUSTOM_MSG_ERR:
                                        PRINT_DEBUG("Entering error regime\n");
                                        ioctl_set_mode_to_test_error(); // TODO: handle error return value
                                        test_regime();
                                        ioctl_set_mode_to_normal(); // TODO: handle error return value
                                        sem_post(&semGetInput);
                                        break;
                                case MODE_STOP_SENDING:
                                        PRINT_DEBUG("Currently in idle regime\n");
                                        sem_post(&semGetInput);
                                        break;
                                default:
                                        break;
                        }
                }
        }

        return 0;
}
