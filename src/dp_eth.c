#include "dp_eth.h"
#include <arpa/inet.h>  // For ntohs()

#define DP_ETH_HDR_LEN 14

//takes in raw packet and output struct and ppopulates output struct
int dp_eth_parse( dp_buf_t *buf,  dp_eth_frame_t *out)
{
    if (buf == NULL || out == NULL) {
        return -1;
    }

    //parse buffer into ethernet frame struct
    if (buf->len < DP_ETH_HDR_LEN) {
        return -1; // Not enough data for Ethernet header
    }

    //first 14 bytes are the header, the rest is payload
    dp_eth_hdr_t *hdr = (dp_eth_hdr_t *)buf->data;
    out->hdr = *hdr;
    out->payload = buf->data + DP_ETH_HDR_LEN;
    out->payload_len = buf->len - DP_ETH_HDR_LEN;

    return 0;
}

void dp_eth_log_frame(dp_eth_frame_t *frame)
{
    if (frame == NULL) {
        return;
    }

    // Log Ethernet frame details
    DP_INFO("Ethernet Frame: SRC MAC: %02x:%02x:%02x:%02x:%02x:%02x, DST MAC: %02x:%02x:%02x:%02x:%02x:%02x, Ethertype: 0x%04x, Payload Length: %zu\n",
            frame->hdr.src_mac[0], frame->hdr.src_mac[1], frame->hdr.src_mac[2],
            frame->hdr.src_mac[3], frame->hdr.src_mac[4], frame->hdr.src_mac[5],
            frame->hdr.dst_mac[0], frame->hdr.dst_mac[1], frame->hdr.dst_mac[2],
            frame->hdr.dst_mac[3], frame->hdr.dst_mac[4], frame->hdr.dst_mac[5],
            ntohs(frame->hdr.ethertype), frame->payload_len);
}