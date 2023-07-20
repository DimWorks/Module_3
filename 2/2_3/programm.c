#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#define _MAX_ 15

char *read_dir(char *path)
{
	DIR *dir;
	struct dirent *ent;
	if((dir = opendir(path)) != NULL)
	{
		while((ent = readdir(dir)) != NULL)
		{
			printf("%s\n", ent->d_name);
		}
		closedir(dir);
	}
	else
	{
		printf("ERROR\n");
		exit(EXIT_SUCCESS);
	}
}

int main()
{
	printf("Enter the path: ");
	char tmp[_MAX_];
	char *path = tmp;
	gets(path);
	read_dir(path);
	return 0;
}
