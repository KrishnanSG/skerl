/* ---- Skerl Shell ---- */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>

#define SKERL_TOKEN_BUFFERSIZE 64
#define SKERL_TOKEN_DELIMITER " "

char cwd[1024];

char *builtin_command[] = {
	"cd", "pwd", "help"};

/* Internal shell commands */
int skerl_cd(char **internal_command);
int skerl_pwd(char **internal_command);
int skerl_help(char **internal_command);

int (*execute_builtin_command[])(char **) = {
	&skerl_cd,
	&skerl_pwd,
	&skerl_help};

int skerl_total_builtin_command()
{
	return sizeof(builtin_command) / sizeof(char *);
}

char **skerl_split_command(char *input_command, int *count)
{
	int buffer_size = SKERL_TOKEN_BUFFERSIZE, position = 0;
	char **tokens = malloc(buffer_size * sizeof(char *));
	char *token, **tokens_backup;

	if (!tokens)
	{
		fprintf(stderr, "Skerl: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(input_command, SKERL_TOKEN_DELIMITER);
	while (token != NULL)
	{
		tokens[position] = token;
		position++;

		if (position >= buffer_size)
		{
			buffer_size += SKERL_TOKEN_BUFFERSIZE;
			tokens_backup = tokens;
			tokens = realloc(tokens, buffer_size * sizeof(char *));

			if (!tokens)
			{
				free(tokens_backup);
				fprintf(stderr, "Skerl: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, SKERL_TOKEN_DELIMITER);
	}
	tokens[position] = NULL;
	*count = position;
	return tokens;
}

int skerl_execute_external_command(char **single_command)
{
	pid_t pid;
	int status;
	pid = fork();
	if (pid == 0)
	{
		if (execvp(single_command[0], single_command) == -1)
		{
			perror("skerl");
		}
		exit(EXIT_FAILURE);
	}
	else if (pid < 0)
	{
		perror("skerl");
	}
	else
	{
		do
		{
			waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return 1;
}

/* Implementation of internal command (cd) */
int skerl_cd(char **internal_command)
{
	char *h = "/home";
	if (internal_command[1] == NULL)
	{
		chdir(h);
	}
	else if ((strcmp(internal_command[1], "~") == 0) || (strcmp(internal_command[1], "~/") == 0))
	{
		chdir(h);
	}
	else if (chdir(internal_command[1]) < 0)
		printf("Skerl: cd: %s: No such file or directory\n", internal_command[1]);
}

/* Implementation of internal command (help) */
int skerl_help(char **internal_command)
{
	int i;

	printf("type program names and arguments, and hit enter");
	printf("\nyou can try some builtin commands:\n");

	for (i = 0; i < skerl_total_builtin_command(); i++)
	{
		printf(" %s\n", builtin_command[i]);
	}

	printf("Use the man command for information on other commands");
}

/* Implementation of internal command (pwd) */
int skerl_pwd(char **internal_command)
{
	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		printf("%s\n", cwd);
	}
	else
	{
		perror("getcwd() error");
	}
}

int skerl_execute(char **single_command)
{
	int i;

	if (single_command[0] == NULL)
	{
		return 1;
	}
	//check if there are internal commands
	for (i = 0; i < skerl_total_builtin_command(); i++)
	{
		if (strcmp(single_command[0], builtin_command[i]) == 0)
		{
			return (*execute_builtin_command[i])(single_command);
		}
	}
	return skerl_execute_external_command(single_command);
}

/* parses the commanfd and tokenizes the input string */
void skerl_parse_input_command(char *input_command)
{
	// an array of single cmds / tokens are obtained
	int token_count;
	char **command = skerl_split_command(input_command, &token_count);

	// here we implement the logic to check for redirection
	int type = 0;
	/*
	 * 0 - input
	 * 1 - output
	 * 2 - pipe
	 */

	for (int i = 1; i < token_count; i++)
	{
		if ((strcmp(command[i], ">") == 0) || (strcmp(command[i], '>>') == 0))
		{
			// output redirection
			type = 1;
			int saved_stdout = dup(1);
			int output_file;
			if (strcmp(command[i], ">") == 0)
			{
				FILE *fptr = fopen(command[i + 1], "w");
				fclose(fptr);
				output_file = open(command[i + 1], O_WRONLY);
			}
			else
			{
				FILE *fptr = fopen(command[i + 1], "a");
				fclose(fptr);
				output_file = open(command[i + 1], O_WRONLY | O_APPEND);
			}

			int redirect_stream = dup2(output_file, 1);
			char *cmd[] = {command[i - 1], NULL};
			skerl_execute(cmd);
			dup2(saved_stdout, 1);
			close(saved_stdout);
		}
		else if (strcmp(command[i], "<") == 0)
		{
			// input redirection
			int input_file = open(command[i + 1], O_RDONLY);
			if (input_file > 0)
			{
				int redirect_stream = dup2(input_file, 0);
				skerl_execute(command[i - 1]);
			}
			else
			{
				perror(strcat(command[i + 1], " not found"));
			}
		}
		else if (strcmp(command[i], "|") == 0)
		{
			// pipes
			type = 1;
		}
	}
	if (type == 0)
	{
		skerl_execute(command);
	}
}

void skerl_prompt()
{
	char prompt_msg[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		strcpy(prompt_msg, "skerl:~");
		strcat(prompt_msg, cwd);
		strcat(prompt_msg, "$ ");
		printf("%s", prompt_msg);
	}
	else
		perror("getcwd error..");
}

int display_history_file()
{
	FILE *fptr;
	fptr = fopen("history.txt", "r");
	int count = 0;

	if (fptr != NULL)
	{
		char s[100];
		while (fgets(s, sizeof(s), fptr) != NULL)
		{
			printf("#%d - %s", count + 1, s);
			count = count + 1;
		}
	}
	if (count == 0)
		return 0;
	else
		return 1;
	fclose(fptr);
}

/* writes cmds into history file */
void write_history(char *command)
{
	FILE *fptr;
	fptr = fopen("history.txt", "a");
	if (fptr != NULL)
	{
		fprintf(fptr, "%s\n", command);
	}
	fclose(fptr);
}

/* returns the cmd from history corresponding to the history number */
char *return_history(int history_number)
{
	FILE *fptr;
	char s[100];
	char *c;
	fptr = fopen("history.txt", "r");
	int count = 0;

	if (fptr != NULL)
	{
		while (fgets(s, sizeof(s), fptr) != NULL)
		{
			count = count + 1;
			if (count == history_number)
			{
				fclose(fptr);
				strcpy(c, s);
				return c;
			}
		}
	}
	fclose(fptr);
	return NULL;
}

int main(int argv, char **argc)
{
	char new_line_checker[2] = {"\n"};
	char *input_command = NULL; // Stores the shell inputs
	ssize_t input_command_buffer = 0;
	int history_choice;
	int flag = 0;
	int enter_history = 0;
	while (1)
	{
		enter_history = 0;
		skerl_prompt();
		getline(&input_command, &input_command_buffer, stdin); // accept input
		if (strcmp(input_command, new_line_checker) == 0)
		{
			continue;
		}
		input_command[strlen(input_command) - 1] = '\0';
		if (strcmp(input_command, "exit") == 0) // terminate shell on exit cmd
		{
			printf("exiting skerl shell\n");
			write_history(input_command);
			flag = 1;
			exit(0);
		}
		if (strcmp(input_command, "history") == 0)
		{
			int count = display_history_file();
			if (count == 0)
			{
				printf("No recent history\n");
				continue;
			}
			else
			{
				printf("\nEnter the history number : ");
				scanf("%d", &history_choice);
				input_command = return_history(history_choice);
				if (input_command == NULL)
				{
					printf("wrong input\n");
					continue;
				}
				write_history(input_command);
				input_command[strlen(input_command) - 1] = '\0';
				enter_history = 1;
			}
		}
		if (enter_history == 0)
		{
			write_history(input_command);
		}
		skerl_parse_input_command(input_command); // handle and parse internal and external cmds
		input_command = NULL;
	}
	return EXIT_SUCCESS;
}