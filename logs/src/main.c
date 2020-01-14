
#include "logs/logger.h"
#include "datetime/datetimeutil.h"
int main(int c,char *v[]){
	timestamp_t datetime;
	now(&datetime);

	logger_appender_t appender;
	appender.lvl = Debug;
	appender.out_stream = stdout;

    appender.modifier = LOG_MODIFIER_DATE | LOG_MODIFIER_TIME;
	LOG_DEBUG(appender,"ymdhms hello  %s\n","world")

    appender.modifier = LOG_MODIFIER_DATE | LOG_MODIFIER_TIME|LOG_MODIFIER_MILLIS;
	LOG_DEBUG(appender,"ymdhmssss hello  %s\n","world")

    appender.modifier = LOG_MODIFIER_FILE|LOG_MODIFIER_FUNC | LOG_MODIFIER_LINE;
	LOG_DEBUG(appender,"file#func:line hello  %s\n","world")

        appender.modifier = LOG_MODIFIER_THREAD;
	LOG_DEBUG(appender,"thread hello  %s\n","world")
    return 0;
}
