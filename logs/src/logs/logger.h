
#ifndef __LOGGER__H
#define __LOGGER__H

#include <stdio.h>
#include <pthread.h>

#include "datetime/datetimeutil.h"
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
    timestamp_t current_starttime;

    FILE* out_stream;

    char* dir_name;
    char* log_name;
} logger_appender_t;


int appender_init(logger_appender_t* appender_ptr, char* dir,char* name,log_level_t filter_lvl,int log_modifier);
logger_appender_t* appender_new(char* dir,char* name,log_level_t filter_lvl,int log_modifier);

void appender_destroy(logger_appender_t* appender_ptr);

void appender_append(logger_appender_t* appender_ptr,const char* modifier,const char* content);

void appender_archive(logger_appender_t* appender_ptr);
void appende_open(logger_appender_t* appender_ptr);

#define __APPEND(log_lvl,fmt,args... ) \
{ \
timestamp_t time_buff; \
now_with_millis(&time_buff); \
char modifier_buff[256]; \
int len= 0; \
if(appender.modifier & LOG_MODIFIER_DATE){len +=snprintf(modifier_buff+len,255-len,"%04d-%02d-%02d ",time_buff.year,time_buff.month,time_buff.day_month);} \
if(appender.modifier & LOG_MODIFIER_TIME){len +=snprintf(modifier_buff+len,255-len,"%02d:%02d:%02d ",time_buff.hour,time_buff.minute,time_buff.second);} \
if(appender.modifier & LOG_MODIFIER_MILLIS){len = (len==0?len:len-1);len +=snprintf(modifier_buff+len,255-len,".%03d ",time_buff.millis);} \
if(appender.modifier & LOG_MODIFIER_LEVEL){len +=snprintf(modifier_buff+len,255-len,"[%s] ",#log_lvl);} \
if(appender.modifier & LOG_MODIFIER_FILE){len +=snprintf(modifier_buff+len,255-len,"[%s] ",__FILE__);} \
if(appender.modifier & LOG_MODIFIER_FUNC){len +=snprintf(modifier_buff+len,255-len,"[%s] ",__FUNCTION__);} \
if(appender.modifier & LOG_MODIFIER_LINE){len +=snprintf(modifier_buff+len,255-len,"[%d] ",__LINE__);} \
if(appender.modifier & LOG_MODIFIER_THREAD){ \
    char current_thread_name[16]; \
    pthread_t current_thread = pthread_self(); \
    pthread_getname_np(current_thread,current_thread_name,16); \
    len +=snprintf(modifier_buff+len,255-len,"[%s] ",current_thread_name); \
} \
char content_buff[256]; \
snprintf(content_buff,255,fmt,args); \
appender_append(&appender,modifier_buff,content_buff); \
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
