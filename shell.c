#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h> 
#include <readline/history.h> // Library for the history feature that utilizes this library

#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "parse.h"

#define MAX_LENGTH 1024
#define DELIMITERS " \t\r\n"

enum
BUILTIN_COMMANDS { NO_SUCH_BUILTIN=0, EXIT,HISTORY};

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
// This function is for printing the prompt in a format of "username > "
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

    using_history (); 

	while(1){
		cmdLine = readline(prompt);

		if (*cmdLine && cmdLine)
		{
			add_history(cmdLine);
		}

		init_info (info);
		info  = parse(cmdLine);

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
			printf("Exiting The Shell..."); 
			exit(1); 
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
				printf("Error: You have requested more history than there is.\n ");
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
				perror("Fork()"); 
			}
			if (pid > 0)
			{
				wait(NULL);
			}
			else
			{
	          if (execvp(currentCommand->command, currentCommand->ArgList) < 0) 
	          {     /* execute the command  */
               printf("*** ERROR: exec failed\n");
               exit(1);
	          }
			}			
		}
	}
	// }
	free_info(info);
	return 0;
}