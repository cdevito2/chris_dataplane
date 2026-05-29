#include "dp_arp.h"
#include "dp_netdev.h"
#include <time.h>
#include <arpa/inet.h>      // Missing: ntohs(), htons()
#include <string.h>         // Missing: memcpy(), memset()
#include "dp_log.h"         // Missing: DP_INFO macro

static arp_cache_entry_t arp_cache[32]; // Simple ARP cache with fixed size

#define ARP_HDR_SIZE 28

void dp_arp_init()
{
    // Initialize ARP cache or any necessary data structures
    DP_INFO("ARP module initialized\n");
    memset(arp_cache, 0, sizeof(arp_cache));
}

int dp_arp_lookup(uint32_t ip_addr, uint8_t *mac_addr_out)
{
    // Lookup IP address in ARP cache and return corresponding MAC address
    // Return 1 on success 0 failure
    if (mac_addr_out == NULL) {
        return 0;
    }
    for (int i=0;i<32;i++)
    {
        if (arp_cache[i].ip_addr == ntohl(ip_addr))
        {
            memcpy(mac_addr_out, arp_cache[i].mac_addr, 6);
            return 1;   
        }
    }
    return 0; 
}

void dp_arp_insert(uint32_t ip_addr, const uint8_t *mac_addr)
{
    // Insert IP address and MAC address into ARP cache
    for (int i=0;i<32;i++)
    {
        if (arp_cache[i].ip_addr == ntohl(ip_addr))
        {
            //update existing entry
            memcpy(arp_cache[i].mac_addr, mac_addr, 6);
            arp_cache[i].timestamp = time(NULL);
            return;
        }
        else if (arp_cache[i].ip_addr == 0)
        {
            arp_cache[i].ip_addr = ntohl(ip_addr);
            memcpy(arp_cache[i].mac_addr, mac_addr, 6);
            arp_cache[i].timestamp = time(NULL); //Chris TODO: add arp aging 
            return;
        }
    }
}

void dp_arp_handle_request(dp_netdev_t *dev, dp_buf_t *buf, dp_eth_frame_t *eth_frame)
{
    // Handle incoming ARP request
    if (buf == NULL || eth_frame == NULL) {
        return;
    }
    //parse arp, update cache, generate arp reply if needed
    if (buf->len < ARP_HDR_SIZE+sizeof(dp_eth_hdr_t)) {
        return; // Not enough data for ARP header
    }
    //we know the payload of buf is arp packet
    arp_hdr_t *arp_hdr = (arp_hdr_t *)eth_frame->payload;
    dp_eth_hdr_t*eth_hdr = (dp_eth_hdr_t *)buf->data; //point to the start of the ethernet header

    // Validate ARP header fields
    if (ntohs(arp_hdr->hw_type) != 1) {
        return; // Not Ethernet
    }
    if (ntohs(arp_hdr->proto_type) != 0x0800) {
        return; // Not IPv4
    }
    if (arp_hdr->hw_addr_len != 6) {
        return; // Invalid MAC length
    }
    if (arp_hdr->proto_addr_len != 4) {
        return; // Invalid IP length
    }

    if (ntohs(arp_hdr->op_code) == 2) //check if it's an arp reqly
    {
        dp_arp_insert(arp_hdr->src_ip, arp_hdr->src_mac); //update cache with sender info
        return;
    }
    if (ntohs(arp_hdr->op_code) != 1) //check if it's an arp request
    {
        return; //not an arp request, ignore
    }

    //update cache with sender info
    dp_arp_insert(arp_hdr->src_ip, arp_hdr->src_mac);

    //check if this is an arp request for our ip
    if (ntohl(arp_hdr->dst_ip) == dev->ip_addr)
    {
        //build arp reply
        arp_hdr->hw_type = htons(1); // Ethernet
        arp_hdr->proto_type = htons(0x0800); // IPv4
        arp_hdr->hw_addr_len = 6;
        arp_hdr->proto_addr_len = 4;
        arp_hdr->op_code = htons(2); // ARP Reply

        uint32_t sender_ip = arp_hdr->src_ip;

        arp_hdr->src_ip = htonl(dev->ip_addr); //our ip is the src ip of the reply
        arp_hdr->dst_ip = sender_ip; //dst ip is the src ip of the request

        uint8_t sender_mac[6];
        memcpy(sender_mac, arp_hdr->src_mac, 6); //sender mac

        //we should get our actual mac address
        //from the netdev struct instead of using the dst mac of the request
        

        memcpy(arp_hdr->src_mac, dev->mac_addr, 6); //our mac is the src mac of the reply
        memcpy(arp_hdr->dst_mac, sender_mac, 6); //dst mac is the src mac 

        //swap ethernet src and dst
        memcpy(eth_hdr->dst_mac, sender_mac, 6); //dst mac is the src mac of the request
        memcpy(eth_hdr->src_mac, dev->mac_addr, 6); //our mac is the src mac of the reply
        eth_hdr->ethertype = htons(0x0806); // Ethertype for ARP

        //transmit arp reply
        //we can reuse the same buffer, just need to update the length and data pointer
        buf->len = sizeof(dp_eth_hdr_t) + ARP_HDR_SIZE; 

        dp_netdev_write(dev, buf->data, buf->len); //send out the arp reply
    } else{
        //do nothing
        return;
    }
}