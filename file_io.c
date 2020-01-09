#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>

#include <fcntl.h>
#include <unistd.h>



#define MSG_BUFF_LENG 128

typedef void (*on_error)(const char* err_msg,void *context) ;

void error_check(int int_ret,on_error handler,void *context){
	if(int_ret>=0){
		return ;
	}
	char msg_buff[MSG_BUFF_LENG];
	strerror_r(errno,msg_buff,MSG_BUFF_LENG-1);

	handler(msg_buff,context);
}

void sleep_seconds(unsigned int s);
void error_print_exit(const char* err_msg,void *context);

ssize_t write_file(int fd,char *buff,size_t len,bool sync);
ssize_t read_file(int fd,char *buff,size_t len);
ssize_t write_file_at(int fd,char *buff,size_t len,off_t offset,bool sync);
ssize_t read_file_at(int fd,char *buff,size_t len,off_t offset);

off_t move_to_EOF(int fd);
off_t position_of_current(int fd);
off_t move_from_current(int fd,off_t offset);
off_t mark_as_current(int fd,off_t position);


typedef	void (*multiplexing_event_callback)(int fd) ;
typedef struct{
	int *read_listeners;
	int read_listeners_len;
	multiplexing_event_callback on_readable;

	int *write_listeners;
	int write_listeners_len;
		multiplexing_event_callback on_writeable;

	int *ex_ofb_listeners;
	int ex_ofb_listeners_len;
	multiplexing_event_callback on_ex_or_ofb;

	long timeout_sec; /* seconds */
	long timeout_usec; /* microseconds */
} listen_fileset;
int multiplexing_select(listen_fileset *set_ptr);

void test_read(int fd){
	printf("found a file %d writeable\n",fd);
	char buff[64];
	int ret=read_file(fd,buff,64);
	error_check(fd,error_print_exit,"a error found while read");	
	printf("read %d bytes\n",ret);
}
int main(int c,char *v[]){

	int fd = open("./f1.c",O_RDWR|O_CREAT|O_NONBLOCK//|O_APPEND
			,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	error_check(fd,error_print_exit,"a error found while open");	

	int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags|O_NONBLOCK);
	flags = fcntl(STDOUT_FILENO, F_GETFL, 0);
	fcntl(STDOUT_FILENO, F_SETFL, flags|O_NONBLOCK);

	int reads[2]={fd,STDOUT_FILENO};
printf("debug 1\n");
	listen_fileset listeners;
	listeners.read_listeners=NULL;
	listeners.read_listeners_len=0;
	listeners.on_readable=NULL;
	listeners.write_listeners=reads;
	listeners.write_listeners_len=2;
	listeners.on_writeable=test_read;
	listeners.ex_ofb_listeners=NULL;
	listeners.ex_ofb_listeners_len=0;
	listeners.on_ex_or_ofb=NULL;
	listeners.timeout_sec=3;
	listeners.timeout_usec=3;
	
printf("debug 2\n");
	for(int i=0;i<4;i++){
printf("debug 2.1\n");
		int ret = multiplexing_select(&listeners);
printf("debug 2.2\n");
		error_check(ret,error_print_exit,"an error found while select");
		printf("active %d fds\n",ret);
printf("debug 2.3\n");
	}
	
printf("debug 3\n");
	int ret = close(fd);
	error_check(ret,error_print_exit,"an error found while close");
	
printf("debug 4\n");
	ret = multiplexing_select(&listeners);
	error_check(ret,error_print_exit,"an error found while select 2");

	return 0;

}

void error_print_exit(const char* err_msg,void *context){
	char * msg_pre = (char*)context;
	printf("%s: %s\n",msg_pre,err_msg);
	//perror("found an error:");
	exit(1);
}
ssize_t write_file(int fd,char *buff,size_t len,bool sync){
	ssize_t size_writed=0;
	for(ssize_t writed = write(fd,buff,len);writed;writed = write(fd,buff,len) ){
		if(writed ==-1){
			if(errno == EAGAIN){continue;}
			if(errno == EINTR){continue;}
			return -1;
		}
		size_writed += writed;
		buff += writed;
		len -=writed;
		if(len ==0){
			break;
		}
	}
	if(sync){fsync(fd);}
	return size_writed;
}
ssize_t read_file(int fd,char *buff,size_t len){
	ssize_t size_readed=0;
	for(ssize_t readed = read(fd,buff,len);readed;readed = read(fd,buff,len) ){
		if(readed ==-1){
			if(errno == EAGAIN){continue;}
			if(errno == EINTR){continue;}
			return -1;
		}
		size_readed += readed;
		buff += readed;
		len -=readed;
		if(len ==0){
			break;
		}
	}
	return size_readed;
}

ssize_t write_file_at(int fd,char *buff,size_t len,off_t offset,bool sync){
	ssize_t size_writed=0;
	for(ssize_t writed = pwrite(fd,buff,len,offset);writed;writed = pwrite(fd,buff,len,offset) ){
		if(writed ==-1){
			if(errno == EAGAIN){continue;}
			if(errno == EINTR){continue;}
			return -1;
		}
		size_writed += writed;
		buff += writed;
		len -=writed;
		offset +=writed;
		if(len ==0){
			break;
		}
	}
	if(sync){fsync(fd);}
	return size_writed;
}
ssize_t read_file_at(int fd,char *buff,size_t len,off_t offset){
	ssize_t size_readed=0;
	for(ssize_t readed = pread(fd,buff,len,offset);readed;readed = pread(fd,buff,len,offset) ){
		if(readed ==-1){
			if(errno == EAGAIN){continue;}
			if(errno == EINTR){continue;}
			return -1;
		}
		size_readed += readed;
		buff += readed;
		len -=readed;
		offset +=readed;
		if(len ==0){
			break;
		}
	}
	return size_readed;
}
void sleep_seconds(unsigned int s){
	for(unsigned int ret = sleep(s);ret;ret=sleep(ret));
}


off_t move_to_EOF(int fd){
	return lseek(fd,0,SEEK_END);
}
off_t position_of_current(int fd){
	return lseek(fd,0,SEEK_CUR);
}
off_t move_from_current(int fd,off_t offset){
	return lseek(fd,offset,SEEK_CUR);
}
off_t mark_as_current(int fd,off_t position){
	return lseek(fd,position,SEEK_SET);
}


int multiplexing_select(listen_fileset *set_ptr){
printf("debug select.1\n");
	int ret;
	while(1){
printf("debug select.2\n");
		int max_fd=0;
		fd_set read_availables;
		FD_ZERO(&read_availables);
		fd_set write_availables;
		FD_ZERO(&write_availables);
		fd_set ex_ofb_availables;
		FD_ZERO(&ex_ofb_availables);
printf("debug select.3\n");

		for(int i=0;i<set_ptr->read_listeners_len;i++){
			int fd = set_ptr->read_listeners[i];
			FD_SET(fd,&read_availables);
			if(max_fd<fd){max_fd=fd;}
		}	
printf("debug select.4\n");
		for(int i=0;i<set_ptr->write_listeners_len;i++){
			int fd = set_ptr->write_listeners[i];
			FD_SET(fd,&write_availables);
			if(max_fd<fd){max_fd=fd;}
		}	
printf("debug select.5\n");
		for(int i=0;i<set_ptr->ex_ofb_listeners_len;i++){
			int fd = set_ptr->ex_ofb_listeners[i];
			FD_SET(fd,&ex_ofb_availables);
			if(max_fd<fd){max_fd=fd;}
		}
printf("debug select.6\n");


		struct timeval timeout = {set_ptr->timeout_sec,set_ptr->timeout_usec};
		ret = select(max_fd,
				&read_availables,&write_availables,
				&ex_ofb_availables,&timeout);
		if(ret <0){
printf("debug select.7.1\n");
			if( errno == EINTR){
printf("debug select.7.2\n");
				continue;		
			}
printf("debug select.7.3\n");
			return ret;
		}
printf("debug select.8\n");
		for(int i=0;i<set_ptr->read_listeners_len;i++){
			int fd = set_ptr->read_listeners[i];
			if(FD_ISSET(fd,&read_availables)){set_ptr->on_readable(fd);}
		}
printf("debug select.9\n");
		for(int i=0;i<set_ptr->write_listeners_len;i++){
			int fd = set_ptr->write_listeners[i];
			if(FD_ISSET(fd,&write_availables)){set_ptr->on_writeable(fd);}
		}
printf("debug select.10\n");
		for(int i=0;i<set_ptr->ex_ofb_listeners_len;i++){
			int fd = set_ptr->ex_ofb_listeners[i];
			if(FD_ISSET(fd,&ex_ofb_availables)){set_ptr->on_ex_or_ofb(fd);}
		}
printf("debug select.11\n");
		break;
	}
printf("debug select.12\n");
	return ret;
}









