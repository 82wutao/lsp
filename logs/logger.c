#include "logger.h"
#include <stdio.h>
#include <string.h>



logger_appender_t* new_appender(){return NULL;}
void distroy_appender(){}

void append(logger_appender_t* ptr,const char* modifier ,const char* content){
fwrite(modifier,strlen(modifier),1,ptr->out_stream);
fwrite(content,strlen(content),1,ptr->out_stream);
}

void archive_appender(){}
void open_appender(){}


