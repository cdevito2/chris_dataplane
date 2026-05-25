#include "dp_netdev.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <string.h>
#include <unistd.h>
#include <linux/if_tun.h>

int dp_netdev_open(dp_netdev_t *dev, const char *if_name, int is_tun)
{
    //validate arguments first
    if ((dev == NULL) || (if_name ==  NULL)){
        return -1;
    }
    //open /dev/net/tun
    int fd = 0;
    fd = open("/dev/net/tun", O_RDWR);
    if (fd < 0 )
    {
        return -1;
    }

    //prepare ifreq struct 
    struct ifreq ifr;
    memset (&ifr, 0 , sizeof(ifr));
    
    strncpy(ifr.ifr_name, if_name, IFNAMSIZ);

    if (is_tun)
    {
        ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
    }else {
        ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    }

    //call ioctl to create/config interface
    if (ioctl(fd, TUNSETIFF, &ifr) < 0 )
    {
        close(fd);
        return -1;
    }

    //fill in dp_netdev struct 
    memset(dev,0,sizeof(*dev));
    dev->fd =fd;
    dev->is_tun = is_tun;
    strncpy(dev->name, ifr.ifr_name, IFNAMSIZ);
    dev->mtu = 1500;


    return 0;
}


int dp_netdev_read(dp_netdev_t *dev, uint8_t *buf, size_t len){

    ssize_t nread;
    if (dev == NULL){
        return -1;
    }
    nread = read(dev->fd, buf, len);

    if (nread < 0)
    {
        return -1;
    }

    return nread;

}

int dp_netdev_write(dp_netdev_t *dev, const uint8_t *buf, size_t len){
    ssize_t nwritten;
    if (dev == NULL){
        return -1;
    }

    nwritten = write(dev->fd, buf, len);

    if (nwritten < 0)
    {
        return -1;
    }

    return nwritten;
}

int dp_netdev_close(dp_netdev_t *dev){
    if (dev == NULL)
    {
        return -1;
    }

    int closeRes = close(dev->fd);

    if (closeRes < 0 ){
        return -1;
    }
    dev->mtu = 0;
    memset(dev->name, 0, sizeof(dev->name));
    return 0;
}