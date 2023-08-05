#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *_buf;
};

int AWAIT = 0;

void isBusy(int sig)
{
	switch(sig)
	{
		case SIGUSR1:
		AWAIT = 1;
		break;

		case SIGUSR2:
		AWAIT = 0;
		break;

		default:
		printf("ERROR\n");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		printf("ERROR: invalid arguments\n");
		exit(EXIT_FAILURE);
	}
	
	key_t key = ftok(".", '#');
    	int sid = semget(key, 1, 0666 | IPC_CREAT);
    	struct sembuf lock = {0, -1, 0};
    	struct sembuf unlock[2] = {{0, 0, 0}, {0, 1, 0}};
    	union semun ar;
    	ar.val = 1;
    	
    	if (semctl(sid, 0, SETVAL, ar) == -1) 
    	{
        perror("Semaphore create");
        exit(EXIT_FAILURE);
    	}
	
	signal(SIGUSR1, isBusy);
    	signal(SIGUSR2, isBusy);

	int pipefd[2];
	pid_t pid;
	
	if(pipe(pipefd) == -1)		//Создаём канал
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	int cpid = fork();
	
	switch(cpid)
	{
		case -1:
			perror("fork");
			exit(EXIT_FAILURE);
			break;
		case 0:
			while(AWAIT);
			close(pipefd[0]);	//Закрываем неиспользуемый канал чтения
			srand(time(NULL));
			for (int i = 0; i < strtol(argv[1], NULL, 10); i++)
			{
				int num = rand() % 1000;
				write(pipefd[1], &num, sizeof(int));

			}
			close(pipefd[1]);
			break;
		default:
			FILE *file = fopen("data", "a");
			for (int i = 0; i < strtol(argv[1], NULL, 10); i++)
			{
				int output;
				read(pipefd[0], &output, sizeof(int));
				printf("%d\n", output);
				fprintf(file, "%d\n", output);
				
				if (semop(sid, unlock, 2) == -1) // unlock file
				{ 
                    		perror("Unlock file");
                    		exit(EXIT_FAILURE);
                		}
			}
			fclose(file);
			break;
	}
	exit(EXIT_SUCCESS);
}
