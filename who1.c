#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <utmp.h>
#include <fcntl.h>
#include <time.h>

void show_sys_time(struct timeval *);
void show_user_info(struct utmp *);

int main(int c,char *v[]){
		printf("ggggggggggg\n");
	struct utmp utbuf;
	int utmpfd;

	if((utmpfd = open(UTMP_FILE,O_RDONLY))<0){
		printf("world\n");
		perror(UTMP_FILE);
		exit(1);
	}
	size_t len ;
	for(len = read(utmpfd,&utbuf,sizeof(utbuf));
			len>0;
			len=read(utmpfd,&utbuf,sizeof(utbuf))){
		printf("hello\n");
	show_user_info(&utbuf);

	}
	close(utmpfd);
	return 0;
}


void show_sys_time(struct timeval *val_p){
	struct tm t;
char date_time[64];
strftime(date_time,sizeof(date_time),
"%Y-%m-%d %H:%M:%S",localtime_r(val_p->tv_sec,&t));


	printf("% -8.8s",date_time);
}
void show_user_info(struct utmp *buf_p){
	if(buf_p->ut_type !=USER_PROCESS){
		return ;
	}

	printf("% -8.8s",buf_p->ut_user);
	printf(" ");
	printf("% -8.8s",buf_p->ut_line);
	printf(" ");
	show_sys_time(&(buf_p->ut_tv));
	printf("% -8.8s",buf_p->ut_host);
	printf("\n");

}
