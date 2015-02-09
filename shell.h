#include <stdio.h>
#include <stdlib.h>
// This is for reading from standard input, and storing the history 
#include <readline/readline.h> 
#include <readline/history.h> 

#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "parse.h"
#include <fcntl.h>

// Max length of the input from the shell 
#define MAX_LENGTH 1024

enum
BUILTIN_COMMANDS { NONE_BUILT_IN=0, EXIT,HISTORY};

int fork_pipes (int n, struct commandType *cmd);
int spawn_proc (int in, int out, struct commandType *cmd);
int isBuiltInCommand(char *cmd);
char* printPrompt();
