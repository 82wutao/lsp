
#include "logs/logger.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>




int appender_init(logger_appender_t* appender_ptr, char* dir,char* name,log_level_t filter_lvl,int log_modifier) {
    if(!appender_ptr) {
        return 0;
    }
    do{
        appender_ptr->dir_name = NULL;
        appender_ptr->log_name = NULL;
        appender_ptr->out_stream = NULL;

        if(!dir && !name){
            break;
        }

        if(!dir){dir=".";}
        if(!name){name="app";}
        appender_ptr->dir_name = calloc(strlen(dir)+1,1);
        if(!(appender_ptr->dir_name)){break;}
        appender_ptr->log_name = calloc(strlen(name)+1,1);
        if(!(appender_ptr->log_name)){break;}

        memcpy(appender_ptr->dir_name,dir,strlen(dir)+1);
        memcpy (appender_ptr->log_name,name,strlen(name)+1 );
    }while(0);

    appender_ptr->modifier=log_modifier;
    appender_ptr->lvl= filter_lvl;

    now(&(appender_ptr->current_starttime));
    return 0;
}

logger_appender_t* appender_new(char* dir,char* name,log_level_t filter_lvl,int log_modifier){
    size_t size = sizeof(logger_appender_t);
    logger_appender_t* appender_ptr = calloc(size,1);
    if(!appender_ptr){return NULL;}

    appender_init(appender_ptr,dir,name,filter_lvl,log_modifier);
    return appender_ptr;
}

void appender_destroy(logger_appender_t* appender_ptr){
    if(!appender_ptr){
        return ;
    }

    if(appender_ptr->out_stream != stdout){
        fclose(appender_ptr->out_stream);
    }

    if(appender_ptr->dir_name){ free(appender_ptr->dir_name);}
    if(appender_ptr->log_name){ free(appender_ptr->log_name);}

    free(appender_ptr);
}

void appender_append(logger_appender_t* appender_ptr,const char* modifier,const char* content) {
    if(!(appender_ptr) || !(appender_ptr->out_stream)) {
        return;
    }

    if(strlen(modifier)>0) {
        fwrite(modifier,strlen(modifier),1,appender_ptr->out_stream);
    }
    fwrite(content,strlen(content),1,appender_ptr->out_stream);
}

void appender_archive(logger_appender_t* appender_ptr){
    if(!(appender_ptr) || !(appender_ptr->out_stream) || appender_ptr->out_stream== stdout){
        return ;
    }

    fclose(appender_ptr->out_stream);
    appender_ptr->out_stream=NULL;

    char logfile_path[128];
    snprintf(logfile_path,127,"%s/%s.log",appender_ptr->dir_name,appender_ptr->log_name);
    char archivefile_path[128];
    snprintf(archivefile_path,127,"%s/%s%04d%02d%02d.log",appender_ptr->dir_name,appender_ptr->log_name,
             appender_ptr->current_starttime.year,appender_ptr->current_starttime.month,appender_ptr->current_starttime.day_month);

    rename(logfile_path, archivefile_path);
}
void appende_open(logger_appender_t* appender_ptr){
    if(!appender_ptr){return ;}
    if(appender_ptr->out_stream){return ;}

    if(!(appender_ptr->dir_name) && !(appender_ptr->log_name)){
        appender_ptr->out_stream = stdout;
        return ;
    }

        char file_path[128];
        snprintf(file_path,127,"%s/%s.log",appender_ptr->dir_name,appender_ptr->log_name);

        FILE* out = fopen(file_path, "a");
        if(!out){return ;}
        appender_ptr->out_stream = out;
}
