/**************************************************************************
 * DBG.C
 *
 * Debug methods
 *
 * Created by Dennis Tseng on Nov 05,'08
 **************************************************************************/

#include    <common.h>
#include	"epoll.h"
#include    "dbg.h"

#define 	DBG_EPOLL_MSG_LEN     		128
	
U8       	epollDbgFlag=1;

/***************************************************
 * DBG_EPOLL:
 ***************************************************/	
void DBG_EPOLL(U8 level, char *fmt,...)
{
	va_list ap; /* points to each unnamed arg in turn */
	char    buf[256],msg[DBG_EPOLL_MSG_LEN];
	
	//user offer level must > system requirement
    if (epollDbgFlag < level){
    	return;
    }

	va_start(ap, fmt); /* set ap pointer to 1st unnamed arg */
    vsnprintf(msg, DBG_EPOLL_MSG_LEN, fmt, ap);

    sprintf(buf,"epoll> ");
    
  	strcat(buf,msg);
   	printf("%s",buf);
    va_end(ap);
}
