#ifndef __DP_ETH__
#define __DP_ETH__

#include <stdint.h>
#include <stddef.h>

#include "dp_buffer.h"
// Ethernet-related definitions and declarations

typedef struct dp_eth_hdr_{
    uint8_t dst_mac[6];
    uint8_t src_mac[6];
    uint16_t ethertype;
} __attribute__((packed)) dp_eth_hdr_t;

typedef struct dp_eth_frame_{
    dp_eth_hdr_t hdr;
    uint8_t *payload;
    size_t payload_len;
} __attribute__((packed)) dp_eth_frame_t;


int dp_eth_parse(dp_buf_t *buf, dp_eth_frame_t *out);

void dp_eth_log_frame(dp_eth_frame_t *frame);

#endif // __DP_ETH__