
#ifndef __LOGGER__H
#define __LOGGER__H

#include <stdio.h>
#include <pthread.h>

#include "datetimeutil.h"
typedef enum {Debug,Info,Warn,Error} log_level_t;

#define LOG_MODIFIER_DATE 1
#define LOG_MODIFIER_TIME 2
#define LOG_MODIFIER_MILLIS 4
#define LOG_MODIFIER_LEVEL 8
#define LOG_MODIFIER_THREAD 16
#define LOG_MODIFIER_FILE 32
#define LOG_MODIFIER_FUNC 64
#define LOG_MODIFIER_LINE 128

typedef struct {
	int modifier;
	log_level_t lvl;
	int current_starttime[7];

	FILE* out_stream;

	char* dir_name;
	char* log_name;

} logger_appender_t;


logger_appender_t* new_appender(char* dir,char* name,log_level_t filter_lvl,int log_modifier);
void distroy_appender(logger_appender_t* appender_ptr);

void append(logger_appender_t* ptr,const char* modifier ,const char* content);

void archive_appender();
void open_appender();

#define __APPEND(log_lvl,fmt,args... ) \
{ \
int time_buff[7]; \
now_with_millis(time_buff); \
char current_thread_name[16]; \
pthread_t current_thread = pthread_self(); \
pthread_getname_np(current_thread,current_thread_name,16); \
char modifier_buff[256]; \
char content_buff[256]; \
int len = snprintf(modifier_buff,255,"%d-%d-%d %d:%d:%d.%d [ %s ] %s %d [t_id:%lu] [t_name:%s] ", \
time_buff[0],time_buff[1],time_buff[2],time_buff[3],time_buff[4],time_buff[5],time_buff[6], \
#log_lvl,__FILE__,__LINE__,current_thread,current_thread_name); \
len =snprintf(content_buff,255,fmt,args); \
append(&appender,modifier_buff,content_buff); \
 \
}

#define LOG_DEBUG(appender,fmt,args...) \
{ \
	if(appender.lvl <= Debug){ \
		__APPEND(D,fmt,args) \
	} \
}
#define LOG_INFO(appender,fmt,args...) \
{ \
	if(appender.lvl <= Info){ \
		__APPEND(I,fmt,args) \
	} \
}
#define LOG_WARN(appender,fmt,args...) \
{ \
	if(appender.lvl <= Warn){ \
		__APPEND(W,fmt,args) \
	} \
}
#define LOG_ERROR(appender,fmt,args...) \
{ \
	if(appender.lvl <= Error){ \
		__APPEND(E,fmt,args) \
	} \
}

#endif //__LOGGER_t char *format, ...H
