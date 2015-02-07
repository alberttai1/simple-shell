/************************************************************************  
 *   parse.c  -  The parsing portion of my small shell  
 *   Syntax:     myshell command1 [< infile] [| command]* [> outfile] [&]
 ************************************************************************/

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h> 
#include <readline/history.h> 

#include "parse.h"
#define TRUE 1
#define FALSE 0 
#define DELIMITERS " \t\r\n"

// This is to initalize parseInfo structure  
void init_info(parseInfo *p) 
{
	int commandCounter = 0; 
	p = malloc(sizeof(parseInfo));
	p->boolInfile = FALSE;
	p->boolOutfile = FALSE;
	p->boolBackground = FALSE; 
	p->pipeNum = 0; 
	for (commandCounter = 0; commandCounter < PIPE_MAX_NUM; commandCounter++)
	{
		p->CommArray[commandCounter].ArgNum = 0; 
	}
}

//  parse a single command 
void parse_command(char * command, struct commandType *comm) 
{
	if (command!=NULL)
	{
		// printf("Command %d\n", comm->ArgNum); 
		if (comm->ArgNum == 0)
		{
			comm->command = malloc(sizeof(command));
			strcpy(comm->command, command); 
			comm->ArgList[0] = malloc(sizeof(command));
			strcpy(comm->ArgList[0], command); 
			comm->ArgNum++; 
		}
		else
		{
			comm->ArgList[comm->ArgNum] = malloc(sizeof(command));
			strcpy(comm->ArgList[comm->ArgNum], command); 	
			comm->ArgNum++;		
		}
		
	}
}

  /*   parse commandline for space separated commands */
parseInfo *parse (char *cmdLine) 
{
  	char *command; 
  	int commandCounter = 0; 
  	parseInfo *parsedCommands; 

  	command = strtok(cmdLine, DELIMITERS); 

  	while (command != NULL)
  	{
  		if(strcmp(command, "&") == 0)
  		{
  			parsedCommands->boolBackground = TRUE; 
  		}
  		else if(strcmp(command, "<") == 0)
  		{
  			parsedCommands->boolInfile = TRUE;
  			command = strtok(NULL, DELIMITERS); 
  			strcpy(parsedCommands->inFile, command); 
  		}
  		else if(strcmp(command, ">") == 0)
  		{
  			parsedCommands->boolOutfile = TRUE;
			command = strtok(NULL, DELIMITERS); 
  			strcpy(parsedCommands->outFile, command); 
  		}
  		else if(strcmp(command, "history") == 0)
  		{
  			parse_command(command, &(parsedCommands->CommArray[commandCounter]));
  			command = strtok(NULL, DELIMITERS); 
  			parse_command(command, &(parsedCommands->CommArray[commandCounter]));
  			commandCounter = commandCounter + 1; 
  			parsedCommands->pipeNum++;   			
  		}
  		else
  		{
  			parse_command(command, &(parsedCommands->CommArray[commandCounter]));
  			// commandCounter = commandCounter + 1; 
  			parsedCommands->pipeNum++; 
  		}
  		command = strtok(NULL, DELIMITERS); 
  	}
  	return parsedCommands; 
}

  // /* prints out parse struct */
void print_info (parseInfo *info) {
	int i = 0; 
	for (i = 0; i < info->pipeNum; i++)
	{
		printf("Arg%d: %s\n", i, info->CommArray[i].command);
	}
	printf("inpipe: %s\n", info->boolInfile == TRUE ? "Yes" : "No");
	printf("outpipe: %s\n", info->boolOutfile == TRUE ? "Yes" : "No");
	printf("background: %s\n", info->boolBackground == TRUE ? "Yes" : "No");
	printf("%s ", info->CommArray[0].command);
	if (info->boolInfile)
	{
		printf("< %s ", info->inFile);
	}
	if (info->boolOutfile)
	{
		printf("> %s ", info->outFile);
	}
	printf("\n"); 
}  

//     free memory used in parseInfo 
void free_info (parseInfo *info) 
{
	free(info); 
}


 // int main(int argc, char **argv) {
 // 	char *cmdLine; 
 // 	parseInfo *info;
 //    while(1) {
 //       cmdLine = readline("ABCDEF");
 //       init_info (info);
 //       info  = parse(cmdLine);

 //       print_info(info);
 //    }

 //    free_info(info);
 // }