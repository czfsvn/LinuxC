//from : http://zyg0227.blog.51cto.com/1043164/554208
//

/*ʹ��select���������Է������ķ�ʽ�Ͷ��socketͨ�š�����ֻ����ʾselect������ʹ�ã���ʹĳ�����ӹر��Ժ�Ҳ�����޸ĵ�ǰ���������������ﵽ���ֵ�����ֹ����
 * 1. ����ʹ����һ������fd��ͨ�ſ�ʼ�����Ҫͨ�ŵĶ��socket�����������������
 * 2. ��������һ����sock_fd��socket�����������ڼ����˿ڡ�
 * 3. ��sock_fd������fd�в�Ϊ0������������select�����ļ���fdsr��
 * 4. ����fdsr�п��Խ������ݵ����ӡ������sock_fd�������������Ӽ��룬���¼������ӵ�socket���������õ�fd�� */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MYPORT 3838//����ʱʹ�õĶ˿�

#define MAXCLINE 5 //���Ӷ����еĸ���

#define BUF_SIZE 200

int fd[MAXCLINE]; //���ӵ�fd

int conn_amount; //��ǰ��������

void showclient()
{
    int i;
    printf("client amount:%d\n",conn_amount);
    for(i=0;i<MAXCLINE;i++)
    {
        printf("[%d]:%d ",i,fd[i]);
    }
    printf("\n\n");
}


int run_1(void)
{
    int sock_fd,new_fd; //�����׽��� �����׽���
    struct sockaddr_in server_addr; // �������ĵ�ַ��Ϣ
    struct sockaddr_in client_addr; //�ͻ��˵ĵ�ַ��Ϣ

    socklen_t sin_size;
    int yes = 1;
    char buf[BUF_SIZE];
    int ret;
    int i;
    //����sock_fd�׽���

    if((sock_fd = socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        perror("setsockopt");
        exit(1);
    }
    //�����׽ӿڵ�ѡ�� SO_REUSEADDR ������ͬһ���˿������������Ķ��ʵ��

    // setsockopt�ĵڶ�������SOL SOCKET ָ��ϵͳ�У�����ѡ��ļ��� ��ͨ�׽���

    if(setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1)
    {
        perror("setsockopt error \n");
        exit(1);
    }

    server_addr.sin_family = AF_INET; //�����ֽ���
    server_addr.sin_port = htons(MYPORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;//ͨ��IP
    memset(server_addr.sin_zero,'\0',sizeof(server_addr.sin_zero));

    if(bind(sock_fd,(struct sockaddr *)&server_addr,sizeof(server_addr)) == -1)
    {
        perror("bind error!\n");
        exit(1);
    }

    if(listen(sock_fd,MAXCLINE)==-1)
    {
        perror("listen error!\n");
        exit(1);
    }

    printf("listen port %d\n",MYPORT);

    fd_set fdsr; //�ļ����������Ķ���

    int maxsock;
    struct timeval tv;

    conn_amount =0;
    sin_size = sizeof(client_addr);
    maxsock = sock_fd;

    while(1)
    {
        //��ʼ���ļ�����������
        FD_ZERO(&fdsr); //�����������
        FD_SET(sock_fd,&fdsr); //��sock_fd������������

        //��ʱ���趨
        tv.tv_sec = 30;
        tv.tv_usec =0;

        //��ӻ������
        for(i=0;i<MAXCLINE;i++)
        {
            if(fd[i]!=0)
            {
                FD_SET(fd[i],&fdsr);
            }
        }

        //����ļ������������������� ������Ӧ�Ĵ���ʵ��I/O�ĸ��� ���û�������ͨѶ
        ret = select(maxsock +1,&fdsr,NULL,NULL,&tv);
        if(ret <0) //û���ҵ���Ч������ ʧ��
        {
            perror("select error!\n");
            break;
        }
        else if(ret ==0)// ָ����ʱ�䵽��
        {
            printf("timeout \n");
            continue;
        }

        //ѭ���ж���Ч�������Ƿ������ݵ���
        for(i=0;i<conn_amount;i++)
        {
            if(FD_ISSET(fd[i],&fdsr))
            {
                ret = recv(fd[i],buf,sizeof(buf),0);
                if(ret <=0) //�ͻ������ӹرգ�����ļ����������е���Ӧ��λ
                {
                    printf("client[%d] close\n",i);
                    close(fd[i]);
                    FD_CLR(fd[i],&fdsr);
                    fd[i]=0;
                    conn_amount--;

                }
                //��������Ӧ�����ݷ��͹��� ��������Ӧ�Ĵ���
                else
                {
                    if(ret <BUF_SIZE)
                        memset(&buf[ret],'\0',1);
                    printf("client[%d] send:%s\n",i,buf);
                }
            }
        }

        if(FD_ISSET(sock_fd,&fdsr))
        {
            new_fd = accept(sock_fd,(struct sockaddr *)&client_addr,&sin_size);
            if(new_fd <=0)
            {
                perror("accept error\n");
                continue;
            }
            //����µ�fd �������� �ж���Ч���������Ƿ�С�����������������С�ڵĻ����Ͱ��µ������׽��ּ��뼯��

            if(conn_amount <MAXCLINE)
            {
                for(i=0;i< MAXCLINE;i++)
                {
                    if(fd[i]==0)
                    {
                        fd[i] = new_fd;
                        break;
                    }
                }
                conn_amount++;
                printf("new connection client[%d]%s:%d\n",conn_amount,inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
                if(new_fd > maxsock)
                {
                    maxsock = new_fd;
                }
            }
            else
            {
                printf("max connections arrive ,exit\n");
                send(new_fd,"bye",4,0);
                close(new_fd);
                continue;
            }
        }
        showclient();
    }

    for(i=0;i<MAXCLINE;i++)
    {
        if(fd[i]!=0)
        {
            close(fd[i]);
        }
    }

    exit(0);
}

#if 0

int run_2()
{
    char msg[BUF_SIZE];
    int Listen_socket,ret,on;
    struct sockaddr_in local_addr;
    struct sockaddr_in client_addr;
    int i;
    fd_set fdsr; //�ļ����������Ķ���
    socklen_t addr_size;
    addr_size = sizeof(struct sockaddr_in);

    int conn_amount = 0; //��ǰ����Ծ������
    int new_fd;
    struct timeval tv;

    //����socket�׽���
    if( (Listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        emit err_msg_signal("failed create socket");
    }

    //bind API �����������ַ����������
    on = 1;
    ret = setsockopt( Listen_socket, SOL_SOCKET, SO_REUSEADDR,
            &on, sizeof(on) );

    int nNetTimeout=2000;//2��
    //���÷���ʱ��
    setsockopt(Listen_socket,SOL_SOCKET,SO_SNDTIMEO,(char *)&nNetTimeout,sizeof(int) );
    ////���ý���ʱ��
    setsockopt(Listen_socket,SOL_SOCKET,SO_RCVTIMEO,(char *)&nNetTimeout,sizeof(int));
    //���ñ�����������
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(port);
    local_addr.sin_addr.s_addr = INADDR_ANY;

    //while(flag_port == 0)
    //�󶨱���IP�Ͷ˿ں�
    if(bind(Listen_socket, (struct sockaddr*)&local_addr, sizeof(struct sockaddr)) == -1)
    {
        emit err_msg_signal("failed bind");
    }

    //�����ͻ�������
    if(listen(Listen_socket, 8) == -1)
    {
        emit err_msg_signal("failed listen");
    }

    QTime current_time;
    current_time = QTime::currentTime();
    int flag_minutechange = 0,lastminute = current_time.currentTime().minute();
    int maxsock = Listen_socket;

    while (1)
    {

        if( current_time.currentTime().minute() != lastminute) //ÿ��ѭ����ʼ����ȡϵͳʱ�䣬���ϴη������Ƚϣ�Ϊ���³�ʱ�ж��ṩ����
        {
            lastminute = current_time.currentTime().minute();
            flag_minutechange = 1;
        }

        FD_ZERO(&fdsr); //ÿ�ν���ѭ�����ؽ���������
        FD_SET(Listen_socket, &fdsr);
        for (i = 0; i < MAXCLIENT; i++) //�����ڵ��׽��ּ�����������
        {
            if (fd[i] != 0)
            {
                FD_SET(fd[i], &fdsr);
                if(flag_minutechange == 1)
                {
                    con_time[i]--;
                    if(con_time[i] <= 0)
                    {
                        close(fd[i]);
                        FD_CLR(fd[i], &fdsr);
                        fd[i] = 0;
                        conn_amount--;
                    }
                }
            }
        }
        flag_minutechange = 0;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        ret = select(maxsock + 1, &fdsr, NULL, NULL,&tv); //�ؼ���select()����������̽����׽��ֵ��쳣
        //������ļ���������������������������󣬻�����Ӧ����
        //�Ӷ��ɹ��Ľ���˵��߳�������������̵������ʵ�ֶ��û�������ͨ��
        if (ret < 0) //<0��ʾ̽��ʧ��
        {
            qDebug()<<"failed select\n";
            break;
        }
        else if (ret == 0) //=0��ʾ��ʱ����һ��ѭ��
        {
            //qDebug()<<"timeout\n";
            continue;

            // ���select�������쳣��ѭ���жϸ���Ծ�����Ƿ������ݵ���
            for (i = 0; i < conn_amount; i++)
            {
                if (FD_ISSET(fd[i], &fdsr))
                {
                    ret = recv(fd[i], msg, BUF_SIZE, 0);
                    if (ret <= 0) // recv<=0,�����ͻ��˹ر����ӣ�������Ҳ�ر���Ӧ���ӣ����������׽��Ӵ��ļ��������������
                    {
                        qDebug("client[%d] close\n", i);
                        close(fd[i]);
                        FD_CLR(fd[i], &fdsr);
                        fd[i] = 0;
                        conn_amount--;
                    }
                    else //��������ͻ��������ݷ��͹���������Ӧ���ܴ���
                    {
                        con_time[i] = MAX_IDLECONNCTIME; //����д��fd[i]�ĳ�ʱ�����ٴ�֮�����MAX_IDLECONNCTIME�����ڴ������޷�Ӧ����������رո�����
                        if (ret < BUF_SIZE)
                            emit err_msg_signal("client ip: " + QString::fromLatin1(inet_ntoa(client_addr.sin_addr)) +
                                    " port: " + QString::number(ntohs(client_addr.sin_port))+" coming data");
                        qDebug("client[%d] send:%s\n", i, msg);
                        msg[ret] = '\0';
                        emit recv_msg_signal(QString::fromLatin1(msg),fd[i]);
                        //send(fd[i],msg,ret,0);
                    }
                }
            }
            // ����˵���쳣�����Կͻ��˵���������
            if (FD_ISSET(Listen_socket, &fdsr))
            {
                new_fd = accept(Listen_socket, (struct sockaddr *)&client_addr, &addr_size);
                if (new_fd <= 0)
                {
                    qDebug("failed accept");
                    continue;
                }

                // �жϻ�Ծ������ʱ���Ƿ�С�����������������ǣ���������ӵ��ļ�����������
                if (conn_amount < MAXCLIENT)
                {
                    for(i = 0;i < MAXCLIENT;i++)
                    {
                        if(fd[i] == 0)
                        {
                            fd[i] = new_fd;
                            con_time[i] = MAX_IDLECONNCTIME; //ÿ���½������ӣ����ø����ӵĳ�ʱ������������Ӵ˺�MAX_IDLECONNCTIME�������޷�Ӧ���رո�����
                            break;
                        }

                    }
                    conn_amount++;
                    //fd[conn_amount++] = new_fd;
                    qDebug("new connection client[%d] %s:%d\n", conn_amount,
                            inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    emit err_msg_signal("client ip: " +
                            QString::fromLatin1(inet_ntoa(client_addr.sin_addr)) +
                            " , port: " +
                            QString::number(ntohs(client_addr.sin_port)));
                    if (new_fd > maxsock)
                        maxsock = new_fd;
                }
                else
                {
                    qDebug("MAXCLIENT arrive, exit\n");
                    send(new_fd, "over MAXCLIENT\n", 25, 0);
                    close(new_fd);
                    continue;
                }
            }
        }
    }
}
#endif


int main()
{
    run_1();
    //run_2();
    return 0;
}
