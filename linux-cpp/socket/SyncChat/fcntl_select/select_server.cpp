//from : http://zyg0227.blog.51cto.com/1043164/554208
//

/*使用select函数可以以非阻塞的方式和多个socket通信。程序只是演示select函数的使用，即使某个连接关闭以后也不会修改当前连接数，连接数达到最大值后会终止程序。
 * 1. 程序使用了一个数组fd，通信开始后把需要通信的多个socket描述符都放入此数组
 * 2. 首先生成一个叫sock_fd的socket描述符，用于监听端口。
 * 3. 将sock_fd和数组fd中不为0的描述符放入select将检查的集合fdsr。
 * 4. 处理fdsr中可以接收数据的连接。如果是sock_fd，表明有新连接加入，将新加入连接的socket描述符放置到fd。 */

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

#define MYPORT 3838//连接时使用的端口

#define MAXCLINE 5 //连接队列中的个数

#define BUF_SIZE 200

int fd[MAXCLINE]; //连接的fd

int conn_amount; //当前的连接数

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
    int sock_fd,new_fd; //监听套接字 连接套接字
    struct sockaddr_in server_addr; // 服务器的地址信息
    struct sockaddr_in client_addr; //客户端的地址信息

    socklen_t sin_size;
    int yes = 1;
    char buf[BUF_SIZE];
    int ret;
    int i;
    //建立sock_fd套接字

    if((sock_fd = socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        perror("setsockopt");
        exit(1);
    }
    //设置套接口的选项 SO_REUSEADDR 允许在同一个端口启动服务器的多个实例

    // setsockopt的第二个参数SOL SOCKET 指定系统中，解释选项的级别 普通套接字

    if(setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1)
    {
        perror("setsockopt error \n");
        exit(1);
    }

    server_addr.sin_family = AF_INET; //主机字节序
    server_addr.sin_port = htons(MYPORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;//通配IP
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

    fd_set fdsr; //文件描述符集的定义

    int maxsock;
    struct timeval tv;

    conn_amount =0;
    sin_size = sizeof(client_addr);
    maxsock = sock_fd;

    while(1)
    {
        //初始化文件描述符集合
        FD_ZERO(&fdsr); //清除描述符集
        FD_SET(sock_fd,&fdsr); //把sock_fd加入描述符集

        //超时的设定
        tv.tv_sec = 30;
        tv.tv_usec =0;

        //添加活动的连接
        for(i=0;i<MAXCLINE;i++)
        {
            if(fd[i]!=0)
            {
                FD_SET(fd[i],&fdsr);
            }
        }

        //如果文件描述符中有连接请求 会做相应的处理，实现I/O的复用 多用户的连接通讯
        ret = select(maxsock +1,&fdsr,NULL,NULL,&tv);
        if(ret <0) //没有找到有效的连接 失败
        {
            perror("select error!\n");
            break;
        }
        else if(ret ==0)// 指定的时间到，
        {
            printf("timeout \n");
            continue;
        }

        //循环判断有效的连接是否有数据到达
        for(i=0;i<conn_amount;i++)
        {
            if(FD_ISSET(fd[i],&fdsr))
            {
                ret = recv(fd[i],buf,sizeof(buf),0);
                if(ret <=0) //客户端连接关闭，清除文件描述符集中的相应的位
                {
                    printf("client[%d] close\n",i);
                    close(fd[i]);
                    FD_CLR(fd[i],&fdsr);
                    fd[i]=0;
                    conn_amount--;

                }
                //否则有相应的数据发送过来 ，进行相应的处理
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
            //添加新的fd 到数组中 判断有效的连接数是否小于最大的连接数，如果小于的话，就把新的连接套接字加入集合

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
    fd_set fdsr; //文件描述符集的定义
    socklen_t addr_size;
    addr_size = sizeof(struct sockaddr_in);

    int conn_amount = 0; //当前最大活跃连接数
    int new_fd;
    struct timeval tv;

    //建立socket套接字
    if( (Listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        emit err_msg_signal("failed create socket");
    }

    //bind API 函数将允许地址的立即重用
    on = 1;
    ret = setsockopt( Listen_socket, SOL_SOCKET, SO_REUSEADDR,
            &on, sizeof(on) );

    int nNetTimeout=2000;//2秒
    //设置发送时限
    setsockopt(Listen_socket,SOL_SOCKET,SO_SNDTIMEO,(char *)&nNetTimeout,sizeof(int) );
    ////设置接收时限
    setsockopt(Listen_socket,SOL_SOCKET,SO_RCVTIMEO,(char *)&nNetTimeout,sizeof(int));
    //设置本机服务类型
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(port);
    local_addr.sin_addr.s_addr = INADDR_ANY;

    //while(flag_port == 0)
    //绑定本机IP和端口号
    if(bind(Listen_socket, (struct sockaddr*)&local_addr, sizeof(struct sockaddr)) == -1)
    {
        emit err_msg_signal("failed bind");
    }

    //监听客户端连接
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

        if( current_time.currentTime().minute() != lastminute) //每次循环开始都读取系统时间，与上次分钟数比较，为以下超时判断提供依据
        {
            lastminute = current_time.currentTime().minute();
            flag_minutechange = 1;
        }

        FD_ZERO(&fdsr); //每次进入循环都重建描述符集
        FD_SET(Listen_socket, &fdsr);
        for (i = 0; i < MAXCLIENT; i++) //将存在的套接字加入描述符集
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
        ret = select(maxsock + 1, &fdsr, NULL, NULL,&tv); //关键的select()函数，用来探测各套接字的异常
        //如果在文件描述符集中有连接请求或发送请求，会作相应处理，
        //从而成功的解决了单线程情况下阻塞进程的情况，实现多用户连接与通信
        if (ret < 0) //<0表示探测失败
        {
            qDebug()<<"failed select\n";
            break;
        }
        else if (ret == 0) //=0表示超时，下一轮循环
        {
            //qDebug()<<"timeout\n";
            continue;

            // 如果select发现有异常，循环判断各活跃连接是否有数据到来
            for (i = 0; i < conn_amount; i++)
            {
                if (FD_ISSET(fd[i], &fdsr))
                {
                    ret = recv(fd[i], msg, BUF_SIZE, 0);
                    if (ret <= 0) // recv<=0,表明客户端关闭连接，服务器也关闭相应连接，并把连接套接子从文件描述符集中清除
                    {
                        qDebug("client[%d] close\n", i);
                        close(fd[i]);
                        FD_CLR(fd[i], &fdsr);
                        fd[i] = 0;
                        conn_amount--;
                    }
                    else //否则表明客户端有数据发送过来，作相应接受处理
                    {
                        con_time[i] = MAX_IDLECONNCTIME; //重新写入fd[i]的超时数，再此之后如果MAX_IDLECONNCTIME分钟内此连接无反应，服务器会关闭该连接
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
            // 以下说明异常有来自客户端的连接请求
            if (FD_ISSET(Listen_socket, &fdsr))
            {
                new_fd = accept(Listen_socket, (struct sockaddr *)&client_addr, &addr_size);
                if (new_fd <= 0)
                {
                    qDebug("failed accept");
                    continue;
                }

                // 判断活跃连接数时候是否小于最大连接数，如果是，添加新连接到文件描述符集中
                if (conn_amount < MAXCLIENT)
                {
                    for(i = 0;i < MAXCLIENT;i++)
                    {
                        if(fd[i] == 0)
                        {
                            fd[i] = new_fd;
                            con_time[i] = MAX_IDLECONNCTIME; //每次新建立连接，设置该连接的超时数，如果此连接此后MAX_IDLECONNCTIME分钟内无反应，关闭该连接
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
