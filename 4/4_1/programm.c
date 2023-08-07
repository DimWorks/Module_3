#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <pthread.h>

#define THREADS_COUNT 2

pthread_t tid[THREADS_COUNT];
void* status[THREADS_COUNT];


union semun {
	int val; 
	struct semid_ds *buf; 
	unsigned short *array; 
	struct seminfo *__buf; 
};

int semid;	
union semun arg;
struct sembuf lock_res = {0, -1, 0};
struct sembuf rel_res = {0, 1, 0};

char* fifoChannelName = "/tmp/fifo0001.1";

void* sendToChat(void* arg) {
	int fd;
	char buf[80];
	
	if ((fd = open(fifoChannelName, O_WRONLY)) == -1){
		perror("open()");
		exit(EXIT_FAILURE);
	}	
		
	printf("\n> ");
	fgets(buf, 80, stdin);
				
	semop(semid, &lock_res, 1);		
	printf("write semaphore locked\n");	
				
	write(fd, buf, strlen(buf)+1);	
	
	semop(semid, &rel_res, 1);	
	printf("write semaphore unlocked\n");
	
	close(fd);

	return (void *) 111;

}

void* readFromChat(void* arg) {
	int fd;
	char buf[80];
	
	
	if ((fd = open(fifoChannelName, O_RDONLY)) == -1){
		perror("zhopka");
		exit(EXIT_FAILURE);
	}	

	while( (long)status[0] != 111);			
	semop(semid, &lock_res, 1);		
	printf("read semaphore locked\n");	
			
	read(fd, buf, 80);	
	
	semop(semid, &rel_res, 1);	
	printf("read semaphore unlocked\n");
	
	printf("FIFO: %s\n", buf);
	close(fd);

    pthread_exit(0);
}


int main() {

	mkfifo(fifoChannelName, 0666);
	
	key_t key;		
	key = ftok("/etc/fstab", getpid());

	semid = semget(key, 1, 0666 | IPC_CREAT);
	arg.val = 1;	
	semctl(semid, 0, SETVAL, arg);
	
	while(1){
	
		pthread_create(&tid[0], NULL, sendToChat, NULL);
		pthread_create(&tid[1], NULL, readFromChat, NULL);

		pthread_join(tid[0], &status[0]);
		pthread_join(tid[1], &status[1]);
		
		status[0] = 0;
	}
		
	semctl(semid, 0, IPC_RMID);

    exit(EXIT_SUCCESS);
}
