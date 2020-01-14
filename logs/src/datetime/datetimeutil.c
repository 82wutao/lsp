#include "datetime/datetimeutil.h"

void now(timestamp_t* buffer){
    if ( !buffer ) {
        return;
    }
    time_t datetime;
    struct tm *p;

    time ( &datetime );
    p = localtime ( &datetime );

    buffer->year = 1900+ p->tm_year;
    buffer->month = p->tm_mon+1;
    buffer->day_month=	p->tm_mday;
    buffer->day_year=p->tm_yday;
    buffer->hour=p->tm_hour;
    buffer->minute=p->tm_min;
    buffer->second	=p->tm_sec;
    buffer->millis=0;
}
void now_with_millis (timestamp_t* buffer ) {
    if ( !buffer ) {
        return;
    }

    time_t datetime;
    struct tm *p;

    struct timeval sms_v;
    time ( &datetime );
    p = localtime ( &datetime );

    gettimeofday ( &sms_v,NULL );
    
    buffer->year = 1900+ p->tm_year;
    buffer->month = p->tm_mon+1;
    buffer->day_month=	p->tm_mday;
    buffer->day_year=p->tm_yday;
    buffer->hour=p->tm_hour;
    buffer->minute=p->tm_min;
    buffer->second	=p->tm_sec;
    buffer->millis=sms_v.tv_usec/1000;
}

unsigned long millis_between(timestamp_t* buffer1,timestamp_t* buffer2){
    unsigned long ms1=0;
    unsigned long ms2=0;
    if(buffer1){ms1= buffer1->millis;}
    if(buffer2){ms2= buffer2->millis;}
    
    unsigned long diff = seconds_between(buffer1,buffer2);
    return diff* 1000 + ms1 - ms2;
}
unsigned long seconds_between(timestamp_t* buffer1,timestamp_t* buffer2){
    unsigned long s1=0;
    unsigned long s2=0;
    if(buffer1){s1=buffer1->second;}
    if(buffer2){s2=buffer2->second;}
    
    int diff = minutes_between(buffer1,buffer2);
    return diff * 60 + s1 - s2;
}
int minutes_between(timestamp_t* buffer1,timestamp_t* buffer2){
    int m1=0;
    int m2=0;
    if(buffer1){m1=buffer1->minute;}
    if(buffer2){m2=buffer2->minute;}
    
    int diff = hours_between(buffer1,buffer2);
    return (diff * 60) + m1 - m2;
}
int hours_between(timestamp_t* buffer1,timestamp_t* buffer2){
    int h1=0;
    int h2=0;
    if(buffer1){h1=buffer1->hour;}
    if(buffer2){h2=buffer2->hour;}
    
    int diff = days_between(buffer1,buffer2);
    return (diff * 24) + h1 - h2;
}
int days_between(timestamp_t* buffer1,timestamp_t* buffer2){
    int d1=0;
    int d2=0;
    if(buffer1){d1=buffer1->day_year;}
    if(buffer2){d2=buffer2->day_year;}
    
    int diff = years_between(buffer1,buffer2);
    if(diff==0){return d1 - d2;}
    
    int dict = diff<0?-1:1;
    int day_sum = 0;
    for(int i=0;i<(diff * dict);i++){
        int year = buffer1->year + i;
        if(is_leap(year)){
            day_sum = day_sum + dict * 366;
        }else{
            day_sum = day_sum + dict * 365;            
        }
    }
    
    return day_sum + d1 - d2;
}
int monthes_between(timestamp_t* buffer1,timestamp_t* buffer2){
    int m1=0;
    int m2=0;
    if(buffer1){m1=buffer1->month;}
    if(buffer2){m2=buffer2->month;}
    
    int diff = years_between(buffer1,buffer2);
    return (diff*12) + m1 - m2;
}
int years_between(timestamp_t* buffer1,timestamp_t* buffer2){
    int y1 = 0;
    int y2 = 0;
    if(buffer1){y1=buffer1->year;}
    if(buffer2){y2=buffer2->year;}
    
    return y1 - y2;
}




