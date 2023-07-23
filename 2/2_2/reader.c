#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>

int main(int argc, char *argv[])
{
	FILE *file = fopen(argv[1], "r");

	if(file == NULL)
	{
		switch(errno)
		{
			case EPERM:
				printf("Operation not permitted\n");
				break;
			case ENOENT:
				printf("No such file or directory\n");
				break;
			case ENOMEM:
				printf("Out of memory\n");
				break;
		}
		exit(EXIT_FAILURE);
	}
	else
	{
		fseek(file, 0, SEEK_END);
		long size = ftell(file);
		rewind(file);
		char *buf = (char*)malloc(sizeof(char)*size);
		fread(buf, 1, size, file);
		puts(buf);
		fclose(file);
		free(buf);
	}

	exit(EXIT_SUCCESS);
}
