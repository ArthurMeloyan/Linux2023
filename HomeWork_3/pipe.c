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
        if(execlp(argv[1], argv[1], NULL) < 0)
        {
        	perror("Execution failed");
        	return 1;
        }
    }

    for (int i = 1; i < count; ++i)
    {
        pid[i] = fork();
        if (pid[i] < 0)
        {
        	perror("Forking failed");
        	return 1;
        }
        if (pid[i] == 0)
        {
            close(fd[i-1][1]);
            dup2(fd[i-1][0], 0);
            close(fd[i-1][0]);

            close(fd[i][0]);
            dup2(fd[i][1], 1);
            close(fd[i][1]);
            if (execlp(argv[i+1], argv[i+1], NULL) < 0)
            {
            	perror("Execution failed");
            	return 1;
            }
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
