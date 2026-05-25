#ifndef __LOG__
#define __LOG__

#include <stdio.h>

typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} dp_log_level_t;

void set_log_level(dp_log_level_t level);

void dp_log(dp_log_level_t level, const char *fmt, ...);

#define DP_DEBUG(fmt, ...) dp_log(LOG_DEBUG, "[DEBUG] " fmt, ##__VA_ARGS__)
#define DP_INFO(fmt, ...)  dp_log(LOG_INFO,  "[INFO ] " fmt, ##__VA_ARGS__)
#define DP_WARN(fmt, ...)  dp_log(LOG_WARN,  "[WARN ] " fmt, ##__VA_ARGS__)
#define DP_ERROR(fmt, ...) dp_log(LOG_ERROR, "[ERROR] " fmt, ##__VA_ARGS__)

#endif
