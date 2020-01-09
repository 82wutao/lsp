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
	char buff[64];
	int ret=read_file(fd,buff,64);
	error_check(fd,error_print_exit,"a error found while read");	
	printf("read %d bytes\n",ret);
}


int multiplexing_create(listen_fileset *set_ptr,
		multiplexing_event_callback on_newconnection,
		multiplexing_event_callback on_readable,
		multiplexing_event_callback on_writeable,
		multiplexing_event_callback on_ex_or_ob,
		multiplexing_event_callback on_closed);

int multiplexing_ctl(listen_fileset *set_ptr,int op,int sock,int event);

int multiplexing_select(listen_fileset *set_ptr){

	int ret;
	while(1){
		int max_fd=0;
		fd_set read_availables;
		FD_ZERO(&read_availables);
		fd_set write_availables;
		FD_ZERO(&write_availables);
		fd_set ex_ofb_availables;
		FD_ZERO(&ex_ofb_availables);

		for(int i=0;i<set_ptr->read_listeners_len;i++){
			int fd = set_ptr->read_listeners[i];
			FD_SET(fd,&read_availables);
			if(max_fd<fd){max_fd=fd;}
		}	
		for(int i=0;i<set_ptr->write_listeners_len;i++){
			int fd = set_ptr->write_listeners[i];
			FD_SET(fd,&write_availables);
			if(max_fd<fd){max_fd=fd;}
		}	
		for(int i=0;i<set_ptr->ex_ofb_listeners_len;i++){
			int fd = set_ptr->ex_ofb_listeners[i];
			FD_SET(fd,&ex_ofb_availables);
			if(max_fd<fd){max_fd=fd;}
		}

		struct timeval timeout = {set_ptr->timeout_sec,set_ptr->timeout_usec};
		ret = select(max_fd,
				&read_availables,&write_availables,
				&ex_ofb_availables,&timeout);
		if(ret <0){
			if( errno == EINTR){
				continue;		
			}
			return ret;
		}

		for(int i=0;i<set_ptr->read_listeners_len;i++){
			int fd = set_ptr->read_listeners[i];
			if(FD_ISSET(fd,&read_availables)){set_ptr->on_readable(fd);}
		}
		for(int i=0;i<set_ptr->write_listeners_len;i++){
			int fd = set_ptr->write_listeners[i];
			if(FD_ISSET(fd,&read_availables)){set_ptr->on_writeable(fd);}
		}
		for(int i=0;i<set_ptr->ex_ofb_listeners_len;i++){
			int fd = set_ptr->ex_ofb_listeners[i];
			if(FD_ISSET(fd,&ex_ofb_availables)){set_ptr->on_ex_or_ofb(fd);}
		}
		break;
	}
	return ret;
}

