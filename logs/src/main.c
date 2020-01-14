
#include "logs/logger.h"
#include "datetime/datetimeutil.h"
int main(int c,char *v[]){
	timestamp_t datetime;
	now(&datetime);

	logger_appender_t appender;
	appender.lvl = Debug;
	appender.out_stream = stdout;
	LOG_DEBUG(appender,"hello  %s\n","world")
	return 0;
}
