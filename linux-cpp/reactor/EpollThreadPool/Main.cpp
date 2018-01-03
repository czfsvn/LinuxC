#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <pthread.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include "thread_pool.h"
#include "thread_pool.c"
#define MAX_EVENT_NUMBER  1000 
#define SIZE    1024 
#define MAX     10 

//�����߳������߳����ݽṹ
struct fd 
{
    int epollfd; 
    int sockfd ;
};
 
//�û�˵��
struct user
{
    int  sockfd ;   //�ļ�������
    char client_buf [SIZE]; //���ݵĻ�����
};
struct user user_client[MAX];  //����һ��ȫ�ֵĿͻ����ݱ�


//����epoll���õ�EPOLLONESHOTģʽ��������errno =EAGAIN,����Ҫ���������ļ����������ɶ���
void reset_oneshot (int epollfd , int fd)
{
    struct epoll_event event ;
    event.data.fd = fd ;
    event.events = EPOLLIN|EPOLLET|EPOLLONESHOT ;
    epoll_ctl (epollfd , EPOLL_CTL_MOD, fd , &event);

}
//��epoll�ں��¼���������ӿ�д���¼�
int addreadfd (int epollfd , int fd , int oneshot)
{
    struct epoll_event  event ;
    event.data.fd = fd ;
    event.events |= ~ EPOLLIN ; 
    event.events |= EPOLLOUT ;
    event.events |= EPOLLET;
    if (oneshot)
    {
        event.events |= EPOLLONESHOT ; //����EPOLLONESHOT

    }
    epoll_ctl (epollfd , EPOLL_CTL_MOD ,fd , &event);

}
//Ⱥ�ĺ���
int groupchat (int epollfd , int sockfd , char *buf)
{
        
    int i = 0 ;
    for ( i  = 0 ; i < MAX ; i++)
    {
        if (user_client[i].sockfd == sockfd)
		{
            continue ;
        }
        strncpy (user_client[i].client_buf ,buf , strlen (buf)) ;
		addreadfd (epollfd , user_client[i].sockfd , 1);

        }

}
//�������ݵĺ�����Ҳ�����̵߳Ļص�����
int funcation (void *args)
{
    int sockfd = ((struct fd*)args)->sockfd ; 
    int epollfd =((struct fd*)args)->epollfd;
    char buf[SIZE];
    memset (buf , '\0', SIZE);
    
    printf ("start new thread to receive data on fd :%d\n", sockfd);
    
    //�����ҽ�epoll�Ĺ���ģʽ����ΪETģʽ�����Ծ�Ҫ��һ��ѭ������ȡ���ݣ���ֹ����û�ж��꣬����ʧ��
    while (1)
    {
        int ret = recv (sockfd ,buf , SIZE-1 , 0);
        if (ret == 0)
        {
            close (sockfd);
            break;
        }
        else if (ret < 0)
        {
            if (errno == EAGAIN)
                {
                reset_oneshot (epollfd, sockfd);  //�������ã������Ѿ������ˣ�
                break;
            }
        }
        else
        {
            printf (" read data is %s\n", buf);
            sleep (5);
                groupchat (epollfd , sockfd, buf );
        }
        

    }
    printf ("end thread receive  data on fd : %d\n", sockfd);
    
}
//��������ע�ᣬ���ļ��������ӿ�д��ɿɶ�
int addagainfd (int epollfd , int fd)
{
       struct epoll_event event;
       event.data.fd = fd ;
       event.events  |= ~EPOLLOUT ;
       event.events = EPOLLIN|EPOLLET|EPOLLONESHOT;
       epoll_ctl (epollfd , EPOLL_CTL_MOD , fd , &event);
}
//��ǰ��Ľ���һ��
int reset_read_oneshot (int epollfd , int sockfd)
{
    struct epoll_event  event;
    event.data.fd = sockfd ;
    event.events = EPOLLOUT |EPOLLET |EPOLLONESHOT ;
    epoll_ctl (epollfd, EPOLL_CTL_MOD , sockfd , &event);
    return 0 ;

}

//���Ͷ�������
int readfun (void *args)
{
       int sockfd = ((struct fd *)args)->sockfd ;
       int epollfd= ((struct fd*)args)->epollfd ;
        
       int ret = send (sockfd, user_client[sockfd].client_buf , strlen (user_client[sockfd].client_buf), 0); //��������
       if (ret == 0 )
       {
           
           close (sockfd);
           printf ("��������ʧ��\n");
           return -1 ;
       }
       else if (ret == EAGAIN)
       {
           reset_read_oneshot (epollfd , sockfd);
           printf("send later\n");
           return -1;
       }
       memset (&user_client[sockfd].client_buf , '\0', sizeof (user_client[sockfd].client_buf));
       addagainfd (epollfd , sockfd);//���������ļ�������
     
}
//�׽�������Ϊ������
int setnoblocking (int fd)
{
    int old_option = fcntl (fd, F_GETFL);
    int new_option = old_option|O_NONBLOCK;
    fcntl (fd , F_SETFL , new_option);
    return old_option ;
}

int addfd (int epollfd , int fd , int oneshot)
{
    struct epoll_event  event;
    event.data.fd = fd ;
    event.events = EPOLLIN|EPOLLET ;
    if (oneshot)
    {
        event.events |= EPOLLONESHOT ;

    }
    epoll_ctl (epollfd , EPOLL_CTL_ADD ,fd ,  &event);
    setnoblocking (fd);
    return 0 ;
}



int main(int argc, char *argv[])
{
    struct sockaddr_in  address ;
    const char *ip = "127.0.0.1";
    int port  = 8086 ;

    memset (&address , 0 , sizeof (address));
    address.sin_family = AF_INET ;
    inet_pton (AF_INET ,ip , &address.sin_addr);
    address.sin_port =htons( port) ;


    int listenfd = socket (AF_INET, SOCK_STREAM, 0);
    assert (listen >=0);
    int reuse = 1;
        setsockopt (listenfd , SOL_SOCKET , SO_REUSEADDR , &reuse , sizeof (reuse)); //�˿����ã���Ϊ���ֹ��˿��޷��󶨵Ĵ���
    int ret = bind (listenfd, (struct sockaddr*)&address , sizeof (address));
    assert (ret >=0 );

    ret = listen (listenfd , 5);
        assert (ret >=0);
        

        struct epoll_event events[MAX_EVENT_NUMBER];
        
    int epollfd = epoll_create (5); //�����ں��¼���������
    assert (epollfd != -1);
    addfd (epollfd , listenfd, 0);
        
        thpool_t  *thpool ;  //�̳߳�
    thpool = thpool_init (5) ; //�̳߳ص�һ����ʼ��
        
        while (1)
    {
        int ret = epoll_wait (epollfd, events, MAX_EVENT_NUMBER , -1);//�ȴ��������ļ�����������������Ὣ�����ĸ��Ƶ�events�Ľṹ�������С�
        if (ret < 0)
        {
            printf ("poll failure\n");
            break ; 
        }
                int i =0  ;
        for ( i = 0 ; i < ret ; i++ )
        {
            int sockfd = events[i].data.fd ;

            if (sockfd == listenfd)
            {
                struct sockaddr_in client_address ;
                socklen_t  client_length = sizeof (client_address);
                int connfd = accept (listenfd , (struct sockaddr*)&client_address,&client_length);
                user_client[connfd].sockfd = connfd ;
                memset (&user_client[connfd].client_buf , '\0', sizeof (user_client[connfd].client_buf));
                addfd (epollfd , connfd , 1);//���µ��׽��ּ��뵽�ں��¼������档
            }
            else if (events[i].events & EPOLLIN) 
            {
                 struct fd    fds_for_new_worker ;
                 fds_for_new_worker.epollfd = epollfd ; 
                 fds_for_new_worker.sockfd = sockfd ;
                
                  thpool_add_work (thpool, (void*)funcation ,&fds_for_new_worker);//��������ӵ�����������
            }
			else if (events[i].events & EPOLLOUT)
            {
                
                                 struct  fd   fds_for_new_worker ;
                 fds_for_new_worker.epollfd = epollfd ;
                 fds_for_new_worker.sockfd = sockfd ;
                                 thpool_add_work (thpool, (void*)readfun , &fds_for_new_worker );//��������ӵ�����������
            }
                        
        }

    }
                            
    thpool_destory (thpool);
    close (listenfd);
        return EXIT_SUCCESS;
}