#include "dp_buffer.h"
#include "dp_netdev.h"
#include "dp_log.h"
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include "dp_eth.h"
#include "dp_arp.h"

static void event_loop(dp_netdev_t *dev)
{
    // Allocate heap-allocated packet buffer
    dp_buf_t *pkt = dp_buf_alloc(2048);
    if (pkt == NULL) {
        DP_ERROR("Failed to allocate packet buffer\n");
        return;
    }

    DP_INFO("Event loop started on %s\n", dev->name);

    while (1)
    {
        // Reset buffer for next packet
        dp_buf_reset(pkt);
        
        // Read packet from TAP interface
        int rc = dp_netdev_read(dev, pkt->data, pkt->capacity);

        // If read failed, continue to next iteration
        if (rc < 0) {
            DP_WARN("Read failed\n");
            continue;
        }

        // Update buffer length with actual bytes read
        pkt->len = rc;

        DP_DEBUG("Read %zu bytes\n", pkt->len);

        // process packet into ethernet/arp 
        dp_eth_frame_t eth_frame;
        if (dp_eth_parse(pkt, &eth_frame) != 0) {
            DP_WARN("Failed to parse Ethernet frame\n");
            continue;
        }

        //else now we have eth frame struct, we can log it and then check if it's an arp packet
        dp_eth_log_frame(&eth_frame);

        switch(ntohs(eth_frame.hdr.ethertype))
        {
            case 0x0806: //arp
                dp_arp_handle_request(dev, pkt, &eth_frame);
                break;
            default:
                DP_DEBUG("Unsupported ethertype: 0x%04x\n", ntohs(eth_frame.hdr.ethertype));
                break;
        }
        /*
        // Echo packet back out
        int write_rc = dp_netdev_write(dev, pkt->data, pkt->len);

        if (write_rc < 0) {
            DP_WARN("Write failed\n");
            continue;
        }

        DP_DEBUG("Wrote %d bytes\n", write_rc);
        */
    }

    DP_INFO("Event loop stopped\n");

    // Free heap-allocated buffer
    dp_buf_free(pkt);
}

int main(int argc, char **argv)
{
    const char *tap_name = "tap0";

    // Allow specifying interface name as argument
    if (argc > 1) {
        tap_name = argv[1];
    }

    DP_INFO("Starting dataplane on interface: %s\n", tap_name);

    // Create and initialize netdev structure
    dp_netdev_t dev;
    memset(&dev, 0, sizeof(dev));

    // Open TAP interface (is_tun=0 for TAP mode)
    int open_rc = dp_netdev_open(&dev, tap_name, 0);
    if (open_rc != 0) {
        DP_ERROR("Failed to open interface %s\n", tap_name);
        return 1;
    }

    DP_INFO("Opened interface: %s (fd=%d)\n", dev.name, dev.fd);

    // Run the event loop
    event_loop(&dev);

    // Close the interface
    int close_rc = dp_netdev_close(&dev);
    if (close_rc != 0) {
        DP_ERROR("Failed to close interface\n");
        return 1;
    }

    DP_INFO("Closed interface: %s\n", dev.name);

    return 0;
}