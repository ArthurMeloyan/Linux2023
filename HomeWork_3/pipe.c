#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{

    int count = argc - 1;

    int fd[count-1][2];
    pid_t pid[count];
    for (int i = 0; i < count - 1; ++i)
    {
        if (pipe(fd[i]) < 0)
        {
            perror("PIPE: ");
            return 1;
        }
    }

    pid[0] = fork();
    if (pid[0] == 0)
    {
        close(fd[0][0]);
        dup2(fd[0][1], 1);
        close(fd[0][1]);
        execlp(argv[1], argv[1], NULL);
    }

    for (int i = 1; i < count; ++i)
    {
        pid[i] = fork();
        if (pid[i] == 0)
        {
            close(fd[i-1][1]);
            dup2(fd[i-1][0], 0);
            close(fd[i-1][0]);

            close(fd[i][0]);
            dup2(fd[i][1], 1);
            close(fd[i][1]);
            execlp(argv[i+1], argv[i+1], NULL);
        }
    }
    for (int i = 0; i < count - 1; ++i)
    {
        close(fd[i][0]);
        close(fd[i][1]);
    }

    for (int i = 0; i < count; ++i)
    {
        waitpid(pid[i], NULL, 0);
    }
    return 0;

}