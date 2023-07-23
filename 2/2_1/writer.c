#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#define _MAX_ 30

int main(int argc, char *argv[])
{
	int file = open(argv[1], O_WRONLY | O_CREAT , S_IRWXU | S_IRWXG);

	if(file == -1)
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
		write(file, text, strlen(text));
		close(argv[1]);
	}

	exit(EXIT_SUCCESS);
}

