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
/************���ڱ��ĵ�********************************************
// *filename: sync-client.c
 *purpose: ��ʾ����ͬ��ͨѶ�����ǿͻ��˳���
 *wrote by: zhoulifa(zhoulifa@163.com) ������(http://zhoulifa.bokee.com)
 Linux������ Linux֪ʶ������ SOHO�� ������ ���ó�C����
 *date time:2007-01-25 20:32
 *Note: �κ��˿������⸴�ƴ��벢������Щ�ĵ�����Ȼ���������ҵ��;
 * ������ѭGPL
 * *Thanks to: Google.com
 * *Hope:ϣ��Խ��Խ����˹����Լ���������Ϊ��ѧ������չ����
 * * �Ƽ�վ�ھ��˵ļ���Ͻ������죡��л�п�Դǰ���Ĺ��ף�
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
            std::cout << "������Ϣ�ɹ�:" << buf << ", ��" << len << "�ֽ�"<< std::endl;
        else
        {
            if (len < 0)
                std::cout << "������Ϣʧ�ܣ�������룺" << errno << "����Ϣ" << strerror(errno) << std::endl;
            else
            {
                std::cout << "�Է���ֹ����" << std::endl;
            }
            break;
        }

        bzero(buf, MAXBUF + 1);
        std::cout << "����������Ϣ:" << std::endl;
        fgets(buf, MAXBUF, stdin);

        if (!strncasecmp(buf, "quit", 4))
        {
            std::cout << "������ֹ������Ϣ" << std::endl;
            break;
        }

        len = send(sockfd, buf, strlen(buf) - 1, 0);
        if (len < 0)
        {
            printf("��Ϣ'%s'����ʧ�ܣ����������%d��������Ϣ��'%s'\n",
                 buf, errno, strerror(errno));
            break;
        }
        else
        {
            printf("��Ϣ��%s\t���ͳɹ�����������%d���ֽڣ�\n",
                 buf, len);
        }
    }

    close(sockfd);
    return 0;
}
