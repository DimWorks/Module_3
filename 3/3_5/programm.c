#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int main()
{
	int schetchik = 0;
	
	while(1)
	{
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
		FILE *fop = fopen("data.txt", "a");

		if(fop == NULL)
		{
			printf("ERROR\n");
			exit(1);
		}
		fprintf(fop, "%d\n", schetchik);
		fclose(fop);
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		sleep(1);
		schetchik++;
	}
}
