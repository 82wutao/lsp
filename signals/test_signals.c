#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void sig_interupt_handler(){
	printf(" interupt_handler entered,rcved signal $sig ,and waiting....\n");
	sleep(2);
	printf(" interupt_handler leaving\n");
}
void sig_quit_action(int sig,siginfo_t* info_ptr,void* context_ptr){
	printf(" quit_handler entered,rcved signal ,and waiting....\n");
	sleep(2);
	printf(" quit_handler leaving\n");
}
int main(int c,char* v[]){
	
	int buffer_len = 100;
	char buffer[buffer_len];
/*
	signal(SIGINT,sig_interupt_handler);
	signal(SIGQUIT,sig_quit_handler);
*/
	struct sigaction interupt_action;
	struct sigaction quit_action;

	interupt_action.sa_handler= sig_interupt_handler;
	interupt_action.sa_flags = SA_RESTART | SA_NODEFER ;
	sigset_t sig_blocked;
	sigemptyset(&sig_blocked);
	sigaddset(&sig_blocked,SIGQUIT);
	interupt_action.sa_mask = sig_blocked;
	sigaction(SIGINT,&interupt_action,NULL);

	quit_action.sa_sigaction= sig_quit_action;
	quit_action.sa_flags = SA_RESTART | SA_NODEFER |SA_SIGINFO;
	sigemptyset(&sig_blocked);
	sigaddset(&sig_blocked,SIGINT);
	quit_action.sa_mask = sig_blocked;
	sigaction(SIGQUIT,&quit_action,NULL);

	int readed = 0;
	do{
		printf("\n now ,you can type some words\n");
		readed = read(0,buffer,buffer_len-1);
		if(readed ==-1){
			perror("while read");
		}else{
			buffer[readed]='\0';
			printf("you typed %s\n",buffer);
		}
	}while(strncmp("quit",buffer,readed-1));
	return 0;
}
