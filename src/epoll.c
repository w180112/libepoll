#include <common.h>
#include <ipc.h>
#include "epoll.h"
#include "sock.h"
#include "dbg.h"

#define EPOLL_Q_KEY 0x0b00

tIPC_ID epollQid = -1;
struct sock_info sock_info;

/*---------------------------------------------------------
 * EPOLL_bye : signal handler for INTR-C only
 *--------------------------------------------------------*/
void EPOLL_bye()
{
    printf("epoll> delete Qid(0x%x)\n",epollQid);
    DEL_MSGQ(epollQid);
	close(sock_info.listen_sock);
    printf("bye!\n");
	exit(0);
}

STATUS EPOLL_ipc_init(void)
{
	if (epollQid != -1){
		printf("epoll> Qid(0x%x) is already existing\n",epollQid);
		return TRUE;
	}
	
	if (GET_MSGQ(&epollQid,EPOLL_Q_KEY) == ERROR){
	   	printf("epoll> can not create a msgQ for key(0x0%x)\n",EPOLL_Q_KEY);
	   	return ERROR;
	}
	printf("epoll> new Qid(0x%x)\n",epollQid);
	return TRUE;
}

STATUS epolldInit()
{	
	if (EPOLL_ipc_init() == ERROR)
		return ERROR;

	sleep(1);
	DBG_EPOLL(DBGLVL1,NULL,"============ epoll server init successfully ==============\n");
    return TRUE;
}

STATUS epoll_server_init(tIPC_ID *Qid)
{
	if (epolldInit() != TRUE)
		return ERROR;
	
	pthread_t t;
  	pthread_create(&t, NULL, (void *)SOCK_INIT, &sock_info);
	/*if (fork() == 0) {
		SOCK_INIT(&sock_info);
   		//sockd(sock_info);
	}*/

    signal(SIGINT,EPOLL_bye);
    *Qid = epollQid;

    return TRUE;
}
