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

char **SkerlSplitCommand(char *inputCommand)
{
	int bufferSize = SKERL_TOKKEN_BUFFERSIZE, position = 0;
	char **tokens = malloc(bufferSize * sizeof(char*));
	char *token, **tokensBackup;

	if (!tokens)
	{
		fprintf(stderr, "Skerl: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(inputCommand, SKERL_TOKKEN_DELIMITER);
	while (token != NULL)
	{
		tokens[position] = token;
		position++;

		if (position >= bufferSize)
		{
			bufferSize+= SKERL_TOKKEN_BUFFERSIZE;
			tokensBackup = tokens;
			tokens = realloc(tokens, bufferSize * sizeof(char*));

			if (!tokens)
			{
				free(tokensBackup);
				fprintf(stderr, "Skerl: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, SKERL_TOKKEN_DELIMITER);
	}
	tokens[position] = NULL;
	return tokens;
}

void skerlParseInputCommand(char *inputCommand)
{
	//here we have to split the command (|)..if there are n pipe operators will get n-1 commands 
	//that parsing should be done here..
	//we have send n-1 commands to exec..
	char **singleCommand = SkerlSplitCommand(inputCommand);
	printf("%s",singleCommand[0]);
}

void SkerlPrompt()
{
	char decorateTerminal[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		strcpy(decorateTerminal,"Skerl:~");
		strcat(decorateTerminal,cwd);
		strcat(decorateTerminal,"$ ");
		printf("%s", decorateTerminal);
	}
	else
		perror("getcwd error..");
}

int main(int argv, char **argc)
{
	char newLineChecker[2] = {"\n"};
	char *inputCommand = NULL;
	ssize_t inputCommandBuffer = 0;
	int flag = 0;
	while (1)
	{
		SkerlPrompt();
		getline(&inputCommand, &inputCommandBuffer, stdin);
		if (strcmp(inputCommand,newLineChecker) == 0)
		{
			continue;
		}
		inputCommand[strlen(inputCommand)-1] = '\0';
		if (strcmp(inputCommand,"exit") == 0)
		{
			printf("Exiting Skerl Shell\n");
			flag = 1;
			exit(0);
		}
		skerlParseInputCommand(inputCommand);
	}
	return EXIT_SUCCESS;
}