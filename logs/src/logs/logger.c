
#include "logs/logger.h"
#include <stdio.h>
#include <string.h>



logger_appender_t* new_appender(char* dir,char* name,log_level_t filter_lvl,int log_modifier){
	return NULL;}

void distroy_appender(logger_appender_t* appender_ptr){

}

void append(logger_appender_t* ptr,const char* modifier ,const char* content){
fwrite(modifier,strlen(modifier),1,ptr->out_stream);
fwrite(content,strlen(content),1,ptr->out_stream);
}

void archive_appender(){}
void open_appender(){}


