#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	printf("argc: %d\n", argc);
	for (int i = 0; i < 3; i++)
	{
		printf("%s", argv[i]);
	}
	
	exit(EXIT_SUCCESS);
}
