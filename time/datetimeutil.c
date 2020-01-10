#include "datetimeutil.h"


void now(int* buffer){
	if(!buffer){return;}
	time_t datetime;
	struct tm *p;
	
	time(&datetime);
	p = localtime(&datetime);


	buffer[DATETIME_INDEX_YEAR] = 1900+ p->tm_year;
	buffer[DATETIME_INDEX_MONTH] = p->tm_mon+1;
	buffer[DATETIME_INDEX_DAYOFMONTH]=	p->tm_mday;
	buffer[DATETIME_INDEX_HOUR]=p->tm_hour;
	buffer[DATETIME_INDEX_MINUTE]=p->tm_min;
	buffer[DATETIME_INDEX_SECOND]	=p->tm_sec;
	buffer[DATETIME_INDEX_MILLIS]=0;
}
void now_with_millis(int* buffer){
	if(!buffer){return;}

	time_t datetime;
	struct tm *p;
	
	struct timeval sms_v;
	time(&datetime);
	p = localtime(&datetime);

	gettimeofday(&sms_v,NULL);

	buffer[DATETIME_INDEX_YEAR] = 1900+ p->tm_year;
	buffer[DATETIME_INDEX_MONTH] = p->tm_mon+1;
	buffer[DATETIME_INDEX_DAYOFMONTH]=	p->tm_mday;
	buffer[DATETIME_INDEX_HOUR]=p->tm_hour;
	buffer[DATETIME_INDEX_MINUTE]=p->tm_min;
	buffer[DATETIME_INDEX_SECOND]	=p->tm_sec;
	buffer[DATETIME_INDEX_MILLIS]=sms_v.tv_usec/1000;
}
