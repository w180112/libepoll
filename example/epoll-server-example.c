#include <common.h>
#include <epoll.h>
#include <sock.h>

int main(int argc, char **argv)
{
    tEPOLL_MBX		*mail;
	tMBUF   		mbuf;
	int				msize;
	U16				ipc_type;
    tIPC_ID         Qid = -1;

    if (epoll_server_init(&Qid, "eth0", TCP) == ERROR) {
        perror("init failed");
        return -1;
    }

    for(;;) {
		printf("\n===============================================\n");
	    if (ipc_rcv2(Qid, &mbuf, &msize) == ERROR)
	    	continue;
	    ipc_type = *(U16*)mbuf.mtext;
		switch(ipc_type) {
		case IPC_EV_TYPE_TMR:
			/*if (ipc_type == IPC_EV_TYPE_TMR){
				ipc_prim = (tIPC_PRIM*)mbuf.mtext;
				ccb = ipc_prim->ccb;
				event = ipc_prim->evt;
				LLDP_FSM(ccb, event, 0);
			}*/
			break;
		case IPC_EV_TYPE_DRV:
			mail = (tEPOLL_MBX*)mbuf.mtext;
            /* Here is the msg we got */
            /* If msg len is 0, it means we recv tcp accept msg */
            if (mail->len == 0) {
                const char *json_str = "{\"action\":5,\"user_index\":0,\"vlan_id\":0}";
                drv_xmit((U8 *)json_str, strlen(json_str)+1, mail->eventfd, TCP);
                break;
            }
            /* Send back to client */
            const char *json_str = "{\"action\":0,\"user_index\":1,\"vlan_id\":100}";
            sleep(5);
            drv_xmit((U8 *)json_str, strlen(json_str)+1, mail->eventfd, TCP);
            //drv_xmit(mail->refp, mail->len, mail->eventfd);
			break;
		default:
		    ;
		}
    }

    return 0;
}