#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void three()
{
	static int i = 0;
	i++;
	if(i == 3)
	{
		exit(EXIT_SUCCESS);
	}
}

int main()
{
	int schetchik = 0;

	signal(SIGINT, three);

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
