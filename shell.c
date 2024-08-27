#include "main.h"

#define BUFFER_SIZE 1024

void display_prompt(void);
char *read_command(void);
void execute_command(char *buffer);
char *find_command(char *command);
int builtin_handler(char **argv);

/**
 * main - Entry point for the shell
 *
 * Return: Always 0
 */
int main(void)
{
	char *buffer;

	while (1)
	{
		display_prompt();
		buffer = read_command();

		if (buffer == NULL)
		{
			printf("\n");
			break;
		}

		if (strlen(buffer) > 0)
			execute_command(buffer);

		free(buffer);
	}

	return (0);
}

/**
 * display_prompt - Displays the shell prompt
 */
void display_prompt(void)
{
	printf("($) ");
}

/**
 * read_command - Reads the command inputted by the user
 *
 * Return: Pointer to the command string
 */
char *read_command(void)
{
	char *buffer = NULL;
	size_t buffsize = 0;
	ssize_t characters;

	characters = getline(&buffer, &buffsize, stdin);

	if (characters == -1) /* Handle EOF */
	{
		free(buffer);
		return (NULL);
	}

	buffer[strcspn(buffer, "\n")] = '\0'; /* Delete newline */

	return (buffer);
}

/**
 * execute_command - Executes the user command
 * @buffer: The command string
 */
void execute_command(char *buffer)
{
	char *argv[BUFFER_SIZE];
	int i = 0;
	char *command_path;
	pid_t pid;
	int status;

	argv[i] = strtok(buffer, " ");
	while (argv[i] != NULL)
		argv[++i] = strtok(NULL, " ");

	if (builtin_handler(argv))
		return;

	command_path = find_command(argv[0]);

	if (command_path == NULL)
	{
		printf("%s: command not found\n", argv[0]);
		return;
	}

	pid = fork(); /* Create child process */
	if (pid == -1) /* Fork failure */
	{
		perror("Error:");
		exit(EXIT_FAILURE);
	}
	if (pid == 0) /* Child process */
	{
		if (execve(command_path, argv, NULL) == -1)
		{
			perror("Error");
			exit(EXIT_FAILURE);
		}
	}
	else /* Parent process */
		wait(&status); /* wait for child to complete */
	if (command_path != argv[0])
		free(command_path);
}

/**
 * builtin_handler - Handles the built-in commands
 * @argv: The argument vector
 *
 * Return: 1 if a built-in command was handled, 0 otherwise
 */
int builtin_handler(char **argv)
{
	if (strcmp(argv[0], "exit") == 0)
		exit(0);
	else if (strcmp(argv[0], "env") == 0)
	{
		char **env = environ;

		while (*env)
		{
			printf("%s\n", *env);
			env++;
		}
		return (1);
	}

	return (0);
}

/**
 * find_command -Finds the command in the PATH
 * @command: The command to find
 *
 * Return: The full PATH to the command if it is found or NULL
 */
char *find_command(char *command)
{
	char *path_env = getenv("PATH");
	char *path, *token, *command_path;
	size_t len;

	if (path_env == NULL || command == NULL || *command == '\0')
		return (NULL);

	if (strchr(command, '/') != NULL)
	{
		if (access(command, X_OK) == 0)
			return (command);
		else
			return (NULL);
	}

	path = strdup(path_env);
	token = strtok(path, ":");

	while (token != NULL)
	{
		len = strlen(token) + strlen(command) + 2;
		command_path = malloc(len);
		if (command_path == NULL)
		{
			free(path);
			return (NULL);
		}

		snprintf(command_path, len, "%s/%s", token, command);

		if (access(command_path, X_OK) == 0)
		{
			free(path);
			return (command_path);
		}

		free(command_path);
		token = strtok(NULL, ":");
	}

	free(path);

	return (NULL);
}
