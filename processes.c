#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <stdio.h>
#include <string.h>


int main(int c,char *v[]){
	pid_t ret=	fork();
	if(ret ==-1)
		perror("fork ");
	else if(!ret){
		ret = execl("/usr/bin/ls","ls","./",NULL);
		if(ret ==-1){
			perror("execl");
			printf("child  over\n");
		}
	}
	int status;
	wait(&status);
	printf("child  end\n");
	return 0;
}
