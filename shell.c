/************************************************************************  
 *   Shell.c  -  Just a simple shell
 *   Supports:
 *   -Multiple commands with piping
 *   -Redirection > and < same time
 *   -Built in commands: 
 *   	-Exit: Exit the Shell
 *   	-History: 
 *   		-Default(without argument): 
 *   		Last 10 History shown unless less than ten then all will be shown.  
 *   		-History "Number": Last Number of history shown
 *   
 ************************************************************************/

#include "shell.h"

/**
 * This is used when there is forking that needs to be done. 
 * @param int forkNum: The amount of forks needed. 
 * @param commandType *cmd: The first commandtype in the forking process. 
 * @return int: Whether or not this was successful. 
 */
int fork_pipes (int forkNum, struct commandType *cmd)
{
  int i;
  // Process ID 
  pid_t pid;
  int input, fd [2];

  // The first process should only get the input from the standard output 
  input = 0;

  // Now let's loop and start piping the process until last one, as the last one
  // only gets an input, no output redirection. 
  for (i = 0; i < forkNum - 1; ++i)
    {
      pipe (fd);

      // Since f[1] is the writing portion of the pipe, we carry the in from the previous 
      start_process (input, fd [1], cmd + i);

      // There is no need for the write portion of the pipe, as the children will write
      close (fd [1]);

      // Keep the read end of the pipe opened as the next children will read from here. 
      input = fd [0];
    }

  // Last process just reads from previous, then writes to standard output 
  if (input != 0)
  {
  	dup2(input, 0); 
  }

  // Execute the last process 
  return execvp (cmd[i].command, cmd[i].ArgList);
}

int start_process (int in, int out, struct commandType *cmd)
{
  pid_t pid;

  // If it is a children 
  if ((pid = fork ()) == 0)
    {
    	// Check if there is a input redirection if so then redirect 
		if (in != 0)
		{
			dup2 (in, 0);
			close (in);
		}
		// Check if there is a output redirection if so then redirect 
		if (out != 1)
		{
			dup2 (out, 1);
			close (out);
		}
		// Execute the process 
		return execvp (cmd->command, cmd->ArgList);
    }
   	/// Return the process id 
	return pid;	
}

// Description: This function is to check if the command is a built in command for the shell
// @param: char* cmd: A string 
// @return: int: The integer associated with the built in command
/**
 * This function is to check if the command is a built in function of the shell.  
 * @param: char* cmd: A string of the command that is to be checked 
 * @return: 0 if not a built in, 1 if exit buit in function, 2 if history. 
 */
int isBuiltInCommand(char *cmd)
{ 
  if ( strncmp(cmd, "exit", strlen("exit")) == 0){
    return EXIT;
  }
  else if( strncmp(cmd, "history", strlen("history")) == 0){
  	return HISTORY; 
  }
  return NONE_BUILT_IN;
}

// Description: This function is for printing the prompt in a format of "username > "
// return: char* username: The prompt for the user in the format of "username >" or 
// "Anon >" if a username cannot be grabbed from the system. 
/**
 * This function is to print the prompt for the shell. 
 * The format is: "username > " if username can be found from system otherwise
 * "Anon > ". 
 * @return: char* username: The string of the prompt 
 */
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

		// Start reading the line and show the prompt as well, store in cmdLine 
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
		// Below is to print the information to make sure parser is working. 
		// You can uncomment this if you want to check. 
		// print_info(info);

		// Set the current command to the one at the first command line of parser. 
		currentCommand = &info->CommArray[0];

		// Check if it is empty if so then free everyone to not waste space. 
		if ((currentCommand == NULL)  || (currentCommand->command == NULL)) {
			free_info(info); 
			free(cmdLine); 
			continue; 
		}

		// Check if exit built in command was called. If so exit successfully.
		if (isBuiltInCommand(currentCommand->command) == EXIT)
		{
			printf("Exiting The Shell...\n"); 
			exit(0); 
		}

		// Check if history built in command was called. If so show the history.
		else if(isBuiltInCommand(currentCommand->command) == HISTORY)
		{
			HIST_ENTRY **the_list;
			int i = 0; 
			// Counter to print the history 
			int printCounter = 1;
			// Set the end place of printing to where our history currently is. 
			int endPlace = where_history();
			int currentPlace = endPlace;
			int numHistory = 0; 

			// If there is an argument for history provided: 
			if(currentCommand->ArgList[1])
			{
				// Set the number of history wanted to our argument 
				numHistory = atoi(currentCommand->ArgList[1]);
				// If the number of history is set less than zero
				if (numHistory <= 0)
				{ 
					// Let the user know it should be a postive number. 
					printf("Error: Previous history of lines cannot be negative\n"); 
				}
				// Otherwise, set the end place to current place minus it 
				endPlace =  currentPlace - (numHistory - 1); 
			}
			// If there is no arguments provided 
			else
			{
				// Show default ten history 
				numHistory = 10; 
				endPlace =  currentPlace - numHistory; 
				// If there is not ten history then show all of it. 
				if (endPlace < 0)
				{
					endPlace = 0; 
				}
			}
			the_list = history_list(); 

			// Start cycling through the history list. 
			if ((the_list) && endPlace >= 0)
			{
				for (i = endPlace; i <= currentPlace; i++)
				{
					if(the_list[i])
					{ 
						// Print the number located in the history along with the line
						printf ("%d: %s\n", i + 1, the_list[i]->line); 
					}
				}
			}
			else if(the_list)
			{
				// If there is not enough history then we let the user know it can't be done
				printf("Error: You have requested more history than there is.\n");
			}
			else
			{
				// Print this if somehow there is an error with history which should never happen
				printf("Error: Unexpected Error With History Command Occured.\n");
			}
		}
		else
		{
			// Fork and set the pid to it.
			pid = fork();
			// If not a parent or children, an error occured and we let user know. 

			if (pid < 0)
			{
				perror("An issued arised processing your command."); 
			}
			// If it is a parent wait until the command is processed by the children. 
			if (pid > 0)
			{
				wait(NULL);
			}
			else
			{
				int in, out; 
				// Check if there is a redirection: 
				// Check if there is a input redirection
				if (info->boolInfile)
				{
					// Open the file, redirect it 
					in = open(info->inFile, O_RDONLY); 
					dup2(in, 0); 
					close(in);
				}
				// Check if there is an output redirection
				if (info->boolOutfile)
				{
					// Open the file, redirect it 
					out = open(info->outFile, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
					dup2(out, 1); 
					close(out);
				}
				// Check if there is piping in the command line 
				if (info->pipeNum > 0)
				{
					// Pipe it and if it is less than zero an error occured with piping 
					if (fork_pipes(info->pipeNum + 1, info->CommArray) < 0)
					{
						printf("The piping did not work\n");
					}

				}
				// If no piping just run the command.
				else if (info->pipeNum == 0)
				{
					if (execvp(currentCommand->command, currentCommand->ArgList) < 0) 
					{     /* execute the command  */
						printf("%s: command not found\n", currentCommand->command);
						exit(1);
					}
				}

			}			
		}
	}

	// Free the parsedInfo structure after it is done 
	free_info(info);
	// Return success 
	return 0;
}