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

#include <iostream>

#define SERVPORT            3838
#define BACKLOG             10
#define MAX_CONNECTED_NO    10
#define MAXDATASIZE         100


int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        exit(1);
    }
    std::cout<<"socket success , socketfd = " << sockfd << std::endl;

    struct sockaddr_in srv_addr;
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(SERVPORT);
    srv_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(srv_addr.sin_zero), 8);

    if (bind(sockfd, (struct sockaddr*)&srv_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind");
        exit(1);
    }
    std::cout<<"bind success "<< std::endl;

    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }
    std::cout<<"listen success "<< std::endl;

    fd_set  readfd;
    FD_ZERO(&readfd);           //清空 readfd;
    FD_SET(sockfd, &readfd);    //将sockfd 加入到readfd中
    while (1)
    {
        socklen_t sin_size = sizeof(struct sockaddr_in);
        std::cout<<"select ..."<< std::endl;

        if (select(MAX_CONNECTED_NO, &readfd, NULL, NULL, (struct timeval*)0) > 0)
        {
            int clifd, rcvbytes;
            if (FD_ISSET(sockfd, &readfd) > 0)
            {
                struct sockaddr_in cli_addr;
                if((clifd = accept(sockfd, (struct sockaddr*)&cli_addr, &sin_size)) == -1)
                {
                    perror("accept");
                    exit(1);
                }
                char buf[MAXDATASIZE] = {0};
                if (rcvbytes = recv(clifd, buf, MAXDATASIZE, 0) == -1)
                {
                    perror("recv");
                    exit(1);
                }
                if (read(clifd, buf, MAXDATASIZE) < 0)
                {
                    perror("read");
                    exit(1);
                }
                std::cout<<"receive: "<< buf << std::endl;
            }
            close(clifd);
        }
    }
    return 0;
}
