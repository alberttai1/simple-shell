#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h> 
#include <readline/history.h> // Library for the history feature that utilizes this library

#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "parse.h"
#include <fcntl.h>

#define MAX_LENGTH 1024
#define DELIMITERS " \t\r\n"

enum
BUILTIN_COMMANDS { NO_SUCH_BUILTIN=0, EXIT,HISTORY};


// Description: This function is to check if the command is a built in command for the shell
// @param: char* cmd: A string 
// @return: int: The integer associated with the built in command
int isBuiltInCommand(char *cmd)
{ 
  if ( strncmp(cmd, "exit", strlen("exit")) == 0){
    return EXIT;
  }
  else if( strncmp(cmd, "history", strlen("history")) == 0){
  	return HISTORY; 
  }
  return NO_SUCH_BUILTIN;
}

// Description: This function is for printing the prompt in a format of "username > "
// return: char* username: The prompt for the user in the format of "username >" or 
// "Anon >" if a username cannot be grabbed from the system. 
char* printPrompt(){
	char *username = getenv("USER");
	if (username == NULL) return "Anon > ";  
	strcmp(username, strcat(username, " > "));
	return username; 
}

int main (int argc, char **argv){
	char line[MAX_LENGTH];
	char *cmdLine; 
	char *prompt = printPrompt(); 
	parseInfo *info;
	struct commandType *currentCommand; //com stores command name and Arg list for one command.

    pid_t pid;

    // Start recording history here. 
    using_history (); 

    // Keep looping until user chooses to Exit, or a error occurs 
	while(1){
		// Start reading the line and show the prompt as well 
		cmdLine = readline(prompt);

		// Add command line to history if not empty
		if (*cmdLine && cmdLine)
		{
			add_history(cmdLine);
		}

		// Initalize the parseInfo, making sure the variables are set to default 
		init_info (info);

		// Each line entered, parse it into correct format for parse info 
		info  = parse(cmdLine);

		// Check if the parsedInfo is null and if so free the command line and continue 
		if (info == NULL){
			free(cmdLine); 
			continue; 
		}
		print_info(info);

		currentCommand = &info->CommArray[0];
		if ((currentCommand == NULL)  || (currentCommand->command == NULL)) {
			free_info(info); 
			free(cmdLine); 
			continue; 
		}
		if (isBuiltInCommand(currentCommand->command) == EXIT)
		{
			printf("Exiting The Shell...\n"); 
			exit(0); 
		}
		else if(isBuiltInCommand(currentCommand->command) == HISTORY)
		{
			HIST_ENTRY **the_list;
			int i = 0; 
			int printCounter = 1;

			int endPlace = where_history();
			int currentPlace = endPlace;

			int numHistory = 0; 

			if(currentCommand->ArgList[1])
			{
				numHistory = atoi(currentCommand->ArgList[1]);
				if (numHistory <= 0)
				{
					printf("Error: Previous history of lines cannot be negative\n"); 
				}
				endPlace =  currentPlace - (numHistory - 1); 
			}
			else
			{
				numHistory = 10; 
				endPlace =  currentPlace - numHistory; 
				if (endPlace < 0)
				{
					endPlace = 0; 
				}
			}

			the_list = history_list(); 

			if ((the_list) && endPlace >= 0)
			{
				for (i = endPlace; i <= currentPlace; i++)
				{
					if(the_list[i])
					{ 
						printf ("%d: %s\n", i + 1, the_list[i]->line); 
					}
				}
			}
			else if(the_list)
			{
				printf("Error: You have requested more history than there is.\n");
			}
			else
			{
				printf("Error: Unexpected Error With History Command Occured.\n");
			}
		}
		else
		{
			// fork 
			pid = fork();
			if (pid < 0)
			{
				perror("Fork had an issue"); 
			}
			if (pid > 0)
			{
				wait(NULL);
			}
			else
			{
				int in, out; 
				if (info->boolInfile)
				{
					in = open(info->inFile, O_RDONLY); 
					dup2(in, 0); 
					close(in);
				}
				if (info->boolOutfile)
				{
					out = open(info->outFile, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
					dup2(out, 1); 
					close(out);
				}
				if (execvp(currentCommand->command, currentCommand->ArgList) < 0) 
				{     /* execute the command  */
				printf("%s: command not found\n", currentCommand->command);
				exit(1);
				}
			}			
		}
	}
	// }
	free_info(info);
	return 0;
}