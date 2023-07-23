#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

#define _MAX_ 30

int main(int argc, char *argv[])
{
	int file = open(argv[1], O_RDONLY);

	if(file == -1)
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
		char buf[_MAX_];

		read(file, buf, _MAX_);
		printf("Text from file: %s\n", buf);
		close(file);
	}

	exit(EXIT_SUCCESS);
}

