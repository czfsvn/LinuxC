//from: http://zhoulifa.bokee.com/6067924.html

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <iostream>

#define MAXBUF 1024



/************关于本文档********************************************
 * *filename: sync-server.c
 * *purpose: 演示网络同步通讯，这是服务器端程序
 * *wrote by: zhoulifa(zhoulifa@163.com) 周立发(http://zhoulifa.bokee.com)
 * Linux爱好者 Linux知识传播者 SOHO族 开发者 最擅长C语言
 * *date time:2007-01-25 20:26
 * *Note: 任何人可以任意复制代码并运用这些文档，当然包括你的商业用途
 * * 但请遵循GPL
 * *Thanks to: Google.com
 * *Hope:希望越来越多的人贡献自己的力量，为科学技术发展出力
 * * 科技站在巨人的肩膀上进步更快！感谢有开源前辈的贡献！
 * *********************************************************************/

int main()
{
    unsigned int myport = 3838, lisnum = 2;

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in my_addr;
    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = PF_INET;
    my_addr.sin_port = htons(myport);
    my_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind");
        exit(1);
    }

    if (listen(sockfd, lisnum) == -1)
    {
        perror("listen");
        exit(1);
    }

    while (1)
    {
        std::cout << "等待新的连接开始新的聊天..." << std::endl;
        socklen_t len = sizeof(struct sockaddr);
        struct sockaddr_in cli_addr;
        int new_fd = accept(sockfd, (struct sockaddr *) &cli_addr, &len);
        if (new_fd == -1)
        {
            perror("accept");
            exit(1);
        }

        std::cout << "connect from " << inet_ntoa(cli_addr.sin_addr);
        std::cout << "port: " << ntohs(cli_addr.sin_port) << ", socket" << new_fd;

        while (1)
        {
            char buf[MAXBUF + 1] = {0};
            std::cout << "输入聊天信息:" << std::endl;
            fgets(buf, MAXBUF, stdin);
            if (!strncasecmp(buf, "quit", 4))
            {
                std::cout << "请求终止聊天信息" << std::endl;
                break;
            }

            int length = send(new_fd, buf, strlen(buf) - 1, 0);
            if (length)
                std::cout << "消息:" <<buf << "发送成功，共发送了"<< length << "字节\n";
            else if (length < 0)
                std::cout << "消息:" <<buf << "发送失败，错误代码:"<<errno << "信息:" << strerror(errno);
            else
            {
                std::cout << "对方退出聊天"<< std::endl;
                break;
            }
        }

        close(new_fd);
    }
    close(sockfd);
    return 0;
}
