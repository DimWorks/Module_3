#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <string.h>

#define _MAX_ 20

int get_arg(char *line[_MAX_], int *n)
{
    char c = 0;
    *n = 0;
    while (c != '\n' && *n < _MAX_)
    {
        line[*n] = (char*)malloc(sizeof(char));
        scanf("%s%c", line[*n], &c);
        (*n)++;
    }
    line[*n] = NULL;
    return 0;
}

int main(int argc, char *argv[])
{
    char *arg[_MAX_];

    int n = 0;

    pid_t pid;
    int status = 0;

    printf("Enter programm and arguments: ");
    get_arg(arg, &n);

    int pipefd[_MAX_][2];
    for(int i = 0; i < _MAX_; i++)
    {
        pipe(pipefd[i]);
    }

    int argi = 0;
    int cmdi = 0;
    int pipei = 0;

    while(argi < _MAX_)
    {
        if (arg[argi] == NULL)
        {
            if (cmdi == 0)
            {
                switch(pid = fork())
                {
                    case -1:
                        perror("fork");
                        exit(EXIT_SUCCESS);
                    case 0:
                        execvp(arg[cmdi], &arg[cmdi]);
                }
            }
            else
            {
                switch(pid = fork())
                {
                    case -1:
                        perror("fork");
                        exit(EXIT_SUCCESS);
                    case 0:
                        close(pipefd[pipei - 1][1]);
                        dup2(pipefd[pipei - 1][0], STDIN_FILENO);
                        execvp(arg[cmdi], &arg[cmdi]);
                }
            }

            pipei++;
            cmdi = argi + 1;

            break;
        }
        else if (strcmp(arg[argi], "|") == 0)
        {
            arg[argi] = NULL;

            if (cmdi == 0)
            {
                switch(pid = fork())
                {
                    case -1:
                        perror("fork");
                        exit(EXIT_SUCCESS);
                    case 0:
                        close(pipefd[pipei][0]);
                        dup2(pipefd[pipei][1], STDOUT_FILENO);
                        execvp(arg[cmdi], &arg[cmdi]);
                }
            }
            else
            {
                switch(pid = fork())
                {
                    case -1:
                        perror("fork");
                        exit(EXIT_SUCCESS);
                    case 0:
                        close(pipefd[pipei][0]);
                        dup2(pipefd[pipei][1], STDOUT_FILENO);
                        close(pipefd[pipei - 1][1]);
                        dup2(pipefd[pipei - 1][0], STDIN_FILENO);
                        execvp(arg[cmdi], &arg[cmdi]);
                }
            }

            pipei++;
            cmdi = argi + 1;
        }

        argi++;
    }

    for (int i = 0; i < n; i++)
    {
        free(arg[i]);
    }

    exit(EXIT_SUCCESS);
}
