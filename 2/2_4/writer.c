#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#define _MAX_ 30

void writeToFile(char *name)
{
	FILE *file = fopen(name, "a");
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
}

int main(int argc, char *argv[])
{
	char *name = argv[1];
	if(!access(argv[1], 0))
	{
		writeToFile(name);
	}
	else
	{
		printf("No sauch file\n Create new(y/n): ");
		char answer = 'a';
		scanf("%c", &answer);
		if(answer == 'y')
		{
			int crf = creat(argv[1], S_IRUSR | S_IWUSR);
			if(crf == -1)
			{
				printf("I can`t to creat file\n");
				exit(EXIT_FAILURE);
			}
			else
			{
				writeToFile(name);

			}
		}
		else
		{
			printf("I am sorry. See you\n");
		}
	
	}

	exit(EXIT_SUCCESS);
}

