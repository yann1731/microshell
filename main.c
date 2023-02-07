#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

void    perr(char *str)
{
    while (*str)
        write(2, str++, 1);
}

int cd(char **argv, int i)
{
    if (i != 2)
        return (perr("error: cd: bad arguments\n"), 1);
    else if (chdir(argv[1]) == -1)
        return (perr("error: cd: cannot change directory to "), perr(argv[1]), perr("\n"), 1);
    return 0;
}

int exec(char **argv, char **envp, int i)
{
    int status;
    int fds[2];
    int pip = (argv[i] && !strcmp(argv[i], "|"));

    if (pipe(fds) == -1)
        return (perr("error: fatal\n"), 1);
    int pid = fork();
    if (!pid)
    {
        argv[i] = 0;
        if (pip && (dup2(fds[1], 1) == -1 || close(fds[0]) || close(fds[1])))
            return (perr("error: fatal\n"), 1);
        execve(*argv, argv, envp);
        return (perr("error: cannot execute "), perr(*argv), perr("\n"), 1);
    }
    waitpid(pid, &status, 0);
    if (pip && (dup2(fds[0], 0) == -1 || close(fds[0]) || close(fds[1])))
        return (perr("error: fatal\n"), 1);
    return WIFEXITED(status) && WEXITSTATUS(status);
}

int main(int argc, char **argv, char **envp)
{
    (void)argc;
    int i = 0;
    int status = 0;

    while (*argv && *(argv + 1))
    {
        ++argv;
        i = 0;
        while (argv[i] && strcmp(argv[i], ";") && strcmp(argv[i], "|"))
            i++;
        if (!strcmp(*argv, "cd"))
            status = cd(argv, i);
        else if (i)
            status = exec(argv, envp, i);
        argv += i;
    }
    return status;
}


// #include <string.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <stdio.h>
// #include <limits.h>
// #include <sys/time.h>
// #include <sys/wait.h>

// typedef struct s_data
// {
// 	char **argv;
// 	int fd_in;
// 	int fd_out;
// 	int pid;
// 	int redir;
// } t_data;

// int ft_strlen(char *s)
// {
// 	int i = 0;

// 	while (s[i])
// 		i++;
// 	return i;
// }

// void error_fatal(void)
// {
// 	write(2, "error: fatal\n", ft_strlen("error: fatal\n"));
// 	exit(1);
// }

// int countArgs(char *argv[])
// {
// 	int count = 0;

// 	while (argv[count])
// 	{
// 		if (!strcmp(argv[count], "|") || !strcmp(argv[count], ";"))
// 			break ;
// 		count++;
// 	}
// 	return count;
// }

// void handle_cd(char *argv[])
// {
// 	int i = countArgs(argv);

// 	if (i > 2 || i == 1)
// 		write(2, "cd: bad arguments\n", ft_strlen("cd: bad arguments\n"));
// 	else
// 	{
// 		if (chdir(*(argv + 1)) != 0)
// 		{
// 			write(2, "error: cd: cannot change directory to ", ft_strlen("error: cd: cannot change directory to "));
// 			write(2, *(argv + 1), ft_strlen(*(argv + 1)));
// 			write(2, "\n", 1);
// 		}
// 		else
// 			printf("Success\n");
// 	}
// }

// char **skip_cmd(char *argv[])
// {
// 	while (*argv && *argv[0] != '|' && *argv[0] != ';')
// 		argv++;
// 	return argv;
// }

// int check_pipe(t_data *data)
// {
// 	int i = -1;
// 	while (data->argv[++i])
// 	{
// 		if (data->argv[i][0] == '|')
// 		{
// 			data->redir = 0;
// 			return 0;
// 		}
// 	}
// 	data->redir = 1;
// 	return 1;
// }

// void redirection(t_data *data, int fd[])
// {
// 	if (data->pid > 0)
// 	{
// 		close(fd[1]);
// 		dup2(fd[0], STDIN_FILENO);
// 		close(fd[0]);
// 	}
// 	else
// 	{
// 		close(fd[0]);
// 		dup2(fd[1], STDOUT_FILENO);
// 		close(fd[1]);
// 	}
// }

// char **run_cmd(t_data *data, char *envp[])
// {
// 	int fd[2];
// 	pipe(fd);
// 	data->pid = fork();
// 	if (data->pid > 0)
// 	{
// 		close(fd[1]);
// 		dup2(fd[0], STDIN_FILENO);
// 		close(fd[0]);
// 		return (data->argv = skip_cmd(data->argv));
// 	}
// 	else
// 	{
// 		close(fd[0]);
// 		close(fd[1]);
// 		int i = 0;
// 		while (data->argv[i])
// 		{
// 			if (data->argv[i][0] == '|' || data->argv[i][0] == ';')
// 			{
// 				data->argv[i] = NULL;
// 				break ;
// 			}
// 			i++;
// 		}
// 		execve(data->argv[0], data->argv, envp);
// 		write(2, "error: unknown command", ft_strlen("error: unknown command"));
// 		exit(1);
// 	}
// }

// int main(int argc, char *argv[], char *envp[])
// {
// 	argv++;
// 	if (argc == 1)
// 		return 1;
// 	else
// 	{
// 		t_data data;
// 		data.fd_in = dup(STDIN_FILENO);
// 		data.fd_out = dup(STDOUT_FILENO);
// 		data.redir = -1;
// 		data.argv = argv;
// 		while (*data.argv)
// 		{
// 			if (strcmp(*(data.argv), "cd") == 0)
// 			{
// 				handle_cd(data.argv);
// 				data.argv = skip_cmd(data.argv);
// 				if (!*(data.argv))
// 					return 0;
// 			}
// 			else
// 			{
// 				data.argv = run_cmd(&data, envp);
// 				if (!*(data.argv))
// 					return 0;
// 				if (!strcmp(*(data.argv), ";"))
// 				{
// 					dup2(STDIN_FILENO, data.fd_in);
// 					dup2(STDOUT_FILENO, data.fd_out);
// 				}
// 			}
// 			data.argv++;
// 		}
// 		dup2(STDIN_FILENO, data.fd_in);
// 		dup2(STDOUT_FILENO, data.fd_out);
// 	}
// 	waitpid(0, NULL, 0);
// 	return 0;
// }