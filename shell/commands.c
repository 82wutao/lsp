
#include "commands.h"
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <readline/readline.h>

char *ERROR_MSG_ANDATBEGIN = "char '&' can not hanle ";

command_t* new_command(char *raw_str,int offset,int length);
//===============================
int COMMAND_FLAG_BACKEND = 1<<0;
int COMMAND_FLAG_REDIRECT_IN = 1<<1;
int COMMAND_FLAG_REDIRECT_OUT = 1<<2;
int COMMAND_FLAG_REDIRECT_OUT_APPEND = 1<<3;
int COMMAND_FLAG_REDIRECT_ERR = 1<<4;
int COMMAND_FLAG_REDIRECT_ERR_APPEND = 1<<5;
int COMMAND_FLAG_PIPE = 1<<6;
int  COMMAND_FLAG_REDIRECT_1MERGE2 =1<<7;
int  COMMAND_FLAG_REDIRECT_2MERGE1 =1<<8;

char* MSG_ERR_MAXARGS = "args max num of command  is 256";

char * read_commands(const char* prompt){
	char *cmd = readline(prompt);
	return cmd;
}


command_t* parse_commands(char *command_str){
	command_t *ret=NULL;
	command_t *current=NULL;

	int pos=0;
	int offset = pos;
	while(1){
		int length =0;

		char c=	command_str[pos];
		if(c!='|' && c!='\0'){
			pos++;
			continue;
		}
		length = pos-offset;
		command_t *cmd=new_command(command_str,offset,length);
		if(current==NULL){
			ret = cmd;
			current = ret;
		}else{
			current->next_command = cmd;
			current = cmd;
		}
		offset = pos +1;

		pos++;
		if(c=='\0'){
			break;
		}
	}

	if(current->flag_bits & COMMAND_FLAG_BACKEND == COMMAND_FLAG_BACKEND){
		for(command_t *ptr= ret;ptr;ptr=ptr->next_command){
			ptr->flag_bits = ptr->flag_bits | COMMAND_FLAG_BACKEND;
		}		
	}
	return ret;
}

int destory_commands(command_t *command){
	command_t *next;
	for(command_t *ptr= command;ptr;ptr=next){
		if(ptr->error_msg){
			free(ptr);
			break;
		}

		for(int i=0;i<ptr->args_length-1;i++){
			free(ptr->command_args[i]);
		}
		if(ptr->flag_bits & COMMAND_FLAG_REDIRECT_IN){
			free(ptr->in_file);}	
		if(ptr->flag_bits & COMMAND_FLAG_REDIRECT_OUT 
				&& ptr->out_file !=NULL){free(ptr->out_file); }
		if(ptr->flag_bits & COMMAND_FLAG_REDIRECT_ERR 
				&& ptr->err_file !=NULL){free(ptr->err_file); }

		next=ptr->next_command;
		free(ptr);
	}
	return 0;
}
//======================================
static int command_arg_append(command_t* command,char* raw_str,int char_begin,int char_end){
	if(command->args_length == 255){
		command->error_msg= MSG_ERR_MAXARGS;
		return -1;
	}
	char *arg =malloc(char_end - char_begin + 1);
	memset(arg,0,char_end - char_begin + 1);
	strncpy(arg,raw_str+char_begin,char_end - char_begin);
	command->command_args[command->args_length] = arg;
	command->args_length=command->args_length + 1;
	return 0;
}
static int command_redirect(command_t* command,char* raw_str,int char_begin,int char_end,
		int std_file_no,int check_offset){
	int flag_redirect = std_file_no == 1?COMMAND_FLAG_REDIRECT_OUT:COMMAND_FLAG_REDIRECT_ERR;
	int flag_redirect_append = std_file_no == 1?COMMAND_FLAG_REDIRECT_OUT_APPEND:COMMAND_FLAG_REDIRECT_ERR_APPEND;
	int flag_redirect_merge =  std_file_no == 1?COMMAND_FLAG_REDIRECT_1MERGE2:COMMAND_FLAG_REDIRECT_2MERGE1;
	char redirect_merge_file_no = std_file_no == 1?'2':'1';
	char **command_std_file_ptr = std_file_no == 1?&(command->out_file):&(command->err_file);

	command->flag_bits = command->flag_bits | flag_redirect;
					
	if (raw_str[char_begin+ check_offset ]=='&' &&
			raw_str[char_begin+ check_offset + 1]==redirect_merge_file_no){
		command->flag_bits = command->flag_bits | flag_redirect_merge;
	}else if(raw_str[char_begin+ check_offset]=='>'){
		command->flag_bits = command->flag_bits | flag_redirect_append;

		char_begin = char_begin+ check_offset+1;
	}else {
		char_begin = char_begin + check_offset;
	}
	char* _temp_file = calloc(1,(size_t)(char_end - char_begin+1));
	strncpy(_temp_file,raw_str+char_begin,char_end - char_begin);
	*command_std_file_ptr = _temp_file;
	return char_begin;
}
static char buffer[256];
command_t* new_command(char *raw_str,int offset,int length){
	void *mem = malloc(sizeof(command_t));
	if(mem==NULL){return NULL;}
	memset(mem,0,sizeof(command_t));
	command_t *ret = (command_t*)mem;

	int char_begin = -1;
	int char_end = char_begin;

	for(int i=0;i<=length;i++){
		char c = raw_str[offset+i];

		if(c!=' ' && c!='\0' && c!='|' && char_begin == -1){char_begin = offset + i;}
		else if (c==' ' || c =='\0' || c=='|'){
			if(char_begin==-1){continue;}

			memset(buffer,0,256);
			char_end = offset + i;

			if(raw_str[char_begin] == '0'){
				if(raw_str[char_begin+1]!='<'){
					int flag = command_arg_append(ret,raw_str,char_begin,char_end);
					if(flag){return ret;}
				}else{
					ret->flag_bits = ret->flag_bits | COMMAND_FLAG_REDIRECT_IN;
					char* _in_file = calloc(1,(size_t)(char_end - char_begin+1));
					strncpy(_in_file,raw_str+char_begin,char_end - char_begin);
					ret->in_file = _in_file;
				}
			}else if(raw_str[char_begin] == '<'){
				ret->flag_bits = ret->flag_bits | COMMAND_FLAG_REDIRECT_IN;
				char* _in_file = calloc(1,(size_t)(char_end - char_begin+1));
				strncpy(_in_file,raw_str+char_begin,char_end - char_begin);
				ret->in_file = _in_file;
			}else if(raw_str[char_begin] == '1'){
				if(raw_str[char_begin+1]!='>'){
					int flag = command_arg_append(ret,raw_str,char_begin,char_end);
					if(flag){return ret;}
				}else{
					char_begin = command_redirect(ret,raw_str,char_begin,char_end,1,2);
				}
			}else if(raw_str[char_begin] == '>'){
					char_begin = command_redirect(ret,raw_str,char_begin,char_end,1,1);
			}else if(raw_str[char_begin] == '2'){
				if(raw_str[char_begin+1]!='>'){
					int flag = command_arg_append(ret,raw_str,char_begin,char_end);
					if(flag){return ret;}
				}else{
					char_begin = command_redirect(ret,raw_str,char_begin,char_end,2,2);
				}
			}else if(raw_str[char_begin] == '&'){
				ret->flag_bits = ret->flag_bits | COMMAND_FLAG_BACKEND;
			} else{
				int flag = command_arg_append(ret,raw_str,char_begin,char_end);
				if(flag){return ret;}
			}

			char_begin = -1;
			if(c=='|'){ret->flag_bits = ret->flag_bits | COMMAND_FLAG_PIPE;}
		}else{
		}
	}
	ret->command_args[ret->args_length]=NULL;
	ret->args_length=ret->args_length +1;
	
	return ret;
}
