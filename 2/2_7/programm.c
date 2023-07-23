#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		printf("ERROR: invalid arguments\n");
		exit(EXIT_FAILURE);
	}

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
			}
			fclose(file);
			break;
	}
	exit(EXIT_SUCCESS);
}
