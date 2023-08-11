#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define LENGTH 10
#define _MAX_ 25

int count = 0;

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *_buf;
};

void sigAct(int sig) {
    printf("\nCount = %d\n", count);
    exit(EXIT_SUCCESS);
}

int main() {
    srand(time(NULL));

    signal(SIGINT, sigAct);

    int sid = semget(IPC_PRIVATE, 2, IPC_CREAT | 0666);
    struct sembuf num_lock = {0, -1, 0};
    struct sembuf num_unlock[2] = {{0, 0, 0}, {0, 1, 0}};
    struct sembuf min_lock = {1, -1, 0};
    struct sembuf min_unlock[2] = {{1, 0, 0}, {1, 1, 0}};
    union semun ar;
    ar.val = 0;
    if (semctl(sid, 0, SETVAL, ar) == -1) 
    {
        perror("Semaphore for nums create");
        exit(EXIT_FAILURE);
    }

    if (semctl(sid, 1, SETVAL, ar) == -1) 
    {
        perror("Semaphore for min|max create");
        exit(EXIT_FAILURE);
    }

    //shared memory
    int shm = shmget(IPC_PRIVATE, sizeof(int) * (LENGTH + 3), IPC_CREAT | 0666);
    if (shm == -1) 
    {
        perror("Create shared memory");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    switch (pid) {
        case -1:
            perror("fork()");
            exit(EXIT_FAILURE);
        case 0: {
            sleep(1);
            while(1) {
                char* addr = shmat(shm, 0, 0);

                //lock shm for parent until child read nums
                if (semop(sid, &num_lock, 1) == -1) {
                    perror("Lock file");
                    exit(EXIT_FAILURE);
                }

                printf("NUM: ");
                int len, min = INT_MAX, max = INT_MIN;
                memcpy(&len, addr, sizeof(len));

                for (int i = 1; i < len + 1; i++) {
                    int num;
                    memcpy(&num, addr + i*sizeof(num), sizeof(num));
                    if (num < min) min = num;
                    if (num > max) max = num;
                    printf("%d ", num);
                }
                puts("");
                //printf("\nC: min = %d, max = %d\n", min, max);
                memcpy(addr + (len+1)*sizeof(min), &min, sizeof(min));
                memcpy(addr + (len+2)*sizeof(max), &max, sizeof(max));

                //lock shm for parent read min|max
                if (semop(sid, min_unlock, 2) == -1) {
                    perror("Unlock file");
                    exit(EXIT_FAILURE);
                }

                count++;
                sleep(2);
            }
            exit(EXIT_SUCCESS);
        }
        default: {
            while(1) {
                char* addr = shmat(shm, 0, 0);

                //printf("P: ");
                int len = rand() % LENGTH + 1;
                memcpy(addr, &len, sizeof(len));
                for (int i = 1; i < len +1; i++) {
                    int num = rand() % _MAX_;
                    //printf("%d ", num);
                    memcpy(addr + i*sizeof(num), &num, sizeof(num));
                }

                //unlock shm for child read nums
                if (semop(sid, num_unlock, 2) == -1) {
                    perror("Unlock file");
                    exit(EXIT_FAILURE);
                }

                //lock shm for child until parent read min|max
                if (semop(sid, &min_lock, 1) == -1) {
                    perror("Lock file");
                    exit(EXIT_FAILURE);
                }

                int min, max;
                memcpy(&min, addr + (len+1)*sizeof(min), sizeof(min));
                memcpy(&max, addr + (len+2)*sizeof(max), sizeof(max));
                printf("> min = %d, max = %d\n\n", min, max);

                count++;
                sleep(2);
            }
            exit(EXIT_SUCCESS);
        }
    }
}