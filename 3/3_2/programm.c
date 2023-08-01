#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int main()
{
	int schetchik = 0;

	signal(SIGINT, SIG_IGN);

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
