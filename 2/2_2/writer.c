#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#define _MAX_ 30

int main(int argc, char *argv[])
{
	FILE *file = fopen(argv[1], "a");

	if(file == NULL)
	{
		perror("Can`t open file");
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("Enter the text: ");

		char text_arr[_MAX_];
		char *text = text_arr;
		gets(text);
		printf("Your text: %s\n", text);
		fprintf(file, "%s", text);
		fclose(file);
	}

	exit(EXIT_SUCCESS);
}

