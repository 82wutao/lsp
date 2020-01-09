#include "commands.h"
#include <errno.h>
#include <signal.h>

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <pwd.h>

#include <readline/readline.h>

typedef struct{
	char user_name[32];
	char user_dir[64];
	char cwd[512];
	char prompt[600];
}context_t;
context_t context;

typedef int (*handle_command_func)(context_t *context,command_t *command);
typedef struct{
	char *command;
	handle_command_func handle;
} builtin_command;

int exit_handle(context_t *context,command_t *command);
int cd_handle(context_t *context,command_t *command);
int other_handle(context_t *context,command_t *command);

int commands_handle(context_t *context,char *command);

builtin_command builtins[] = {{"exit",exit_handle},{"cd",cd_handle}};
size_t size ;
size_t length ;

#define NUM_MAX_BACKEND_PROC 128
pid_t backend_proc_arr[NUM_MAX_BACKEND_PROC]={0};

void on_chldproc_exit(int sig);


int main(int c,char *v[]){
	memset(&context,0,sizeof(context_t));

	struct passwd *pwd;
	pwd = getpwuid(getuid());
	strncpy(context.user_name,pwd->pw_name,strlen(pwd->pw_name));
	strncpy(context.user_dir,pwd->pw_dir,strlen(pwd->pw_dir));

	getcwd(context.cwd,sizeof(context.cwd));

	strncpy(context.prompt,context.user_name,strlen(context.user_name));
	context.prompt[strlen(context.user_name)]='@';
	strncpy(context.prompt+(strlen(context.user_name)+1),context.cwd,strlen(context.cwd));
	context.prompt[strlen(context.prompt)]='$';
	context.prompt[strlen(context.prompt)]=' ';


	size = sizeof(builtin_command);
	length = sizeof(builtins) / size;

	if(signal(SIGCHLD,on_chldproc_exit) == SIG_ERR){
		perror("注册信号函数失败");
	}
	memset(backend_proc_arr,0,sizeof(backend_proc_arr));

	int flag = 0;
	while(!flag){
		char * my_command = read_commands(context.prompt);

		flag=commands_handle(&context,my_command);
		free(my_command);
	}
	//
	// destory var / resource
	return 0;
}

int exit_handle(context_t *context,command_t *command){
	return 1;
}
int cd_handle(context_t *context,command_t *command){
	char* target_dir = context->user_dir;
	if(command->args_length >2){
		target_dir = command->command_args[1];
	}
	int ret = chdir(target_dir);
	if(ret ){
		perror("");
		return 0;
	}

	memset(context->cwd,0,sizeof(context->cwd));
	memset(context->prompt,0,sizeof(context->prompt));
	getcwd(context->cwd,sizeof(context->cwd));

	strncpy(context->prompt,context->user_name,strlen(context->user_name));
	context->prompt[strlen(context->user_name)]='@';
	strncpy(context->prompt+(strlen(context->user_name)+1),context->cwd,strlen(context->cwd));
	context->prompt[strlen(context->prompt)]='$';
	context->prompt[strlen(context->prompt)]=' ';
	return 0;
}

pid_t exec_subprocess(context_t* context,command_t* user_command,int pipe_in,int pipe_out){
	pid_t pid = fork();
	if (pid<0){return pid;}
	else if (pid==0){
		if(user_command->flag_bits & COMMAND_FLAG_REDIRECT_IN ){
			int flag = open(user_command->in_file,O_RDONLY);
			if(flag){perror("stdin redirect ");return 0;}
			dup2(flag,0);
			close(flag);
		}
		if(user_command->flag_bits & COMMAND_FLAG_REDIRECT_OUT ){
			do{
				if(user_command->flag_bits&COMMAND_FLAG_REDIRECT_1MERGE2){
					dup2(2,1);
					break;
				}
				int open_bits = O_WRONLY|O_CREAT | O_TRUNC;
				if(user_command->flag_bits & COMMAND_FLAG_REDIRECT_OUT_APPEND){
					open_bits = open_bits | O_APPEND;
					open_bits = open_bits & (~O_TRUNC);
				}
				int flag = open(user_command->out_file,open_bits);
				if(flag==-1){perror("stdout redirect");return 0;}
				dup2(flag,1);
				close(flag);
			}while(0);
		}
		if(user_command->flag_bits & COMMAND_FLAG_REDIRECT_ERR ){
			do{
				if(user_command->flag_bits&COMMAND_FLAG_REDIRECT_2MERGE1){
					dup2(1,2);
					break;
				}
				int open_bits = O_WRONLY|O_CREAT | O_TRUNC;
				if(user_command->flag_bits & COMMAND_FLAG_REDIRECT_OUT_APPEND){
					open_bits = open_bits | O_APPEND;
					open_bits = open_bits & (~O_TRUNC);
				}
				int flag = open(user_command->err_file,open_bits);
				if(flag==-1){perror("stdout redirect");return 0;}
				dup2(flag,2);
				close(flag);
			}while(0);
		}
		if(user_command->flag_bits & COMMAND_FLAG_BACKEND){
			//TODO 
		}
		if(pipe_in !=-1){
			dup2(pipe_in,0);
			close(pipe_in);
		}
		if(pipe_out!=-1){
			dup2(pipe_out,1);
			close(pipe_out);
		}
		int ret = execvp(user_command->command_args[0],user_command->command_args);
		if(ret<0){perror("exec error");}
		return -1;		
	}
	return pid;
}
int other_handle(context_t *context,command_t *user_command){
	if(user_command->flag_bits & COMMAND_FLAG_PIPE){
		int pipes[2];
		int flag = pipe(pipes);
		if(flag == -1){perror("create pipe on ");return 0;}

	}	

	command_t* cmd = user_command;
	int pipes[2];
	int curr_subproc_in=-1;
	int curr_subproc_out=-1;
	while(cmd){
		if(curr_subproc_out!=-1){
			close(curr_subproc_out);
		}
		if(cmd->next_command){
			pipe(pipes);
			curr_subproc_out = pipes[1];
		}

		pid_t subproc_pid = exec_subprocess(context,cmd,curr_subproc_in,curr_subproc_out);
		if(curr_subproc_in!=-1){
			close(curr_subproc_in);
		}
		if(subproc_pid<0){
			close(pipes[0]);
			close(pipes[1]);
			break;
		}
		cmd->pid = subproc_pid;

		if(cmd->next_command){
			curr_subproc_in = pipes[0];
		}
		cmd=cmd->next_command;
	}
	for(command_t* ptr=user_command;
			ptr;
			ptr=ptr->next_command){
		if(ptr->flag_bits & COMMAND_FLAG_BACKEND){
			for(int i=0;i<NUM_MAX_BACKEND_PROC;i++){
				if(backend_proc_arr[i]!=0){continue;}
			}
		}else{
			waitpid(ptr->pid,NULL,0);
		}
	}
	return 0;
}

int commands_handle(context_t *context,char *command){
	command_t *user_command = parse_commands(command);

	int ret_val = 0;
	do{
		if(user_command->error_msg){
			printf("command has wrong:%s\n",user_command->error_msg);
			ret_val = 0;
			break;
		}
		handle_command_func handle_func = other_handle; 
		for(int i=0;i<length;i++){
			int ret = strcmp(builtins[i].command,user_command->command_args[0]);
			if(ret){continue;}
			handle_func = builtins[i].handle;
			break;
		}
		ret_val = handle_func(context,user_command);
	}while(0);

	destory_commands(user_command);
	return ret_val;
}

void on_chldproc_exit(int sig){
	pid_t pid;
	for(int i=0;i<NUM_MAX_BACKEND_PROC;i++){
		if(backend_proc_arr[i]==0){continue;}
		
		pid = waitpid(backend_proc_arr[i],NULL,WNOHANG);
		if(pid>0){
			printf("child proc %d has exited",pid);
			backend_proc_arr[i]=0;
		}else if(pid < 0){
			switch(errno){
				case ECHILD:
					perror("waitpid");
					break;
				default:
					perror("waitpid other");
			}
		}
	}
}
