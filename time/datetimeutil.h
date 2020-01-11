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

void now(int* buffer);
void now_with_millis(int* buffer);

int diff_between_millis(int* buffer1,int* buffer2);
int diff_between_seconds(int* buffer1,int* buffer2);
int diff_between_minutes(int* buffer1,int* buffer2);
int diff_between_hours(int* buffer1,int* buffer2);
int diff_between_days(int* buffer1,int* buffer2);
int diff_between_monthes(int* buffer1,int* buffer2);
int diff_between_years(int* buffer1,int* buffer2);
#endif
