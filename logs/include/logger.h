#ifndef __LOGGER__H
#define __LOGGER__H

#include <stdio.h>
#include <pthread.h>

#include "datetimeutil.h"
typedef enum {Debug,Info,Warn,Error} log_level_t;

typedef struct {
	FILE* out_stream;
	log_level_t lvl;
} logger_appender_t;


logger_appender_t* new_appender();
void distroy_appender();

void append(logger_appender_t* ptr,const char* modifier ,const char* content);

void archive_appender();
void open_appender();

#define MODIFIER_CONTENT(modifier_buff,content_buff, log_lvl,fmt,args... ) \
{ \
int time_buff[7]; \
now(time_buff); \
char current_thread_name[16]; \
pthread_t current_thread = pthread_self(); \
pthread_getname_np(current_thread,current_thread_name,16); \
int len = snprintf(modifier_buff,255,"%d-%d-%d %d:%d:%d.%d [ %s ] %s %d [t_id:%lu] [t_name:%s] ", \
time_buff[0],time_buff[1],time_buff[2],time_buff[3],time_buff[4],time_buff[5],time_buff[6], \
#log_lvl,__FILE__,__LINE__,current_thread,current_thread_name); \
len =snprintf(content_buff,255,fmt,args); \
}

#define LOG_DEBUG(appender,fmt,args...) \
{ \
	while(appender.lvl <= Debug){ \
		char modifier_buff[256]; \
		char content_buff[256]; \
		MODIFIER_CONTENT(modifier_buff,content_buff,D,fmt,args) \
append(&appender,modifier_buff,content_buff); \
break; \
	} \
}
#define LOG_INFO(appender,fmt,args...) \
{ \
	while(appender.lvl <= Info){ \
		char modifier_buff[256]; \
		char content_buff[256]; \
		MODIFIER_CONTENT(modifier_buff,content_buff,I,fmt,args) \
append(&appender,modifier_buff,content_buff); \
break; \
	} \
}
#define LOG_WARN(appender,fmt,args...) \
{ \
	while(appender.lvl <= Warn){ \
		char modifier_buff[256]; \
		char content_buff[256]; \
		MODIFIER_CONTENT(modifier_buff,content_buff,W,fmt,args) \
append(&appender,modifier_buff,content_buff); \
break; \
	} \
}
#define LOG_ERROR(appender,fmt,args...) \
{ \
	while(appender.lvl <= Error){ \
		char modifier_buff[256]; \
		char content_buff[256]; \
		MODIFIER_CONTENT(modifier_buff,content_buff,E,fmt,args) \
append(&appender,modifier_buff,content_buff); \
break; \
	} \
}

#endif //__LOGGER_t char *format, ...H
