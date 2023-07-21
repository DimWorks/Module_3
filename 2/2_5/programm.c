#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define _MAX_ 15

char *who_is_it(const char *path)
{
	//printf("WHAT %s\n", path);
	struct stat inf;
	if(lstat(path, &inf) == -1)
	{
		//exit(EXIT_FAILURE);
		return "ERROR";
	}

	if((inf.st_mode & S_IFMT) == S_IFDIR)
		//printf("DIR: %s\n", path);
		return "DIR";
	else if((inf.st_mode & S_IFMT) == S_IFLNK)
	{
		return "LINK";
	} 
	else if((inf.st_mode & S_IFMT) == S_IFREG)
	{
		return "FILE";
	}
	else if((inf.st_mode & S_IFMT) == S_IFCHR)
	{
		return "Character Device";
	}
	else if((inf.st_mode & S_IFMT) == S_IFBLK)
	{
		return "Block Device";
	}
	else if((inf.st_mode & S_IFMT) == S_IFSOCK)
	{
		return "SOCKET";
	}
	else if((inf.st_mode & S_IFMT) == S_IFIFO)
	{
		return "FIFO";
	}
	else
	{
		return "ERROR";
	}
}

void read_dir(const char *path)
{
	DIR *dir;
	struct dirent *ent;
	//char tmp[_MAX_] = path;
	//char *name = tmp;
	if((dir = opendir(path)) != NULL)
	{
		while((ent = readdir(dir)) != NULL)
		{
			char tmp[_MAX_];
		        strcpy(tmp,path);
			/*for(int i = 0; i < _MAX_; i++)
			{
					printf("%c
			}*/
			//printf("tmp: %s\n", tmp);
			printf("%s - %s\n", who_is_it(strcat(tmp, ent->d_name)), ent->d_name);
			//printf("%s\n", ent->d_name);
		}
		closedir(dir);
	}
	else
	{
		printf("ERROR\n");
	}
}

int main()
{
	printf("Enter the path: ");
	char tmp[_MAX_];
	char *path = tmp;
	gets(path);
	read_dir(path);
	printf("%s\n", who_is_it(path));
	return 0;
}
