#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
	int schetchik = 0;



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
