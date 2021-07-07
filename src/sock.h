
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
    int listen_sock;
};

extern STATUS SOCK_INIT(struct sock_info *sock_info);
extern void drv_xmit(U8 *mu, U16 mulen, int eventfd);
extern void sockd(struct sock_info *sockinfo);

#endif