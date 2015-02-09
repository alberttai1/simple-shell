#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_VAR_NUM 10
#define PIPE_MAX_NUM 10
#define FILE_MAX_SIZE 40
/**
 * This is the structure of a command. 
 * 
 * command : The string of the command ex: ls 
 * ArgList: An array of string of arguments such as -l 
 * ArgNum: The amount of arguments 
 */
struct commandType {
  char *command;
  char *ArgList[MAX_VAR_NUM];
  int ArgNum;
};

/**
 * This is the structure of parsed line. 
 *
 * boolInFile: Whether or not there is a input file. 
 * boolOutFile: Whether or not there is an output file. 
 * boolBackground: Whether or not the process is running in the background. 
 * CommArray: The array of commands processed from the line 
 * inFile: The name of the file being redirected from
 * outFile: The name of the file being redirected into 
 */
typedef struct {
  int   boolInfile;		       
  int   boolOutfile;		       
  int   boolBackground;		       
  struct commandType CommArray[PIPE_MAX_NUM];
  int   pipeNum;
  char  inFile[FILE_MAX_SIZE];	       
  char  outFile[FILE_MAX_SIZE];	       
} parseInfo;

// Prototype functions 
void init_info(parseInfo *p);
void parse_command(char * command, struct commandType *comm);
parseInfo *parse(char *);
void free_info(parseInfo *);
void print_info(parseInfo *);











