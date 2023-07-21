#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
//#include <sys/wait.h>


int main(int argc, char* argv[])
{
	int midPoint = argc / 2;
	pid_t pid = -2;
	switch(pid = fork())
	{
		case -1:
			perror("fork");
			exit(EXIT_FAILURE);
			break;
		case 0:
			for(int i = midPoint; i < argc; i++)
			{
				int num = strtol(argv[i], NULL, 10);
				printf("I am daughter: num(%d) => sqr(%d)\n", num, num*num); 
	
			}
			exit(EXIT_SUCCESS);
			break;
		default:
			for(int i = 1; i < midPoint; i++)
			{
				int num = strtol(argv[i], NULL, 10);
				printf("I am parent: num(%d) => sqr(%d)\n", num, num*num); 
	
			}

	}

	exit(EXIT_SUCCESS);
}
