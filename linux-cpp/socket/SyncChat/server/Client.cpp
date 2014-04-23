//from : http://zhoulifa.bokee.com/6067924.html

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

#define MAXBUF 1024
/************关于本文档********************************************
// *filename: sync-client.c
 *purpose: 演示网络同步通讯，这是客户端程序
 *wrote by: zhoulifa(zhoulifa@163.com) 周立发(http://zhoulifa.bokee.com)
 Linux爱好者 Linux知识传播者 SOHO族 开发者 最擅长C语言
 *date time:2007-01-25 20:32
 *Note: 任何人可以任意复制代码并运用这些文档，当然包括你的商业用途
 * 但请遵循GPL
 * *Thanks to: Google.com
 * *Hope:希望越来越多的人贡献自己的力量，为科学技术发展出力
 * * 科技站在巨人的肩膀上进步更快！感谢有开源前辈的贡献！
 * *********************************************************************/

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        exit(errno);
    }
    std::cout << "socket created" << std::endl;

    struct sockaddr_in dest;
    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = 3838;
    dest.sin_addr.s_addr = inet_addr("192.168.122.230");
    std::cout << "address created" << std::endl;

    if (connect(sockfd, (struct sockaddr*)&dest, sizeof(dest)) != 0)
    {
        perror("connect");
        exit(errno);
    }
    std::cout << "server connected" << std::endl;

    while (1)
    {
        char buf[MAXBUF + 1] = {0};
        int len = recv(sockfd, buf, MAXBUF, 0);
        if (len > 0)
            std::cout << "接收消息成功:" << buf << ", 共" << len << "字节"<< std::endl;
        else
        {
            if (len < 0)
                std::cout << "接收消息失败，错误代码：" << errno << "，信息" << strerror(errno) << std::endl;
            else
            {
                std::cout << "对方终止聊天" << std::endl;
            }
            break;
        }

        bzero(buf, MAXBUF + 1);
        std::cout << "输入聊天信息:" << std::endl;
        fgets(buf, MAXBUF, stdin);

        if (!strncasecmp(buf, "quit", 4))
        {
            std::cout << "请求终止聊天信息" << std::endl;
            break;
        }

        len = send(sockfd, buf, strlen(buf) - 1, 0);
        if (len < 0)
        {
            printf("消息'%s'发送失败！错误代码是%d，错误信息是'%s'\n",
                 buf, errno, strerror(errno));
            break;
        }
        else
        {
            printf("消息：%s\t发送成功，共发送了%d个字节！\n",
                 buf, len);
        }
    }

    close(sockfd);
    return 0;
}
