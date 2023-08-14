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
#define MAX_NUM 20

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

    int sid = semget(IPC_PRIVATE, 4, IPC_CREAT | 0666);
    struct sembuf num_lock = {0, -1, 0},
                num_unlock[3] = {{0, 1, 0}, {0, 1, 0}, {0, 1, 0}},
                min_lock = {1, -1, 0},
                min_unlock[2] = {{1, 0, 0}, {1, 1, 0}},
                max_lock = {2, -1, 0},
                max_unlock[2] = {{2, 0, 0}, {2, 1, 0}},
                aver_lock = {3, -1, 0},
                aver_unlock[2] = {{3, 0, 0}, {3, 1, 0}};
    union semun arg;
    arg.val = 0;
    if (semctl(sid, 0, SETVAL, arg) == -1) {
        perror("Semaphore for nums create");
        exit(EXIT_FAILURE);
    }
    if (semctl(sid, 1, SETVAL, arg) == -1) {
        perror("Semaphore for min create");
        exit(EXIT_FAILURE);
    }
    if (semctl(sid, 2, SETVAL, arg) == -1) {
        perror("Semaphore for max create");
        exit(EXIT_FAILURE);
    }
    if (semctl(sid, 3, SETVAL, arg) == -1) {
        perror("Semaphore for average create");
        exit(EXIT_FAILURE);
    }

    //shared memory
    int shm = shmget(IPC_PRIVATE, sizeof(int) * (LENGTH + 3) + sizeof(float), IPC_CREAT | 0666);
    if (shm == -1) {
        perror("Create shared memory");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    switch (pid) {
        case -1:
            perror("Fork min");
            exit(EXIT_FAILURE);
        case 0: {
            sleep(1);
            while(1) {
                char* addr = shmat(shm, 0, 0);

                if (semop(sid, &num_lock, 1) == -1) {
                    perror("Lock nums");
                    exit(EXIT_FAILURE);
                }

                printf("C_min: ");
                int len, min = INT_MAX;
                memcpy(&len, addr, sizeof(len));

                for (int i = 1; i < len + 1; i++) {
                    int num;
                    memcpy(&num, addr + i*sizeof(num), sizeof(num));
                    if (num < min) min = num;
                    printf("%d ", num);
                }
                puts("");
                memcpy(addr + (len+1)*sizeof(min), &min, sizeof(min));

                if (semop(sid, min_unlock, 2) == -1) {
                    perror("Unlock min");
                    exit(EXIT_FAILURE);
                }

                count++;
                sleep(2);
            }
            exit(EXIT_SUCCESS);
        }
        default: {
            pid = fork();
            switch (pid) {
                case -1:
                    perror("Fork max");
                    exit(EXIT_FAILURE);
                case 0: { // max
                    sleep(1);
                    while(1) {
                        char* addr = shmat(shm, 0, 0);

                        //lock shm for parent until child read nums
                        if (semop(sid, &num_lock, 1) == -1) {
                            perror("Lock nums");
                            exit(EXIT_FAILURE);
                        }

                        printf("C_max: ");
                        int len, max = INT_MIN;
                        memcpy(&len, addr, sizeof(len));

                        for (int i = 1; i < len + 1; i++) {
                            int num;
                            memcpy(&num, addr + i*sizeof(num), sizeof(num));
                            if (num > max) max = num;
                            printf("%d ", num);
                        }
                        puts("");
                        memcpy(addr + (len+2)*sizeof(max), &max, sizeof(max));

                        //lock shm for parent read max
                        if (semop(sid, max_unlock, 2) == -1) {
                            perror("Unlock max");
                            exit(EXIT_FAILURE);
                        }

                        count++;
                        sleep(2);
                    }
                    exit(EXIT_SUCCESS);
                }
                default: {
                    pid = fork();
                    switch (pid) {
                        case -1:
                            perror("Fork max");
                            exit(EXIT_FAILURE);
                        case 0: { // average
                            sleep(1);
                            while(1) {
                                char* addr = shmat(shm, 0, 0);

                                //lock shm for parent until child read nums
                                if (semop(sid, &num_lock, 1) == -1) {
                                    perror("Lock nums");
                                    exit(EXIT_FAILURE);
                                }

                                printf("C_ave: ");
                                int len;
                                float average = 0;
                                memcpy(&len, addr, sizeof(len));

                                for (int i = 1; i < len + 1; i++) {
                                    int num;
                                    memcpy(&num, addr + i*sizeof(num), sizeof(num));
                                    average += num;
                                    printf("%d ", num);
                                }
                                average /= (float) len;
                                puts("");
                                memcpy(addr + (len+3)*sizeof(int), &average, sizeof(average));

                                //lock shm for parent read average
                                if (semop(sid, aver_unlock, 2) == -1) {
                                    perror("Unlock average");
                                    exit(EXIT_FAILURE);
                                }

                                count++;
                                sleep(2);
                            }
                            exit(EXIT_SUCCESS);
                        }
                        default: { // parent
                            while(1) {
                                char* addr = shmat(shm, 0, 0);

                                int len = rand() % LENGTH + 1;
                                memcpy(addr, &len, sizeof(len));
                                for (int i = 1; i < len +1; i++) {
                                    int num = rand() % MAX_NUM;
                                    memcpy(addr + i*sizeof(num), &num, sizeof(num));
                                }

                                //unlock shm for child read nums
                                if (semop(sid, num_unlock, 3) == -1) {
                                    perror("Unlock nums");
                                    exit(EXIT_FAILURE);
                                }

                                int min, max;
                                float average;
                                //lock shm for child until parent read min
                                if (semop(sid, &min_lock, 1) == -1) {
                                    perror("Lock min");
                                    exit(EXIT_FAILURE);
                                }
                                memcpy(&min, addr + (len+1)*sizeof(min), sizeof(min));
                                //lock shm for child until parent read max
                                if (semop(sid, &max_lock, 1) == -1) {
                                    perror("Lock max");
                                    exit(EXIT_FAILURE);
                                }
                                memcpy(&max, addr + (len+2)*sizeof(max), sizeof(max));
                                //lock shm for child until parent read average
                                if (semop(sid, &aver_lock, 1) == -1) {
                                    perror("Lock average");
                                    exit(EXIT_FAILURE);
                                }
                                memcpy(&average, addr + (len+3)*sizeof(max), sizeof(average));
                                printf("P: min = %d, max = %d, average = %0.2f\n\n", min, max, average);

                                count++;
                                sleep(2);
                            }
                            exit(EXIT_SUCCESS);
                        }
                    }
                }
            }
        }
    }
}
