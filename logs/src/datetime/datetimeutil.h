#ifndef __DATETIMEUTIL_H
#define __DATETIMEUTIL_H

#include <sys/time.h>
#include <unistd.h>
#include <time.h>

#define DATETIME_INDEX_YEAR 0
#define DATETIME_INDEX_MONTH 1
#define DATETIME_INDEX_DAYOFMONTH 2
#define DATETIME_INDEX_HOUR 3
#define DATETIME_INDEX_MINUTE 4
#define DATETIME_INDEX_SECOND 5
#define DATETIME_INDEX_MILLIS 6

typedef enum {MILLIS,SECOND,MINUTE,HOUR,DAY,MONTH,YEAR} timeunit_t;

typedef struct {
    int year;
    int month;
    int day_month;
    int day_year;
    int hour;
    int minute;
    int second;
    int millis;
} timestamp_t;

void now(timestamp_t* buffer);
void now_with_millis(timestamp_t* buffer);
void zero_timestamp(timestamp_t* buffer);

#define is_leap(year) ((year % 4 == 0 && year / 100 != 0) || (year % 400 == 0))
    
unsigned long millis_between(timestamp_t* buffer1,timestamp_t* buffer2);
unsigned long seconds_between(timestamp_t* buffer1,timestamp_t* buffer2);
int minutes_between(timestamp_t* buffer1,timestamp_t* buffer2);
int hours_between(timestamp_t* buffer1,timestamp_t* buffer2);
int days_between(timestamp_t* buffer1,timestamp_t* buffer2);
int monthes_between(timestamp_t* buffer1,timestamp_t* buffer2);
int years_between(timestamp_t* buffer1,timestamp_t* buffer2);

void timestamp_add(timestamp_t* src,int v,timeunit_t unit,timestamp_t* dest);
#endif
