#ifndef __COMMANDS__H
#define __COMMANDS__H

#include <sys/types.h>

#define MAX_LENGTH_COMMAND_ARGS 256

extern int  COMMAND_FLAG_BACKEND ;
extern int  COMMAND_FLAG_REDIRECT_IN ;
extern int  COMMAND_FLAG_REDIRECT_OUT ;
extern int  COMMAND_FLAG_REDIRECT_OUT_APPEND ;
extern int  COMMAND_FLAG_REDIRECT_ERR ;
extern int  COMMAND_FLAG_REDIRECT_ERR_APPEND ;
extern int  COMMAND_FLAG_PIPE ;
extern int  COMMAND_FLAG_REDIRECT_1MERGE2 ;
extern int  COMMAND_FLAG_REDIRECT_2MERGE1 ;

extern char* MSG_ERR_MAXARGS ;

typedef struct _command{
	int args_length;
	int flag_bits;
	int hash_val;
	pid_t pid;
	char* error_msg;
	char* command_args[MAX_LENGTH_COMMAND_ARGS];
	char* out_file;
	char* err_file;
	char* in_file;
	struct _command *next_command;
} command_t;

char *read_commands(const char* prompt);
command_t* parse_commands(char *command_str);
int destory_commands(command_t *command);

#endif // __COMMANDS__H
