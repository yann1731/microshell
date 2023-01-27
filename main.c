#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>

typedef struct s_data
{
	char **argv;
	int fd_in;
	int fd_out;
	int pid;
} t_data;

int ft_strlen(char *s)
{
	int i = 0;

	while (s[i])
		i++;
	return i;
}

void error_fatal(void)
{
	write(2, "error: fatal\n", ft_strlen("error: fatal\n"));
	exit(1);
}

int countArgs(char *argv[])
{
	int count = 0;

	while (argv[count])
	{
		if (!strcmp(argv[count], "|") || !strcmp(argv[count], ";"))
			break ;
		count++;
	}
	return count;
}

void handle_cd(char *argv[])
{
	int i = countArgs(argv);

	if (i > 2 || i == 1)
		write(2, "cd: bad arguments\n", ft_strlen("cd: bad arguments\n"));
	else
	{
		if (chdir(*(argv + 1)) != 0)
		{
			write(2, "error: cd: cannot change directory to ", ft_strlen("error: cd: cannot change directory to "));
			write(2, *(argv + 1), ft_strlen(*(argv + 1)));
			write(2, "\n", 1);
		}
		else
			printf("Success\n");
	}
}

char **skip_cmd(char *argv[])
{
	while (*argv && !strcmp(*argv, "|") && !strcmp(*argv, ";"))
	{
		argv++;
	}
	return argv;
}

char **run_cmd(t_data *data, char *envp[])
{
	data->pid = fork();

	if (data->pid == 0)
	{
		char *args[1000];
		int i = 0;
	
		while (data->argv[i] && !strcmp(data->argv[i], "|") && !strcmp(data->argv[i], ";"))
		{
			args[i] = data->argv[i];
			i++;
		}
		args[i] = NULL;
		execve(args[0], args, envp);
		write(2, "error: unknown command", ft_strlen("error: unknown command"));
		exit(1);
	}
	else
	{
		return (data->argv = skip_cmd(data->argv));
	}
}

void redirection(t_data *data)
{
	int fd[2];
	pipe(fd);
	if (data->pid > 0)
	{
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO);
		close(fd[0]);
	}
	else
	{
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);
	}
}

int main(int argc, char *argv[], char *envp[])
{
	argv++;
	if (argc == 1)
		return 1;
	else
	{
		t_data data;
		data.fd_in = dup(STDIN_FILENO);
		data.fd_out = dup(STDOUT_FILENO);
		data.argv = argv;
		int i = -1;
		while (data.argv[++i])
		{
			if (strcmp(*(data.argv), "cd") == 0)
			{
				handle_cd(data.argv);
				data.argv = skip_cmd(data.argv);
				if (!*(data.argv))
					return 0;
			}
			else
			{
				argv = run_cmd(&data, envp);
				if (strcmp(*(data.argv), "|"))
					i++;
				if (strcmp(*(data.argv), ";"))
				{
					dup2(STDIN_FILENO, data.fd_in);
					dup2(STDOUT_FILENO, data.fd_out);
					i++;					
				}
			}
		}
		dup2(STDIN_FILENO, data.fd_in);
		dup2(STDOUT_FILENO, data.fd_out);
	}

	return 0;
}