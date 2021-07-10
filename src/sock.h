
#ifndef _SOCK_H_
#define _SOCK_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <common.h>

struct sock_info {
    int epfd;
    int sockfd;
};

typedef enum {
    RAW,
    TCP
}SOCK_MODE_t;

extern STATUS SOCK_INIT(struct sock_info *sock_info, char *if_name, SOCK_MODE_t sock_mode);
extern void drv_xmit(U8 *mu, U16 mulen, int eventfd, SOCK_MODE_t sock_mode);
extern void raw_recvd(struct sock_info *sockinfo);
extern void tcp_recvd(struct sock_info *sockinfo);

#endif