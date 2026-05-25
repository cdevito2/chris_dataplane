#include "dp_log.h"
#include <time.h>
#include <stdarg.h>

static dp_log_level_t currentLevel = LOG_DEBUG;


void set_log_level(dp_log_level_t lvl)
{
    currentLevel = lvl;
}

static const char* dp_log_level_str(dp_log_level_t level)
{
    switch (level) {
        case LOG_DEBUG:
            return "DEBUG";
        case LOG_INFO:
            return "INFO";
        case LOG_WARN: 
            return "WARN";
        case LOG_ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}

void dp_log(dp_log_level_t level, const char *fmt, ...)
{
    if (level < currentLevel){
        return;
    }

    //now we need to add timestamps
    time_t now = time(NULL);

    //convert epoch to local time
    struct tm *tm_now;
    tm_now = localtime(&now);

    //log buffer
    char ts[32];

    //custom string
    strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", &tm_now);

    //add log level 

    fprintf(stderr, "%s [%s]", ts, dp_log_level_str(level));

    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    fprintf(stderr, "\n");
}



