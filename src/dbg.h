/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
  DBG.H

  Designed by Dennis Tseng on Jan 1, 2002
/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

#ifndef _DBG_H_
#define _DBG_H_

#include <common.h>

#define	DBGLVL1		1
#define DBGLVL2		2

extern 	void 		DBG_EPOLL(U8 level, char *fmt,...);
extern  U8      epollDbgFlag;
#endif