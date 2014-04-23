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



/************���ڱ��ĵ�********************************************
 * *filename: sync-server.c
 * *purpose: ��ʾ����ͬ��ͨѶ�����Ƿ������˳���
 * *wrote by: zhoulifa(zhoulifa@163.com) ������(http://zhoulifa.bokee.com)
 * Linux������ Linux֪ʶ������ SOHO�� ������ ���ó�C����
 * *date time:2007-01-25 20:26
 * *Note: �κ��˿������⸴�ƴ��벢������Щ�ĵ�����Ȼ���������ҵ��;
 * * ������ѭGPL
 * *Thanks to: Google.com
 * *Hope:ϣ��Խ��Խ����˹����Լ���������Ϊ��ѧ������չ����
 * * �Ƽ�վ�ھ��˵ļ���Ͻ������죡��л�п�Դǰ���Ĺ��ף�
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
        std::cout << "�ȴ��µ����ӿ�ʼ�µ�����..." << std::endl;
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
            std::cout << "����������Ϣ:" << std::endl;
            fgets(buf, MAXBUF, stdin);
            if (!strncasecmp(buf, "quit", 4))
            {
                std::cout << "������ֹ������Ϣ" << std::endl;
                break;
            }

            int length = send(new_fd, buf, strlen(buf) - 1, 0);
            if (length)
                std::cout << "��Ϣ:" <<buf << "���ͳɹ�����������"<< length << "�ֽ�\n";
            else if (length < 0)
                std::cout << "��Ϣ:" <<buf << "����ʧ�ܣ��������:"<<errno << "��Ϣ:" << strerror(errno);
            else
            {
                std::cout << "�Է��˳�����"<< std::endl;
                break;
            }
        }

        close(new_fd);
    }
    close(sockfd);
    return 0;
}
