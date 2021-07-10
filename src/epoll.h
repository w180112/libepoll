#ifndef _EPOLL_H_
#define _EPOLL_H_

#include <common.h>
#include "sock.h"

#define ETH_MTU	1500

STATUS epoll_server_init(tIPC_ID *Qid, char *if_name, SOCK_MODE_t sock_mode);
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