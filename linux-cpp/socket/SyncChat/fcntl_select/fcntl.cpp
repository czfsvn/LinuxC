//from: http://outofmemory.cn/code-snippet/7749/linux-socket-network-program-fcntl-select-duoge-customer-duan-connection-service-qi-duan-qingxing
//


#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/un.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>

#define     SERVER_PORT     3838
#define     BACKLOG         10
#define     MAX_CONN_NUM    10
#define     MAX_DATA_SIZE   100

int main()
{
    //创建socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        exit(1);
    }
    printf("socket success! sockfd = %d\n", sockfd);

    //设置sockaddr_in信息
    struct sockaddr_in  srv_addr;
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(SERVER_PORT);
    srv_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(srv_addr.sin_zero), 8);

    //bind
    if (bind(sockfd, (struct sockaddr*)&srv_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind");
        exit(1);
    }
    printf("bind success! ");

    //listen
    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }
    printf("listen ....\n");

    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags < 0)
    {
        perror("F_GETFL");
        exit(1);
    }

    //if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0)
    if (fcntl(sockfd, F_SETFL, flags | O_ASYNC) < 0)
    {
        perror("F_SETFL");
        exit(1);
    }
    printf("fcntl....\n");

    while (1)
    {
        socklen_t sin_size = sizeof(struct sockaddr_in);
        struct sockaddr_in cli_addr;
        printf("wait for accept a new ....\n");
        int clifd = accept(sockfd, (struct sockaddr*)&cli_addr, &sin_size);
        if (clifd == -1)
        {
            perror("accept");
            exit(1);
        }

        printf("wait for recv a new ....\n");
        char buf[MAX_DATA_SIZE] = {0};
        int rcv_size = recv(clifd, buf, MAX_DATA_SIZE, 0);
        if (rcv_size == -1)
        {
            perror("recv");
            exit(1);
        }
        printf("rcvMsg: %s", buf);

        printf("wait for read a new ....\n");
        if (read(clifd, buf, MAX_DATA_SIZE) < 0)
        {
            perror("read");
            exit(1);
        }

        printf("rcvMsg: %s", buf);
        close(clifd);
    }

    return 0;
}

