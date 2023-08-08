#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <stdlib.h>
#include <pthread.h>

#define THREADS_COUNT 2

pthread_t tid[THREADS_COUNT];
void* status[THREADS_COUNT];

sem_t semaphore;

FILE *fp;

int p[2];
long int number = 10;
char text[100];
char text2[100];        

void* sendToFD(void* arg) {  //child

	if((fp=fopen("data.txt","rb"))==NULL) {
		printf("Errors occurred.");
		exit(EXIT_FAILURE);
	}
	
	sem_wait(&semaphore);		
	printf("file read semaphore locked\n");	

	fread(text, sizeof(text), 1, fp);	
	
	sem_post(&semaphore);	
	printf("file read semaphore unlocked\n");
	
	fclose(fp);
	
	sscanf(text, "%ld", &number);
	for (int i = 0; i < 3; i++){
		number *= number % 13;
	}
	number++;
	
	sprintf(text, "%ld", number);
	
    write(p[1], text, strlen(text) + 1);

	return (void *) 111;

}

void* readFromFD(void* arg) 
{
	
	while( (long)status[0] != 111);	
	read(p[0], text2, sizeof(text2));

	if((fp=fopen("data.txt", "wb"))==NULL) {
		printf("Errors occurred.");
		exit(EXIT_FAILURE);
	}
	
	sem_wait(&semaphore);	
	printf("file write semaphore locked\n");
	 	
	fwrite(&text2, strlen(text2), 1, fp);
	
	sem_post(&semaphore);
	printf("file write semaphore unlocked\n");
	
	fclose (fp);
             
    printf("SAY: %s\n", text2);

    pthread_exit(0);
}


int main()
{

	if (pipe(p)) 
	{
        	perror("pipe() error");
        	exit(EXIT_FAILURE);
    	}
	
	key_t key;		
	key = ftok("/etc/fstab", getpid());

	sem_init(&semaphore, 0, 1);
	
	while(1)
	{
		pthread_create(&tid[0], NULL, sendToFD, NULL);
		pthread_create(&tid[1], NULL, readFromFD, NULL);

		pthread_join(tid[0], &status[0]);
		pthread_join(tid[1], &status[1]);
		
		status[0] = 0;
		
		sleep(3);
	}
		
	sem_destroy(&semaphore);

    return 0;
}
