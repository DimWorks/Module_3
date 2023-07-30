#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define _MAX_ 15

#define AC_BLACK "\x1b[30m"
#define AC_RED "\x1b[31m"
#define AC_GREEN "\x1b[32m"
#define AC_YELLOW "\x1b[33m"
#define AC_BLUE "\x1b[34m"
#define AC_MAGENTA "\x1b[35m"
#define AC_CYAN "\x1b[36m"
#define AC_WHITE "\x1b[37m"
#define AC_NORMAL "\x1b[m"


void write_to_file(char*line)
{
	FILE *file = fopen("tmp.tmp", "a");

	if(file == NULL)
	{
		printf("Can't open file\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		fprintf(file, "%s\n", line);
		fclose(file);
	}
}

char *who_is_it(const char *path)
{
	struct stat inf;
	if(lstat(path, &inf) == -1)
	{
		return "ERROR";
	}

	if((inf.st_mode & S_IFMT) == S_IFDIR)
	{
		return "DIR";
	}
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

char *read_dir(char *path)
{
	DIR *dir;
	struct dirent *ent;
	if((dir = opendir(path)) != NULL)
	{
		int i = 0;
		while((ent = readdir(dir)) != NULL)
		{
			i++;
			char tmp[_MAX_];
		        strcpy(tmp,path);
		        strcat(tmp, ent->d_name);
		        struct stat inf;
			if(lstat(tmp, &inf) == -1)
			{
				printf("ERROR\n");
			}
	
			if((inf.st_mode & S_IFMT) == S_IFDIR)
			{
				printf("\x1b[m%d \x1b[32m%s\n", i, ent->d_name);
			}
			else if((inf.st_mode & S_IFMT) == S_IFLNK)
			{
				printf("\x1b[m%d \x1b[36m%s\n", i, ent->d_name);
			} 
			else if((inf.st_mode & S_IFMT) == S_IFREG)
			{
				printf("\x1b[m%d \x1b[33m%s\n", i, ent->d_name);
			}
			else if((inf.st_mode & S_IFMT) == S_IFCHR)
			{
				printf("\x1b[m%d \x1b[34m%s\n", i, ent->d_name);
			}
			else if((inf.st_mode & S_IFMT) == S_IFBLK)
			{
				printf("\x1b[m%d \x1b[35m%s\n", i, ent->d_name);
			}
			else if((inf.st_mode & S_IFMT) == S_IFSOCK)
			{
				printf("\x1b[m%d \x1b[37m%s\n", i, ent->d_name);
			}
			else if((inf.st_mode & S_IFMT) == S_IFIFO)
			{
				printf("\x1b[m%d \x1b[38m%s\n", i, ent->d_name);
			}
			else
			{
				printf("\x1b[mERROR\n");
			}
			//printf("%d %s - %s\n", i, ent->d_name, who_is_it(strcat(tmp, ent->d_name)));
 			write_to_file(tmp);

		}
		closedir(dir);
	}
	else
	{
		printf("ERROR\n");
		exit(EXIT_SUCCESS);
	}
}

void open_next()
{

	if(remove("tmp.tmp") == -1)
	{
		printf("ERROR: Can't delite file\n");
	}
	else
	{
		//execv("programm.out", );
	}
}

int main(int argc, char* argv[])
{
	if(argc == 1)
	{
		read_dir("/");
	}
	else
	{
		read_dir(argv[1]);
	}

	printf("Enter the num: ");
	int num = 0;
	scanf("%d", &num);
	
	if(num == 0)
	{
		exit(EXIT_SUCCESS);
	}
	else
	{
		//open_next();
	}
}
