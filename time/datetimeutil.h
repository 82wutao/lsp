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
#endif
