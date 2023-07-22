#include <stdio.h>
#include <stdlib.h>
//#include <io.h>
#include <unistd.h>
#include <string.h>

#define _MAX_ 25

int main(int argc, char* argv[])
{
	char path[_MAX_], *prog;
	if(!strcmp(argv[1], "1_1") || !strcmp(argv[1], "1_3"))	//Задание 1.4
	{
		prog = argv[1];
	}
	else							//Задание 1.5
	{
		strcat(path, "/bin/");
		strcat(path, argv[1]);
		if(!access(path, 1))
		{
			prog = path;
		}
		else
		{
			for(int i = 0; i < _MAX_; i++)
			{
				path[i] = "";
			}
			strcat(path, "/sbin/");
			strcat(path, argv[1]);
			if(!access(path, 1))
			{
				prog = path;
			}
			else
			{
				printf("Dose not exist\n");
			}
		}
	}
	
	argv[1] = " ";
	execv(prog, argv);
}
