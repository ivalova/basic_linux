#include "execute_regime.h"

pthread_mutex_t lock;
static sem_t semStartTh2;
static sem_t semEndTh2;
static sem_t semEnd;
int option;
FILE *fd;
char* path = "/dev/null";

static char *rand_string(void)
{
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int size;

    do{
        size = rand() % 51;
    }while(size<=0);

    char* str = malloc(size + 1);

    for (size_t n = 0; n < size; ++n) {
        int key = rand() % (int) (sizeof charset - 1);
        str[n] = charset[key];
    }

    str[size] = '\0';

    return str;
}

int normal_regime()
{
    char *random_string;

    // Generate random string up to 50 chars
    random_string = rand_string();
    printf("%s\n", random_string);

    pthread_mutex_lock(&lock);
    fd=fopen(path,"w+");
    fputs(random_string, fd);
    printf("Written %s to %s\n", random_string, path);
    fclose(fd);
    pthread_mutex_unlock(&lock); 

    return 0;
}

// void* perform_work (void *pParam)
// {
//     if (sem_trywait(&semEnd) == 0)
//     {
//         break;
//     }
//     if (sem_trywait(&semStartTh2) == 0)
//     {
//         switch(option)
//         {
//             case 1:
//                 normal_regime();
//                 break;
//             case 2:
//                 test_regime();
//                 break;
//             case 3:
//                 error_regime();
//                 break;
//             default:
//                 break;
//         }

//         sem_post(&semEndTh2);
//     }

//     return 0;
// }
