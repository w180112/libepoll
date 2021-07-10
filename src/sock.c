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
#include <ipc.h>

#include "sock.h"
#include "epoll.h"

#define DEFAULT_PORT    7002
#define MAX_CONN        16
#define MAX_EVENTS      32
#define BUF_SIZE        1500
#define MAX_LINE        256

#define EPOLL_Q_KEY 	0x0b00

extern tIPC_ID epollQid;
static struct  sockaddr_ll 	sll;

static STATUS epoll_ctl_add(int epfd, int fd, uint32_t events);
static int setnonblocking(int sockfd);
STATUS epoll_send2mailbox(U8 *mu, int mulen, int eventfd);
BOOL raw_sock_init(struct sock_info *sock_info, char *ifname);
BOOL tcp_sock_init(struct sock_info *sock_info, char *ifname);

/**************************************************************************
 * SOCK_INIT :
 *
 * Some useful methods :
 * - from string to ulong
 *   inet_aton("10.5.5.217", (struct in_addr*)cSA_ip); 
 **************************************************************************/ 
STATUS SOCK_INIT(struct sock_info *sock_info, char *if_name, SOCK_MODE_t sock_mode) 
{
	switch (sock_mode) {
	case RAW:
		if (raw_sock_init(sock_info, if_name) == FALSE)
			return FALSE;
		break;
	case TCP:
		if (tcp_sock_init(sock_info, if_name) == FALSE)
			return FALSE;
		break;
	default:
		printf("Wrong sock mode detected.\n");
		return FALSE;
	}
	
	sock_info->epfd = epoll_create(1);
	epoll_ctl_add(sock_info->epfd, sock_info->sockfd, EPOLLIN | EPOLLOUT | EPOLLET);

	return TRUE;
}

static STATUS epoll_ctl_add(int epfd, int fd, uint32_t events)
{
	struct epoll_event ev;
	ev.events = events;
	ev.data.fd = fd;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		perror("epoll_ctl()\n");
		return ERROR;
	}

	return TRUE;
}

static int setnonblocking(int sockfd)
{
	if (fcntl(sockfd, F_SETFD, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK) == -1)
		return -1;
	return 0;
}

void raw_recvd(struct sock_info *sockinfo)
{
	int rxlen;
	struct epoll_event events[MAX_EVENTS];
	int nfds;
	char buf[1500];

	for (;;) {
        nfds = epoll_wait(sockinfo->epfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }
		for(int i=0; i<nfds; i++) {
			if (events[i].data.fd == sockinfo->sockfd) {
				for(;;) {
					int rxlen = recvfrom(sockinfo->sockfd, buf, 1500, 0, NULL, NULL);
					if (rxlen == -1) {
                      	/* If errno == EAGAIN, that means we have read all
                       	 * data. So go back to the main loop. */
                      	if (errno != EAGAIN) {
                          	perror ("read");
                        }
                      	break;
                    }
					printf("recv: \n");
					PRINT_MESSAGE(buf, rxlen);
					epoll_send2mailbox(buf, rxlen, events[i].data.fd);
				}
			}
        }
    }
}

void tcp_recvd(struct sock_info *sockinfo)
{
	int nfds;
	int conn_sock;
	struct sockaddr_in cli_addr;
	U8 buf[BUF_SIZE];
	int n;
	struct epoll_event events[MAX_EVENTS];
	struct sock_info sock_info = *sockinfo;

	int socklen = sizeof(cli_addr);

	for (;;) {
		nfds = epoll_wait(sock_info.epfd, events, MAX_EVENTS, -1);
		for (int i=0; i<nfds; i++) {
			if (events[i].data.fd == sock_info.sockfd) {
				/* handle new connection */
				conn_sock = accept(sock_info.sockfd, (struct sockaddr *)&cli_addr, (socklen_t * restrict)&socklen);
				inet_ntop(AF_INET, (char *)&(cli_addr.sin_addr), (char * restrict)buf, sizeof(cli_addr));
				printf("connected with %s:%d\n", buf, ntohs(cli_addr.sin_port));
				setnonblocking(conn_sock);
				epoll_ctl_add(sock_info.epfd, conn_sock, EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP | EPOLLOUT);
				//epoll_send2mailbox(buf, 0, conn_sock);
			} 
			else if (events[i].events & EPOLLIN) {
				/* handle EPOLLIN event */
				for(;;) {
					bzero(buf, sizeof(buf));
					n = recvfrom(events[i].data.fd, buf, sizeof(buf), 0, NULL, NULL);
					if (n <= 0 /* || errno == EAGAIN */ ) {
						break;
					} 
					else {
						printf("recv: \n");
						PRINT_MESSAGE(buf, n);
						epoll_send2mailbox(buf, n, events[i].data.fd);
					}
				}
			}
			else if (events[i].events & EPOLLOUT) {
				//printf("Client %d is ready to send\n", events[i].data.fd);
				struct epoll_event ev;
				ev.events = EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP;
        		ev.data.fd = events[i].data.fd;
        		epoll_ctl(sock_info.epfd, EPOLL_CTL_MOD, events[i].data.fd, &ev);
				epoll_send2mailbox(buf, 0, events[i].data.fd);
			}
			else
				printf("unexpected event\n");
			/* check if the connection is closing */
			if (events[i].events & (EPOLLRDHUP | EPOLLHUP)) {
				printf("connection closed\n");
				epoll_ctl(sock_info.epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
				close(events[i].data.fd);
				continue;
			}
		}
	}
}

/**************************************************************
 * drv_xmit :
 *
 * - sid  : socket id
 **************************************************************/
void drv_xmit(U8 *mu, U16 mulen, int eventfd, SOCK_MODE_t sock_mode)
{
	switch (sock_mode) {
	case RAW:
		memcpy(sll.sll_addr, mu, ETH_ALEN);
		sendto(eventfd, mu, mulen, 0, (struct sockaddr*)&sll, sizeof(sll));
		break;
	case TCP:
		if (write(eventfd, mu, mulen) < 0)
			perror("send to client socket fd failed");
		break;
	default:
		printf("Wrong sock mode detected.\n");
	}
}

/*********************************************************
 * epoll_send2mailbox:
 *
 * Input  : mu - incoming ethernet+igmp message unit
 *          mulen - mu length
 *          port - 0's based
 *          sid - need tag ? (Y/N)
 *          prior - if sid=1, then need set this field
 *          vlan - if sid=1, then need set this field
 *
 * return : TRUE or ERROR(-1)
 *********************************************************/
STATUS epoll_send2mailbox(U8 *mu, int mulen, int eventfd)
{
	tEPOLL_MBX mail;

    if (epollQid == -1) {
		if ((epollQid=msgget(EPOLL_Q_KEY,0600|IPC_CREAT)) < 0) {
			printf("send> Oops! epollQ(key=0x%x) not found\n",EPOLL_Q_KEY);
   	 	}
	}
	
	if (mulen > ETH_MTU) {
	 	printf("Incoming frame length(%d) is too lmaile!\n",mulen);
		return ERROR;
	}

	mail.len = mulen;
	memcpy(mail.refp,mu,mulen); /* mail content will be copied into mail queue */
	
	//printf("lldp_send2mailbox(lldp_sock.c %d): mulen=%d\n",__LINE__,mulen);
	mail.type = IPC_EV_TYPE_DRV;
	mail.eventfd = eventfd;
	ipc_sw(epollQid, &mail, sizeof(mail), -1);
	return TRUE;
}

BOOL raw_sock_init(struct sock_info *sock_info, char *ifname)
{
	struct ifreq		ethreq;
	
	if ((sock_info->sockfd=socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
	    perror("br raw socket");
	    return FALSE;
	}

	/* Set the network card in promiscuous mode */
  	strncpy(ethreq.ifr_name,ifname,IFNAMSIZ-1);
  	if (ioctl(sock_info->sockfd, SIOCGIFFLAGS, &ethreq)==-1) {
    	perror("ioctl (SIOCGIFCONF) 1\n");
    	close(sock_info->sockfd);
    	return FALSE;
  	}
	
  	ethreq.ifr_flags |= IFF_PROMISC;
  	if (ioctl(sock_info->sockfd, SIOCSIFFLAGS, &ethreq)==-1) {
    	perror("ioctl (SIOCGIFCONF) 2\n");
    	close(sock_info->sockfd);
    	return FALSE;
  	}

	memset(&sll, 0, sizeof(sll));
	sll.sll_family = AF_PACKET;
	sll.sll_protocol = htons(ETH_P_ALL);
	sll.sll_halen = 6;
		
	ioctl(sock_info->sockfd, SIOCGIFINDEX, &ethreq); //ifr_name must be set to "eth?" ahead
	sll.sll_ifindex = ethreq.ifr_ifindex;
	
	printf("ifindex=%d\n",sll.sll_ifindex);

	return TRUE;
}
BOOL tcp_sock_init(struct sock_info *sock_info, char *ifname)
{
	struct sockaddr_in srv_addr;

	sock_info->sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_info->sockfd < 0) {
		perror("sockfd init: ");
		return FALSE;
	}

	bzero((char *)&srv_addr, sizeof(struct sockaddr_in));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = INADDR_ANY;
	srv_addr.sin_port = htons(DEFAULT_PORT);
	bind(sock_info->sockfd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));

	setnonblocking(sock_info->sockfd);
	listen(sock_info->sockfd, MAX_CONN);

	return TRUE;
}