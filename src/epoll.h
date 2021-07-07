#ifndef _EPOLL_H_
#define _EPOLL_H_

#include <common.h>

#define ETH_MTU	1500

STATUS epoll_server_init(tIPC_ID *Qid);
void EPOLL_bye();

/*-----------------------------------------
 * Queue between IF driver and daemon
 *----------------------------------------*/
typedef struct {
	U16  			type;
	U8          	refp[ETH_MTU];
	int	        	len;
	int 			eventfd;
} tEPOLL_MBX;

#endif