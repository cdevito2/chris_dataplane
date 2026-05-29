#ifndef __DP_NETDEV__
#define __DP_NETDEV__

#include <stdint.h>
#include <linux/if.h>

typedef struct dp_netdev_{
    int fd; //file descriptor for /dev/net/tun
    char name[IFNAMSIZ];
    int is_tun; //1 = TUN, 0 = TAP
    int mtu;
    uint8_t mac_addr[6]; //MAC address for TAP interfaces, not used for TUN
    uint32_t ip_addr; //IP address for TUN interfaces, not used for TAP
} dp_netdev_t;


int dp_netdev_open(dp_netdev_t *dev, const char *if_name, int is_tun);
int dp_netdev_read(dp_netdev_t *dev, uint8_t *buf, size_t len);
int dp_netdev_write(dp_netdev_t *dev, const uint8_t *buf, size_t len);
int dp_netdev_close(dp_netdev_t *dev);

#endif