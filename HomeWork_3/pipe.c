#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        printf("Invalid count of arguments is provided");
        return 1;
    }

    int fd[argc-2][2];
    pid_t pid[argc-1];
    for (int i = 0; i < argc - 2; ++i)
    {
        if (pipe(fd[i]) < 0)
        {
            perror("PIPE: ");
            return 1;
        }
    }

    pid[0] = fork();
    if (pid[0] < 0)
    {
        perror("Unable to create a child process");
        return 1;
    }
    if (pid[0] == 0)
    {
        close(fd[0][0]);
        dup2(fd[0][1], 1);
        close(fd[0][1]);
        if (execlp(argv[1], argv[1], NULL) < 0)
        {
            perror("Execution failed");
            return 1;
        }
    }

    for (int i = 1; i < argc - 2; ++i)
    {
        pid[i] = fork();
        if (pid[i] < 0)
        {
            perror("Cannot create a child process");
            return 1;
        }
        if (pid[i] == 0)
        {
            close(fd[i-1][1]);
            dup2(fd[i-1][0], 0);
            close(fd[i-1][0]);

            close(fd[i][0]);
            dup2(fd[i][1],1);
            close(fd[i][1]);
             for (int j = 0; j < argc - 2; ++j)
            {
                if (j != i && j != i - 1)
                {
                    close(fd[j][0]);
                    close(fd[j][1]);
                }
            }
            if (execlp(argv[i+1], argv[i+1], NULL) < 0)
            {
                perror("Execution failed");
                return 1;
            }
        }
    }

  

    pid[argc-2] = fork();
    if (pid[argc-2] < 0)
    {
        perror("Unable create a child process");
        return 1;
    }
    if (pid[argc-2] == 0)
    {
        for (int i = 0; i < argc - 3; ++i)
        {
            close(fd[i][0]);
            close(fd[i][1]);
        }
        
        close(fd[argc-3][1]);
        dup2(fd[argc-3][0], 0);
        close(fd[argc-3][0]);
        if (execlp(argv[argc-1], argv[argc-1], NULL) < 0)
        {
            perror("Execution failed");
            return 1;
        }
    }
    for (int i = 0; i < argc - 2; ++i)
    {
        close(fd[i][0]);
        close(fd[i][1]);
    }

    for (int i = 0; i < argc - 1; ++i)
    {
        waitpid(pid[i], NULL, 0);
    }
    return 0;
}