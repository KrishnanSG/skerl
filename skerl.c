#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<errno.h>
#include<sys/types.h>

char cwd[1024];

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
	char inputCommand[1024];
	int flag = 0;
	while(1)
	{
		SkerlPrompt();
		gets(inputCommand);
		if(strcmp(inputCommand,newLineChecker) == 0)
		{
			printf("\nHello");
			continue;
		}
		if(strcmp(inputCommand,"exit") == 0)
		{
			printf("Exiting Skerl Shell");
			flag = 1;
			break;
		}
	}
	if(flag == 1)
	{
		exit(0);
	}
	return EXIT_SUCCESS;
}