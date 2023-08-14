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
    
    int shm = shm_open("/shm", O_CREAT | O_RDWR, 0666);
    if (shm == -1) {
        perror("Shared memory open");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(shm, sizeof(int) * (LENGTH + 3)) == -1) {
        perror("Shared memory truncate");
        exit(EXIT_FAILURE); 
    }

    pid_t pid = fork();
    switch (pid) {
        case -1:
            perror("Fork");
            exit(EXIT_FAILURE);
        case 0: {
            sleep(1);
            while(1) {
                char* addr = mmap(0, sizeof(int) * (LENGTH + 3), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);

                if (sem_wait(sid_num) == -1) {
                    perror("Lock num");
                    exit(EXIT_FAILURE);
                }
                
                printf("C: ");
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
                
                memcpy(addr + (len+1)*sizeof(min), &min, sizeof(min));
                memcpy(addr + (len+2)*sizeof(max), &max, sizeof(max));

                //lock shm for parent read min|max
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
            while(1) {
                char* addr = mmap(0, sizeof(int) * (LENGTH + 3), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);

                if (addr == (char*)-1) {
                    perror("Mmap POSIX");
                    exit(EXIT_FAILURE);
                }
                
                int len = rand() % LENGTH + 1;
                memcpy(addr, &len, sizeof(len));
                for (int i = 1; i < len +1; i++) {
                    int num = rand() % MAX_NUM;
                    
                    memcpy(addr + i*sizeof(num), &num, sizeof(num));
                }

                if (sem_post(sid_num) == -1) {
                    perror("Unlock num");
                    exit(EXIT_FAILURE);
                }

                if (sem_wait(sid_min) == -1) {
                    perror("Lock min");
                    exit(EXIT_FAILURE);
                }

                int min, max;
                memcpy(&min, addr + (len+1)*sizeof(min), sizeof(min));
                memcpy(&max, addr + (len+2)*sizeof(max), sizeof(max));
                printf("P: min = %d, max = %d\n\n", min, max);

                count++;
                sleep(2);
            }
            exit(EXIT_SUCCESS);
        }
    }
}
