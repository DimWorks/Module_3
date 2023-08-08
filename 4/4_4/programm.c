#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <pthread.h>

#define THREADS_COUNT 2

pthread_t tid[THREADS_COUNT];
void* status[THREADS_COUNT];

sem_t semaphore;

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
				
	sem_wait(&semaphore);	
	printf("write semaphore locked\n");	
				
	write(fd, buf, strlen(buf)+1);	
	
	sem_post(&semaphore);
	printf("write semaphore unlocked\n");
	
	close(fd);

	return (void *) 111;

}

void* readFromChat(void* arg) {
	int fd;
	char buf[80];
	
	
	if ((fd = open(fifoChannelName, O_RDONLY)) == -1){
		perror("open()");
		exit(EXIT_FAILURE);
	}	

	while( (long)status[0] != 111);			
	sem_wait(&semaphore);	
	printf("read semaphore locked\n");	
			
	read(fd, buf, 80);	
	
	sem_post(&semaphore);
	printf("read semaphore unlocked\n");
	
	printf("READ FROM FIFO: %s\n", buf);
	close(fd);

    pthread_exit(0);
}


int main() {

	mkfifo(fifoChannelName, 0666);
	
	sem_init(&semaphore, 0, 1);
	
	while(1){
	
		pthread_create(&tid[0], NULL, sendToChat, NULL);
		pthread_create(&tid[1], NULL, readFromChat, NULL);

		pthread_join(tid[0], &status[0]);
		pthread_join(tid[1], &status[1]);
		
		status[0] = 0;
	}
		
	sem_destroy(&semaphore);

    exit(EXIT_SUCCESS);
}
