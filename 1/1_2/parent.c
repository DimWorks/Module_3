#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

void call_daugther()
{
	pid_t pid = -2;
	switch(pid = fork())
	{
		perror("fork");
		exit(EXIT_FAILURE);
		break;
		case 0:
		printf("PID: %d\n", getpid());
		execl("/home/dmitriy/Module_3/1/1_2/daugther.out", "Hellow", ", ", "world",  NULL);
		break;
		default:
		break;
	}
}

void Bye()
{
	printf("It`s OK!\n");
}

int main()
{
	call_daugther();
	atexit(Bye);
	exit(EXIT_SUCCESS);
}
