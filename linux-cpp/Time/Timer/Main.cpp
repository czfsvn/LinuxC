#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

using namespace std;

namespace ns_time_socket
{
    int timeout_connect(const char *ip, int port, int time)
    {
        int ret = 0;
        struct sockaddr_in address;
        bzero(&address, sizeof(address));
        address.sin_family = AF_INET;
        inet_pton(AF_INET, ip, &address.sin_addr);
        address.sin_port = htons(port);
        int sockfd = socket(PF_INET, SOCK_STREAM, 0);
        assert(sockfd >= 0); /*ͨ��ѡ�� SO_RCVTIMEO �� SO_SNDTIMEO �����õĳ�ʱʱ��������� timeval�� ��� select ϵͳ���õĳ�ʱ����������ͬ*/
        struct timeval timeout;
        timeout.tv_sec = time;
        timeout.tv_usec = 0;
        socklen_t len = sizeof(timeout);
        ret = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, len);
        assert(ret != -1);
        ret = connect(sockfd, (struct sockaddr *)&address, sizeof(address));
        if (ret == -1)
        { /*��ʱ ��Ӧ �� ���� �� �� EINPROGRESS�� ���� ��� ���� ��� ������ ���� �Ϳ��� ���� ��ʱ ���� ��*/
            if (errno == EINPROGRESS)
            {
                printf("connecting timeout, process timeout logic\ n");
                return -1;
            }
            printf("error occur when connecting to server\ n");
            return -1;
        }
        return sockfd;
    }

    int Main(int argc, char *argv[])
    {
        if (argc <= 2)
        {
            printf(" usage:% s ip_ address port_ number\ n", basename(argv[0]));
            return 1;
        }
        const char *ip = argv[1];
        int port = atoi(argv[2]);
        int sockfd = timeout_connect(ip, port, 10);
        if (sockfd < 0)
        {
            return;
        }
    }
} // namespace ns_time_socket

namespace ns_time2_list
{
#define BUFFER_SIZE 64
    class util_timer; /* ǰ������*/
    /*
    �û����� �ṹ��
    �ͻ� �� socket ��ַ�� socket �ļ� ���� ���� �� ���� �� ��ʱ��
    */
    struct client_data
    {
        sockaddr_in address;
        int sockfd;
        char buf[BUFFER_SIZE];
        util_timer *timer;
    }; /*��ʱ�� ��*/
    class util_timer
    {
    public:
        util_timer() : prev(NULL), next(NULL) {}

    public:
        time_t expire;                                       /* ����ĳ�ʱʱ�䣬 ����ʹ�þ���ʱ��*/
        void (*cb_func)(client_data *); /* ���� �� �� ����*/ /*�� �� ���� ���� �� �ͻ� ���ݣ� �� ��ʱ�� �� ִ���� �� �ݸ� �� �� ����*/
        client_data *user_data;
        util_timer *prev; /* ָ�� ǰ һ�� ��ʱ��*/
        util_timer *next; /* ָ�� ��һ�� ��ʱ��*/
    };
    /*��ʱ������ ����һ������ ˫������ �Ҵ���ͷ����β�ڵ�*/
    class sort_timer_lst
    {
    private:
        util_timer *head;
        util_timer *tail;

    public:
        sort_timer_lst() : head(NULL), tail(NULL) {} /*��������ʱ��ɾ���������еĶ�ʱ��*/
        ~sort_timer_lst()
        {
            util_timer *tmp = head;
            while (tmp)
            {
                head = tmp->next;
                delete tmp;
                tmp = head;
            }
        }
        /*��Ŀ�궨ʱ�� timer ��ӵ�������*/
        void add_timer(util_timer *timer)
        {
            if (!timer)
            {
                return;
            }
            if (!head)
            {
                head = tail = timer;
                return;
            }
            /*���Ŀ�궨ʱ���ĳ�ʱʱ��С�ڵ�ǰ���������ж�ʱ���ĳ�ʱʱ�䣬 ��Ѹö�ʱ����������ͷ���� ��Ϊ�����µ�ͷ�ڵ㡣 �������Ҫ�������غ���
                add_timer(util_timer* timer, util_timer* lst_head)�� �������������к��ʵ�λ�ã� �Ա�֤�������������*/
            if (timer->expire < head->expire)
            {
                timer->next = head;
                head->prev = timer;
                head = timer;
                return;
            }
            add_timer(timer, head);
        }
        /*��ĳ����ʱ�������仯ʱ�� ������Ӧ�Ķ�ʱ���������е�λ�á� �������ֻ���Ǳ������Ķ�ʱ���ĳ�ʱʱ���ӳ�������� ���ö�ʱ����Ҫ�������β���ƶ�*/
        void adjust_timer(util_timer *timer)
        {
            if (!timer)
            {
                return;
            }
            util_timer *tmp = timer->next;
            /*�����������Ŀ�궨ʱ����������β���� ���߸ö�ʱ���µĳ�ʱֵ��ȻС������һ����ʱ���ĳ�ʱֵ�� ���õ���*/
            if (!tmp || (timer->expire < tmp->expire))
            {
                return;
            }
            /*���Ŀ�궨ʱ���������ͷ�ڵ㣬 �򽫸ö�ʱ����������ȡ�������²�������*/
            if (timer == head)
            {
                head = head->next;
                head->prev = NULL;
                timer->next = NULL;
                add_timer(timer, head);
            }
            /*���Ŀ�궨ʱ�����������ͷ�ڵ㣬 �򽫸ö�ʱ����������ȡ���� Ȼ�������ԭ������λ��֮��Ĳ���������*/
            else
            {
                timer->prev->next = timer->next;
                timer->next->prev = timer->prev;
                add_timer(timer, timer->next);
            }
        }
        /*��Ŀ�궨ʱ�� timer ��������ɾ��*/
        void del_timer(util_timer *timer)
        {
            if (!timer)
            {
                return;
            }
            /*�����������������ʾ������ֻ��һ����ʱ���� ��Ŀ�궨ʱ��*/
            if ((timer == head) && (timer == tail))
            {
                delete timer;
                head = NULL;
                tail = NULL;
                return;
            }
            /*���������������������ʱ���� ��Ŀ�궨ʱ���������ͷ��㣬 �������ͷ�������Ϊԭͷ�ڵ����һ���ڵ㣬 Ȼ��ɾ��Ŀ�궨ʱ��*/
            if (timer == head)
            {
                head = head->next;
                if ((timer == head) && (timer == tail))
                {
                    delete timer;
                    head = NULL;
                    tail = NULL;
                    return;
                }
                /*���������������������ʱ���� ��Ŀ�궨ʱ���������ͷ��㣬 �������ͷ�������Ϊԭͷ�ڵ����һ���ڵ㣬 Ȼ��ɾ��Ŀ�궨ʱ��*/
                if (timer == head)
                {
                    head = head->next;
                    head->prev = NULL;
                    delete timer;
                    return;
                }
                /*���������������������ʱ���� ��Ŀ�궨ʱ���������β��㣬 �������β�������Ϊԭβ�ڵ��ǰһ���ڵ㣬 Ȼ��ɾ��Ŀ�궨ʱ��*/
                if (timer == tail)
                {
                    tail = tail->prev;
                    tail->next = NULL;
                    delete timer;
                    return;
                }
                /*��� Ŀ�� ��ʱ�� λ�� �� �� �� �м䣬 �� �� �� ǰ�� �� ��ʱ�� ���� ������ Ȼ�� ɾ�� Ŀ�� ��ʱ��*/
                timer->prev->next = timer->next;
                timer->next->prev = timer->prev;
                delete timer;
            }
        }

        /*SIGALRM �ź� ÿ�� �� ���� ���� �� �ź� ���� ������ ��� ʹ�� ͳһ �¼� Դ�� ���� �� ������ �� ִ�� һ�� tick ������ �� ���� �� ���� ���� �� ����*/
        void tick()
        {
            if (!head)
            {
                return;
            }
            printf(" timer tick\ n");
            time_t cur = time(NULL); /* ��� ϵͳ ��ǰ �� ʱ��*/
            util_timer *tmp = head;  /*��ͷ ��� ��ʼ ���� ���� ÿ�� ��ʱ���� ֱ�� ���� һ�� ��δ ���� �� ��ʱ���� ��� �� ��ʱ�� �� ���� �߼�*/
            while (tmp)
            { /*��Ϊ ÿ�� ��ʱ�� �� ʹ�� ���� ʱ�� ��Ϊ �� ʱֵ�� ���� ���� ���� �� ��ʱ�� �� �� ʱֵ �� ϵͳ ��ǰ ʱ�䣬 �Ƚ� �� �ж� ��ʱ�� �Ƿ� ����*/
                if (cur < tmp->expire)
                {
                    break;
                }                             /*���� ��ʱ�� �� �� �� ������ �� ִ�� ��ʱ ����*/
                tmp->cb_func(tmp->user_data); /*ִ�� �� ��ʱ�� �е� ��ʱ ���� ֮�� �� ���� �� �� ���� ɾ���� �� ���� �� ��ͷ ���*/
                head = tmp->next;
                if (head)
                {
                    head->prev = NULL;
                }
                delete tmp;
                tmp = head;
            }
        }

    private:
        /*һ�� ���� �� ���� ������ ���� ���� �� add_timer ���� �� adjust_ timer ���� ���á� �� ���� ��ʾ �� Ŀ�� ��ʱ�� timer ��� �� �ڵ� lst_ head ֮�� �� ���� �� ����*/
        void add_timer(util_timer *timer, util_timer *lst_head)
        {
            util_timer *prev = lst_head;
            util_timer *tmp = prev->next; /*���� lst_ head �ڵ� ֮�� �� ���� �� �� ֱ�� �ҵ� һ�� ��ʱ ʱ�� ���� Ŀ�� ��ʱ�� �� ��ʱ ʱ�� �� �ڵ㣬 ���� Ŀ�� ��ʱ�� ���� �� �ڵ� ֮ǰ*/
            while (tmp)
            {
                if (timer->expire < tmp->expire)
                {
                    prev->next = timer;
                    timer->next = tmp;
                    tmp->prev = timer;
                    timer->prev = prev;
                    break;
                }
                prev = tmp;
                tmp = tmp->next;
            } /*��� ���� �� lst_ head �ڵ� ֮�� �� ���� �� �� ��δ �ҵ� ��ʱ ʱ�� ���� Ŀ�� ��ʱ�� �� ��ʱ ʱ�� �� �ڵ㣬 �� �� Ŀ�� ��ʱ�� ���� �� ��β ���� ���� �� ���� Ϊ �� �� �µ� β
                 �ڵ�*/
            if (!tmp)
            {
                prev->next = timer;
                timer->prev = prev;
                timer->next = NULL;
                tail = timer;
            }
        }
    };

} // namespace ns_time2_list

namespace ns_time3
{

#define FD_LIMIT 65535
#define MAX_EVENT_NUMBER 1024
#define TIMESLOT 5

    static int pipefd[2];
    /*���� ���� �嵥 11- 2 �е� ���� �� �� �� ���� ��ʱ��*/
    static sort_timer_lst timer_lst;
    static int epollfd = 0;
    int setnonblocking(int fd)
    {
        int old_option = fcntl(fd, F_GETFL);
        int new_option = old_option | O_NONBLOCK;
        fcntl(fd, F_SETFL, new_option);
        return old_option;
    }
    void addfd(int epollfd, int fd)
    {
        epoll_event event;
        event.data.fd = fd;
        event.events = EPOLLIN | EPOLLET;
        epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
        setnonblocking(fd);
    }
    void sig_handler(int sig)
    {
        int save_errno = errno;
        int msg = sig;
        send(pipefd[1], (char *)&msg, 1, 0);
        errno = save_errno;
    }
    void addsig(int sig)
    {
        struct sigaction sa;
        memset(&sa, '\0', sizeof(sa));
        sa.sa_handler = sig_handler;
        sa.sa_flags |= SA_RESTART;
        sigfillset(&sa.sa_mask);
        assert(sigaction(sig, &sa, NULL) != -1);
    }
    void timer_handler()
    {
        /*��ʱ��������ʵ���Ͼ��ǵ��� tick ����*/
        timer_lst.tick();
        /*��Ϊһ��alarm ����ֻ������һ��SIGALRM�źţ� ��������Ҫ���¶�ʱ�� �Բ��ϴ���SIGALRM �ź�*/
        alarm(TIMESLOT);
    }
    /*��ʱ���ص������� ��ɾ���ǻ���� socket ��ע���¼��� ���ر�֮*/
    void cb_func(client data *user_data)
    {
        epoll_ctl(epollfd, EPOLL_CTL_DEL, user_data->sockfd, 0);
        assert(user_data);
        close(user_data->sockfd);
        printf("close fd% d\ n", user_data->sockfd);
    }

    int Main(int argc, char *argv[])
    {
        if (argc <= 2)
        {
            printf(" usage:% s ip_address port_number\ n", basename(argv[0]));
            return 1;
        }

        const char *ip = argv[1];
        int port = atoi(argv[2]);
        int ret = 0;
        struct sockaddr_in address;
        bzero(&address, sizeof(address));
        address.sin_family = AF_INET;
        inet_pton(AF_INET, ip, &address.sin_addr);
        address.sin_port = htons(port);

        int listenfd = socket(PF_INET, SOCK_STREAM, 0);
        assert(listenfd >= 0);

        ret = bind(listenfd, (struct sockaddr *)&address, sizeof(address));
        assert(ret != -1);

        ret = listen(listenfd, 5);
        assert(ret != -1);

        epoll_event events[MAX_EVENT_NUMBER];
        int epollfd = epoll_create(5);
        assert(epollfd != -1);

        addfd(epollfd, listenfd);
        ret = socketpair(PF_UNIX, SOCK_STREAM, 0, pipefd);
        assert(ret != -1);
        setnonblocking(pipefd[1]);
        addfd(epollfd, pipefd[0]);
        /*���� �ź� ���� ����*/
        addsig(SIGALRM);
        addsig(SIGTERM);

        bool stop_server = false;
        client_data *users = new client_data[FD_LIMIT];
        bool timeout = false;
        alarm(TIMESLOT);
        /* ��ʱ*/
        while (!stop_server)
        {
            int number = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
            if ((number < 0) && (errno != EINTR))
            {
                printf("epoll failure\ n");
                break;
            }
            for (int i = 0; i < number; i++)
            {
                int sockfd = events[i].data.fd;
                /*���� �µ� �� �ͻ� ����*/
                if (sockfd == listenfd)
                {
                    struct sockaddr_in client_address;
                    socklen_t client_addrlength = sizeof(client_address);
                    int connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_addrlength);
                    addfd(epollfd, connfd);

                    users[connfd].address = client_address;
                    users[connfd].sockfd = connfd;

                    /*���� ��ʱ���� ���� �� �� �� ���� �� ��ʱ ʱ�䣬 Ȼ�� �� �� ��ʱ�� �� �û� ���ݣ� ��� �� ��ʱ�� �� �ӵ� �� �� timer_ lst ��*/
                    util_timer *timer = new util_timer;
                    timer->user_data = &users[connfd];
                    timer->cb_func = cb_func;
                    time_t cur = time(NULL);
                    timer->expire = cur + 3 * TIMESLOT;

                    users[connfd].timer = timer;
                    timer_lst.add_timer(timer);
                }
                /*���� �ź�*/
                else if ((sockfd == pipefd[0]) && (events[i].events & EPOLLIN))
                {
                    int sig;
                    char signals[1024];
                    ret = recv(pipefd[0], signals, sizeof(signals), 0);
                    if (ret == -1)
                    {
                        //handle the error
                        continue;
                    }
                    else if (ret == 0)
                    {
                        continue;
                    }
                    else
                    {
                        for (int i = 0; i < ret; ++i)
                        {
                            switch (signals[i])
                            {
                            case SIGALRM:
                            {
                                /*�� timeout ���� ��� �� ��ʱ ���� ��Ҫ ���� ���� ���� ���� ��ʱ ���� ���� ��Ϊ ��ʱ ���� �� ���ȼ� ���� �ܸߣ� ���� ���� ���� ���� ���� Ҫ�� ����*/
                                timeout = true;
                                break;
                            }
                            case SIGTERM:
                            {
                                stop_server = true;
                            }
                            }
                        }
                    }
                }
                /*���� �ͻ� ���� �� �� �յ� �� ����*/
                else if (events[i].events & EPOLLIN)
                {
                    memset(users[sockfd].buf, '\ 0', BUFFER_SIZE);
                    ret = recv(sockfd, users[sockfd].buf, BUFFER_SIZE - 1, 0);
                    printf(" get %d bytes of client data %s from %d\ n", ret, users[sockfd].buf, sockfd);
                    util_timer *timer = users[sockfd].timer;
                    if (ret < 0)
                    {
                        /*������������� ��ر����ӣ� ���Ƴ����Ӧ�Ķ�ʱ��*/
                        if (errno != EAGAIN)
                        {
                            cb_func(&users[sockfd]);
                            if (timer)
                            {
                                timer_lst.del_timer(timer);
                            }
                        }
                    }
                    else if (ret == 0)
                    {
                        /*����Է��Ѿ��ر����ӣ� ������Ҳ�ر����ӣ� ���Ƴ���Ӧ�Ķ�ʱ��*/
                        cb_func(&users[sockfd]);
                        if (timer)
                        {
                            timer_lst.del_timer(timer);
                        }
                    }
                    else
                    {
                        /*���ĳ���ͻ������������ݿɶ��� ������Ҫ���������Ӷ�Ӧ�Ķ�ʱ���� ���ӳٸ����ӱ��رյ�ʱ��*/
                        if (timer)
                        {
                            time_t cur = time(NULL);
                            timer->expire = cur + 3 * TIMESLOT;
                            printf(" adjust timer once\ n");
                            timer_lst.adjust_timer(timer);
                        }
                    }
                }
                else
                { //others
                }
            } /*�����ʱ�¼��� ��Ϊ I/O �¼��и��ߵ����ȼ��� ��Ȼ�� �����������¶�ʱ�����ܾ�ȷ�ذ���Ԥ�ڵ�ʱ��ִ��*/
            if (timeout)
            {
                timer_handler();
                timeout = false;
            }
        }
        close(listenfd);
        close(pipefd[1]);
        close(pipefd[0]);
        delete[] users;
        return 0;
    }

} // namespace ns_time3

namespace ns_timer_wheel
{

#define BUFFER_ SIZE 64
    class tw_timer;
    /*�� �� socket �� ��ʱ��*/
    struct client_data
    {
        sockaddr_in address;
        int sockfd;
        char buf[BUFFER_SIZE];
        tw_timer *timer;
    };
    /*��ʱ�� ��*/
    class tw_timer
    {
    public:
        tw_timer(int rot, int ts) : next(NULL), prev(NULL), rotation(rot), time_slot(ts) {}

    public:
        int rotation;                   /* ��¼ ��ʱ�� ��ʱ �� ��ת ���� Ȧ �� ��Ч*/
        int time_slot;                  /* ��¼ ��ʱ�� ���� ʱ�� ���� �ĸ� �ۣ� ��Ӧ �� �� �� ��ͬ��*/
        void (*cb_func)(client_data *); /* ��ʱ�� �� �� ����*/
        client_data *user_data;         /* �ͻ� ����*/
        tw_timer *next;                 /* ָ�� ��һ�� ��ʱ��*/
        tw_timer *prev;                 /* ָ�� ǰ һ�� ��ʱ��*/
    };
    class time_wheel
    {
    private:                     /*ʱ�� ���� �� �� ��Ŀ*/
        static const int N = 60; /*ÿ 1 s ʱ�� ��ת �� һ�Σ� �� �� ��� Ϊ 1 s*/
        static const int SI = 1; /*ʱ�� �� �� �ۣ� ���� ÿ�� Ԫ�� ָ�� һ�� ��ʱ�� �� �� �� �� ����*/
        tw_timer *slots[N];
        int cur_slot; /* ʱ�� �� �� ��ǰ ��*/
    public:
        time_wheel() : cur_slot(0)
        {
            for (int i = 0; i < N; ++i)
            {
                slots[i] = NULL; /* ��ʼ�� ÿ�� �� �� ͷ ���*/
            }
        }
        ~time_wheel()
        { /*���� ÿ�� �ۣ� �� ���� ���� �� ��ʱ��*/
            for (int i = 0; i < N; ++i)
            {
                tw_timer *tmp = slots[i];
                while (tmp)
                {
                    slots[i] = tmp->next;
                    delete tmp;
                    tmp = slots[i];
                }
            }
        }
        /*���� �� ʱֵ timeout ���� һ�� ��ʱ���� ���� �� ���� ���� �� ����*/
        tw_timer *add_timer(int timeout)
        {
            if (timeout < 0)
            {
                return NULL;
            }
            int ticks = 0;
            /*
            ������ݴ����붨ʱ�� �� ��ʱ ֵ ���� ���� ��ʱ �� ��ת �� ���� �� �δ� �� ���������� �� �δ� �� �洢 �� ���� ticks �С� 
            ��� �� ���� ��ʱ�� �� ��ʱ ֵ С�� ʱ�� �� �� �� ��� SI�� �� �� ticks ���� �ۺ� Ϊ 1�� ���� �� �� ticks ���� �ۺ� Ϊ timeout/ SI
            */
            if (timeout < SI)
            {
                ticks = 1;
            }
            else
            {
                ticks = timeout / SI;
            }                                             /*���� �� ���� �� ��ʱ�� ��ʱ �� ��ת �� ���� Ȧ �� ����*/
            int rotation = ticks / N;                     /*���� �� ���� �� ��ʱ�� Ӧ�� �� ���� �ĸ� ����*/
            int ts = (cur_slot + (ticks % N)) % N;        /*���� �µ� ��ʱ���� ���� ʱ�� ��ת �� rotation Ȧ֮ �� ������ �� λ�� �� ts �� �� ��*/
            tw_timer *timer = new tw_timer(rotation, ts); /*��� �� ts �� ���� ���� �κ� ��ʱ���� �� �� �½� �� ��ʱ�� ���� ���У� ���� �� ��ʱ�� ���� Ϊ �� �� �� ͷ ���*/
            if (!slots[ts])
            {
                printf(" add timer, rotation is% d, ts is% d, cur_slot is% d\ n", rotation, ts, cur_slot);
                slots[ts] = timer;
            }
            /*���� �� ��ʱ�� ���� �� ts �� ����*/
            else
            {
                timer->next = slots[ts];
                slots[ts]->prev = timer;
                slots[ts] = timer;
            }
            return timer;
        }
        /*ɾ�� Ŀ�� ��ʱ�� timer*/
        void del_timer(tw_timer *timer)
        {
            if (!timer)
            {
                return;
            }
            int ts = timer->time_slot;
            /*slots[ ts] �� Ŀ�� ��ʱ�� ���� �� �� ͷ ��㡣 ��� Ŀ�� ��ʱ�� ���� �� ͷ ��㣬 ���� Ҫ ���� �� ts �� �� �� ͷ ���*/
            if (timer == slots[ts])
            {
                slots[ts] = slots[ts]->next;
                if (slots[ts])
                {
                    slots[ts]->prev = NULL;
                }
                delete timer;
            }
            else
            {
                timer->prev->next = timer->next;
                if (timer->next)
                {
                    timer->next->prev = timer->prev;
                }
                delete timer;
            }
        } /*SI ʱ�� �� �� ���� �� ������ ʱ�� �� ��ǰ ���� һ�� �� �� ���*/
        void tick()
        {
            tw_timer *tmp = slots[cur_slot]; /* ȡ�� ʱ�� ���� ��ǰ �� �� ͷ ���*/
            printf(" current slot is% d\ n", cur_slot);
            while (tmp)
            {
                printf(" tick the timer once\ n"); /*��� ��ʱ�� �� rotation ֵ ���� 0�� �� ���� ��һ �� ��������*/
                if (tmp->rotation > 0)
                {
                    tmp->rotation--;
                    tmp = tmp->next;
                }
                /*���� ˵�� ��ʱ�� �Ѿ� ���ڣ� ���� ִ�� ��ʱ ���� Ȼ�� ɾ�� �� ��ʱ��*/
                else
                {
                    tmp->cb_func(tmp->user_data);
                    if (tmp == slots[cur_slot])
                    {
                        printf(" delete header in cur_slot\ n");
                        slots[cur_slot] = tmp->next;
                        delete tmp;
                        if (slots[cur_slot])
                        {
                            slots[cur_slot]->prev = NULL;
                        }
                        tmp = slots[cur_slot];
                    }
                    else
                    {
                        tmp->prev->next = tmp->next;
                        if (tmp->next)
                        {
                            tmp->next->prev = tmp->prev;
                        }
                        tw_timer *tmp2 = tmp->next;
                        delete tmp;
                        tmp = tmp2;
                    }
                }
            }
            cur_slot = ++cur_slot % N; /* ���� ʱ�� �� �� ��ǰ �ۣ� �� ��ӳ ʱ�� �� �� ת��*/
        }
    };

} // namespace ns_timer_wheel

int main()
{
    ns_time1::main();
    std::find_if();
    return 0;
}