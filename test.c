#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[], char *envp[])
{
	(void) argc;
	(void) argv;
	char *ls[] = {"/bin/ls", NULL};
	char *wc[] = {"/usr/bin/wc", NULL};

	int fd[2];

	pipe(fd);
	int pid = fork();
	if (pid > 0)
	{
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO);
		close(fd[0]);
	}
	else
	{
		close(fd[0]);
		close(fd[1]);
		execve(wc[0], wc, envp);
	}
	waitpid(pid, NULL, 0);
	return 0;
}