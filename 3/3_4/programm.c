#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void catcher(int sig)
{
	switch(sig)
	{
		case SIGINT:
		printf("\nSIGINT\n");
		break;
		case SIGQUIT:
		printf("\nSIGQUIT\n");
		break;
	}
	
	exit(EXIT_SUCCESS);
}

int main()
{
	int schetchik = 0;

	signal(SIGINT, catcher);
	signal(SIGQUIT, catcher);

	while(1)
	{
		FILE *fop = fopen("data.txt", "a");

		if(fop == NULL)
		{
			printf("ERROR\n");
			exit(1);
		}
		fprintf(fop, "%d\n", schetchik);
		fclose(fop);
		sleep(1);
		schetchik++;
	}
}
