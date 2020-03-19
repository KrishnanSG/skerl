#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<errno.h>
#include<sys/types.h>
#define SKERL_TOKKEN_BUFFERSIZE 64
#define SKERL_TOKKEN_DELIMITER " "

char cwd[1024];

char *builtin_command[] = {
	"cd","pwd","help"
};



int skerl_cd(char **internal_command);
int skerl_pwd(char **internal_command);
int skerl_help(char **internal_command);

int (*execute_builtin_command[]) (char **) = {
	&skerl_cd,
	&skerl_pwd,
	&skerl_help
};

int skerl_total_builtin_command()
{
	return sizeof(builtin_command) / sizeof(char *);
}



char **skerl_split_command(char *input_command)
{
	int buffer_size = SKERL_TOKKEN_BUFFERSIZE, position = 0;
	char **tokens = malloc(buffer_size * sizeof(char*));
	char *token, **tokens_backup;

	if (!tokens)
	{
		fprintf(stderr, "Skerl: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(input_command, SKERL_TOKKEN_DELIMITER);
	while (token != NULL)
	{
		tokens[position] = token;
		position++;

		if (position >= buffer_size)
		{
			buffer_size+= SKERL_TOKKEN_BUFFERSIZE;
			tokens_backup = tokens;
			tokens = realloc(tokens, buffer_size * sizeof(char*));

			if (!tokens)
			{
				free(tokens_backup);
				fprintf(stderr, "Skerl: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, SKERL_TOKKEN_DELIMITER);
	}
	tokens[position] = NULL;
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

int skerl_cd(char **internal_command)
{
	char *h="/home";
	if (internal_command[1] == NULL)
	{
		chdir(h);
	}
	else if ((strcmp(internal_command[1],"~")==0) || (strcmp(internal_command[1], "~/")==0))
	{
		chdir(h);
	}
	else if(chdir(internal_command[1]) < 0)
		printf("Skerl: cd: %s: No such file or directory\n", internal_command[1]);
}

int skerl_help(char **internal_command)
{
	int i;

	printf("type program names and arguments, and hit enter");
	printf("\nyou can try some builtin commands:\n");

	for (i=0; i<skerl_total_builtin_command(); i++)
	{
		printf(" %s\n", builtin_command[i]);
	}

	printf("Use the man command for information on other commands");

}

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
		if(strcmp(single_command[0], builtin_command[i]) == 0)
		{
			return (*execute_builtin_command[i])(single_command);
		}
	}

	return skerl_execute_external_command(single_command);
}



void skerl_parse_input_command(char *input_command)
{
	//here we have to split the command (|)..if there are n pipe operators will get n-1 commands 
	//that parsing should be done here..
	//we have send n-1 commands to exec..
	char **single_command = skerl_split_command(input_command);
	//now we need to execute the single command
	skerl_execute(single_command);
}

void skerl_prompt()
{
	char decorate_terminal[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		strcpy(decorate_terminal,"skerl:~");
		strcat(decorate_terminal,cwd);
		strcat(decorate_terminal,"$ ");
		printf("%s", decorate_terminal);
	}
	else
		perror("getcwd error..");
}

int main(int argv, char **argc)
{
	char new_line_checker[2] = {"\n"};
	char *input_command = NULL;
	ssize_t input_command_buffer = 0;
	int flag = 0;
	while (1)
	{
		skerl_prompt();
		getline(&input_command, &input_command_buffer, stdin);
		if (strcmp(input_command,new_line_checker) == 0)
		{
			continue;
		}
		input_command[strlen(input_command)-1] = '\0';
		if (strcmp(input_command,"exit") == 0)
		{
			printf("exiting skerl shell\n");
			flag = 1;
			exit(0);
		}
		skerl_parse_input_command(input_command);
	}
	return EXIT_SUCCESS;
}