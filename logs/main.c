#include "logger.h"
#include "datetimeutil.h"
int main(int c,char *v[]){
	int datetime[7];
	now(datetime);

	logger_appender_t appender;
	appender.out_stream = stdout;
	LOG_DEBUG(appender,"hello %s\n","world")
	return 0;
}
