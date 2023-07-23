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

	int pipefdf[2], pipefds[2];
	pid_t pid;
	
	if(pipe(pipefdf) == -1)		//Создаём канал
	{
		perror("pipe first");
		exit(EXIT_FAILURE);
	}
	
	if(pipe(pipefds) == -1)		//Создаём канал
	{
		perror("pipe second");
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
			//close(pipefds[0]);	//Закрываем неиспользуемый канал чтения
			srand(time(NULL));
			for (int i = 0; i < strtol(argv[1], NULL, 10); i++)
			{
				int num = rand() % 1000;
				write(pipefdf[1], &num, sizeof(int));
				int answer;
				read(pipefds[0], &answer,sizeof(int));
				printf("say chaild from parent: %d\n", answer);
			}
			close(pipefdf[1]);
			close(pipefds[0]);
			break;
		default:
			FILE *file = fopen("data", "a");
			for (int i = 0; i < strtol(argv[1], NULL, 10); i++)
			{
				int output;
				read(pipefdf[0], &output, sizeof(int));
				printf("say parent from child: %d\n", output);
				fprintf(file, "%d\n", output);
				int answer = output * 2;
				write(pipefds[1], &answer, sizeof(int));
			}
			fclose(file);
			close(pipefdf[0]);
			close(pipefds[1]);
			break;
	}
	exit(EXIT_SUCCESS);
}
