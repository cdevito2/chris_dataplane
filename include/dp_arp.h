#ifndef __DP_ARP_H__
#define __DP_ARP_H__

#include <stdint.h>
#include "dp_buffer.h"
#include "dp_netdev.h"
#include "dp_eth.h"

typedef struct arp_hdr_{
    uint16_t hw_type; //1 for ethernet cable
    uint16_t proto_type;//0x0800 for ipv4
    uint8_t hw_addr_len;//6 for mac
    uint8_t proto_addr_len;//4 for ipv4
    uint16_t op_code;//Arp Req or reply
    uint8_t src_mac[6];
    uint32_t src_ip;
    uint8_t dst_mac[6];
    uint32_t dst_ip;

}__attribute__((packed)) arp_hdr_t; //total 28 bytes


typedef struct arp_cache_entry_{
    uint32_t ip_addr;
    uint8_t mac_addr[6];
    uint32_t timestamp;
} __attribute__((packed)) arp_cache_entry_t;


void dp_arp_init();
int dp_arp_lookup(uint32_t ip_addr, uint8_t *mac_addr_out);
void dp_arp_insert(uint32_t ip_addr, uint8_t *mac_addr);
void dp_arp_handle_request(dp_netdev_t *dev, dp_buf_t *buf, dp_eth_frame_t *eth_frame);

#endif 