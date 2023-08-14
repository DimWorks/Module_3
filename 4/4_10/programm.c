#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <string.h>

#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <semaphore.h>

#define LENGTH 10
#define MAX_NUM 20

int count = 0;

void sigAct(int sig) {
    printf("\nCount = %d\n", count);
    exit(EXIT_SUCCESS);
}

int main() {
    srand(time(NULL));
    
    signal(SIGINT, sigAct);

    sem_unlink("/sem1");
    sem_t* sid_num = sem_open("/sem1", O_CREAT, 0666, 0);
    if (sid_num == SEM_FAILED) {
        perror("Semaphore num create");
        exit(EXIT_FAILURE);
    }
    sem_unlink("/sem2");
    sem_t* sid_min = sem_open("/sem2", O_CREAT, 0666, 0);
    if (sid_min == SEM_FAILED) {
        perror("Semaphore min create");
        exit(EXIT_FAILURE);
    }
    sem_unlink("/sem3");
    sem_t* sid_max = sem_open("/sem3", O_CREAT, 0666, 0);
    if (sid_max == SEM_FAILED) {
        perror("Semaphore max create");
        exit(EXIT_FAILURE);
    }
    sem_unlink("/sem4");
    sem_t* sid_aver = sem_open("/sem4", O_CREAT, 0666, 0);
    if (sid_aver == SEM_FAILED) {
        perror("Semaphore average create");
        exit(EXIT_FAILURE);
    }    

    int shm = shm_open("/shm", O_CREAT | O_RDWR, 0666);
    if (shm == -1) {
        perror("Shared memory open");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(shm, sizeof(int) * (LENGTH + 3) + sizeof(float)) == -1) {
        perror("Shared memory truncate");
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
                char* addr = mmap(0, sizeof(int) * (LENGTH + 3), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);

                //lock shm for parent until child read nums
                if (sem_wait(sid_num) == -1) {
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

                //lock shm for parent read min
                if (sem_post(sid_min) == -1) {
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
                        char* addr = mmap(0, sizeof(int) * (LENGTH + 3), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);

                        //lock shm for parent until child read nums
                        if (sem_wait(sid_num) == -1) {
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
                        if (sem_post(sid_max) == -1) {
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
                                char* addr = mmap(0, sizeof(int) * (LENGTH + 3), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);

                                //lock shm for parent until child read nums
                                if (sem_wait(sid_num) == -1) {
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
                                if (sem_post(sid_aver) == -1) {
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
                                char* addr = mmap(0, sizeof(int) * (LENGTH + 3), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);

                                int len = rand() % LENGTH + 1;
                                memcpy(addr, &len, sizeof(len));
                                for (int i = 1; i < len +1; i++) {
                                    int num = rand() % MAX_NUM;
                                    memcpy(addr + i*sizeof(num), &num, sizeof(num));
                                }

                                //unlock shm for child read nums
                                sem_post(sid_num);
                                sem_post(sid_num);
                                sem_post(sid_num);


                                int min, max;
                                float average;
                                //lock shm for child until parent read min
                                if (sem_wait(sid_min) == -1) {
                                    perror("Lock min");
                                    exit(EXIT_FAILURE);
                                }
                                memcpy(&min, addr + (len+1)*sizeof(min), sizeof(min));
                                //lock shm for child until parent read max
                                if (sem_wait(sid_max) == -1) {
                                    perror("Lock max");
                                    exit(EXIT_FAILURE);
                                }
                                memcpy(&max, addr + (len+2)*sizeof(max), sizeof(max));
                                //lock shm for child until parent read average
                                if (sem_wait(sid_aver) == -1) {
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
