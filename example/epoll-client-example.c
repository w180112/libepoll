#include <common.h>

#define ETH_MTU 1500

STATUS send2cloud(const char *mu, U32 mulen, int sockfd);

int init_cloud_conn(U8 *server_ip, U16 port)
{
    int connfd;
    struct sockaddr_in servaddr;

    connfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    inet_pton(AF_INET, (const char *)server_ip, &servaddr.sin_addr);

    if (connect(connfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("connect error");
        return -1;
    }

    return connfd;
}

int cloud_recvd(int sockfd)
{
    char recv_buf[ETH_MTU];
    int n;

    for(;;) {
        n = read(sockfd, recv_buf, ETH_MTU);
        if (n == 0) {
            printf("server terminated\n");
            break;
        }
        PRINT_MESSAGE((unsigned char *)recv_buf, n);
        send2cloud(recv_buf, n, sockfd);
    }

    return 0;
}

STATUS send2cloud(const char *mu, U32 mulen, int sockfd)
{
    if (sockfd < 0) {
        printf("Send to sockfd failed.\n");
        return FALSE;
    }
    if (write(sockfd, mu, mulen) < 0) {
		perror("send to server socket fd failed");
        return FALSE;
    }

    return TRUE;
}

int main(int argc, char **argv)
{
    int sockfd = init_cloud_conn((unsigned char *)"127.0.0.1", 7002);
    if (sockfd < 0)
        return -1;
    cloud_recvd(sockfd);

    return 0;
}