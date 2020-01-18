#include "datetime/datetimeutil.h"


static int days_permonth[]={0,31,28,31,30,31,30,31,31,30,31,30,31};

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

void zero_timestamp(timestamp_t* buffer){
    if ( !buffer ) {
        return;
    }

    buffer->year = 0;
    buffer->month = 0;
    buffer->day_month= 0;
    buffer->day_year=0;
    buffer->hour=0;
    buffer->minute=0;
    buffer->second	=0;
    buffer->millis=0;
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
    
    if(diff<0){
        int days = 0;
        for(int i=0;i<diff*-1;i++){
            int year = buffer1->year + i;
            if(is_leap(year)){
            days +=366;
            }else{
            days +=365;
            }
        }
        return d1 - d2+days;
    }
        int days = 0;
        for(int i=0;i<diff*-1;i++){
            int year = buffer2->year + i;
            if(is_leap(year)){
            days +=366;
            }else{
            days +=365;
            }
        }
        return d2 - d1+days;
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

void timestamp_add(timestamp_t* src,int v,timeunit_t unit,timestamp_t* dest){
    if(!src || !dest){return ;}

    if(src !=dest){
        dest->year = src->year;
        dest->month = src->month;
        dest->day_month = src->day_month;
        dest->day_year = src->day_year;
        dest->hour = src->hour;
        dest->minute = src->minute;
        dest->second = src->second;
        dest->millis = src->millis;
    }

    if(unit==MILLIS){
        int tmp_v = dest->millis + v;
        int times = tmp_v / 1000;times = tmp_v<0? times-1 : times;
        int left = tmp_v % 1000;
        if (left < 0){
            left = left + 1000;
        }
        dest->millis = left;
        if(times==0){return ;}
        v = times;unit = SECOND;
    }

    if(unit==SECOND){
        int tmp_v = dest->second + v;
        int times = tmp_v / 60;times =tmp_v<0? times-1 : times;
        int left = tmp_v % 60;
        if (left < 0){
            left = left + 60;
        }
        dest->second = left;
        if(times==0){return ;}
        v = times;unit = MINUTE;
    }
    if(unit==MINUTE){
        int tmp_v = dest->minute + v;
        int times = tmp_v / 60;        times = tmp_v<0? times-1 : times;
        int left = tmp_v % 60;
        if (left < 0){
            left = left + 60;
        }
        dest->minute = left;
        if(times==0){return ;}
        v = times;unit = HOUR;
    }
    if(unit==HOUR){
        int tmp_v = dest->hour + v;
        int times = tmp_v / 24;        times = tmp_v<0? times-1 : times;
        int left = tmp_v % 24;
        if (left < 0){
            left = left + 24;
        }
        dest->hour = left;
        if(times==0){return ;}
        v = times;unit = DAY;
    }
    if(unit==DAY){
        dest->day_month= dest->day_month + v;
    }
    if(unit==MONTH){
        int tmp_v = (dest->month-1) + v;
        int times = tmp_v / 12;        times = tmp_v<0? times-1 : times;
        int left = tmp_v % 12;
        if (left < 0){
            left = left + 12;
        }
        dest->month = left + 1;
        dest->day_month = dest->day_month + v<0?1:-1;
        if(times==0){return ;}
        v = times;unit = YEAR;
    }
    if(unit==YEAR){
        dest->year = dest->year + v;
        dest->day_month = dest->day_month + v<0?1:-1;
    }

    int direct = v<0?-1:1;
         int c_m = dest->month;
        int c_y = dest->year;
        int c_d_m = dest->day_month;
    while(1){
        int max_month = days_permonth[c_m];
        if(c_d_m * direct > max_month){
            c_d_m = c_d_m - max_month*direct;
            c_m = c_m + direct;
        }else{
            if(c_d_m<0){c_d_m = c_d_m+max_month;}
            break;

        }
        if(c_m==0){c_m=12;c_y--;}
        if(c_m==13){c_m=1;c_y++;}
    }
    dest->year = c_y;
    dest->month = c_m;
    dest->day_month=c_d_m;

    int c_d_y =0;
    for(int i=1;i<c_m;i++){
        int days = days_permonth[i];
        c_d_y +=days;
    }
    dest->day_year = c_d_y+c_d_m;

}

//  ______|______|_______

