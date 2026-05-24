#ifndef __DP_BUFFER__
#define __DP_BUFFER__

#include <stdint.h>
#include <stddef.h>

typedef struct dp_buf {
    uint8_t *data;
    size_t len;
    size_t capacity;
} dp_buf_t;

dp_buf_t *dp_buf_alloc(size_t capacity);
void dp_buf_free(dp_buf_t *buf);
void dp_buf_reset(dp_buf_t *buf);


#endif