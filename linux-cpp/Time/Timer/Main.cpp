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
        assert(sockfd >= 0); /*通过选项 SO_RCVTIMEO 和 SO_SNDTIMEO 所设置的超时时间的类型是 timeval， 这和 select 系统调用的超时参数类型相同*/
        struct timeval timeout;
        timeout.tv_sec = time;
        timeout.tv_usec = 0;
        socklen_t len = sizeof(timeout);
        ret = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, len);
        assert(ret != -1);
        ret = connect(sockfd, (struct sockaddr *)&address, sizeof(address));
        if (ret == -1)
        { /*超时 对应 的 错误 号 是 EINPROGRESS。 下面 这个 条件 如果 成立， 我们 就可以 处理 定时 任务 了*/
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
    class util_timer; /* 前向声明*/
    /*
    用户数据 结构：
    客户 端 socket 地址、 socket 文件 描述 符、 读 缓存 和 定时器
    */
    struct client_data
    {
        sockaddr_in address;
        int sockfd;
        char buf[BUFFER_SIZE];
        util_timer *timer;
    }; /*定时器 类*/
    class util_timer
    {
    public:
        util_timer() : prev(NULL), next(NULL) {}

    public:
        time_t expire;                                       /* 任务的超时时间， 这里使用绝对时间*/
        void (*cb_func)(client_data *); /* 任务 回 调 函数*/ /*回 调 函数 处理 的 客户 数据， 由 定时器 的 执行者 传 递给 回 调 函数*/
        client_data *user_data;
        util_timer *prev; /* 指向 前 一个 定时器*/
        util_timer *next; /* 指向 下一个 定时器*/
    };
    /*定时器链表。 它是一个升序、 双向链表， 且带有头结点和尾节点*/
    class sort_timer_lst
    {
    private:
        util_timer *head;
        util_timer *tail;

    public:
        sort_timer_lst() : head(NULL), tail(NULL) {} /*链表被销毁时，删除其中所有的定时器*/
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
        /*将目标定时器 timer 添加到链表中*/
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
            /*如果目标定时器的超时时间小于当前链表中所有定时器的超时时间， 则把该定时器插入链表头部， 作为链表新的头节点。 否则就需要调用重载函数
                add_timer(util_timer* timer, util_timer* lst_head)， 把它插入链表中合适的位置， 以保证链表的升序特性*/
            if (timer->expire < head->expire)
            {
                timer->next = head;
                head->prev = timer;
                head = timer;
                return;
            }
            add_timer(timer, head);
        }
        /*当某个定时任务发生变化时， 调整对应的定时器在链表中的位置。 这个函数只考虑被调整的定时器的超时时间延长的情况， 即该定时器需要往链表的尾部移动*/
        void adjust_timer(util_timer *timer)
        {
            if (!timer)
            {
                return;
            }
            util_timer *tmp = timer->next;
            /*如果被调整的目标定时器处在链表尾部， 或者该定时器新的超时值仍然小于其下一个定时器的超时值， 则不用调整*/
            if (!tmp || (timer->expire < tmp->expire))
            {
                return;
            }
            /*如果目标定时器是链表的头节点， 则将该定时器从链表中取出并重新插入链表*/
            if (timer == head)
            {
                head = head->next;
                head->prev = NULL;
                timer->next = NULL;
                add_timer(timer, head);
            }
            /*如果目标定时器不是链表的头节点， 则将该定时器从链表中取出， 然后插入其原来所在位置之后的部分链表中*/
            else
            {
                timer->prev->next = timer->next;
                timer->next->prev = timer->prev;
                add_timer(timer, timer->next);
            }
        }
        /*将目标定时器 timer 从链表中删除*/
        void del_timer(util_timer *timer)
        {
            if (!timer)
            {
                return;
            }
            /*下面这个条件成立表示链表中只有一个定时器， 即目标定时器*/
            if ((timer == head) && (timer == tail))
            {
                delete timer;
                head = NULL;
                tail = NULL;
                return;
            }
            /*如果链表中至少有两个定时器， 且目标定时器是链表的头结点， 则将链表的头结点重置为原头节点的下一个节点， 然后删除目标定时器*/
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
                /*如果链表中至少有两个定时器， 且目标定时器是链表的头结点， 则将链表的头结点重置为原头节点的下一个节点， 然后删除目标定时器*/
                if (timer == head)
                {
                    head = head->next;
                    head->prev = NULL;
                    delete timer;
                    return;
                }
                /*如果链表中至少有两个定时器， 且目标定时器是链表的尾结点， 则将链表的尾结点重置为原尾节点的前一个节点， 然后删除目标定时器*/
                if (timer == tail)
                {
                    tail = tail->prev;
                    tail->next = NULL;
                    delete timer;
                    return;
                }
                /*如果 目标 定时器 位于 链 表 的 中间， 则 把 它 前后 的 定时器 串联 起来， 然后 删除 目标 定时器*/
                timer->prev->next = timer->next;
                timer->next->prev = timer->prev;
                delete timer;
            }
        }

        /*SIGALRM 信号 每次 被 触发 就在 其 信号 处理 函数（ 如果 使用 统一 事件 源， 则是 主 函数） 中 执行 一次 tick 函数， 以 处理 链 表上 到期 的 任务*/
        void tick()
        {
            if (!head)
            {
                return;
            }
            printf(" timer tick\ n");
            time_t cur = time(NULL); /* 获得 系统 当前 的 时间*/
            util_timer *tmp = head;  /*从头 结点 开始 依次 处理 每个 定时器， 直到 遇到 一个 尚未 到期 的 定时器， 这就 是 定时器 的 核心 逻辑*/
            while (tmp)
            { /*因为 每个 定时器 都 使用 绝对 时间 作为 超 时值， 所以 我们 可以 把 定时器 的 超 时值 和 系统 当前 时间， 比较 以 判断 定时器 是否 到期*/
                if (cur < tmp->expire)
                {
                    break;
                }                             /*调用 定时器 的 回 调 函数， 以 执行 定时 任务*/
                tmp->cb_func(tmp->user_data); /*执行 完 定时器 中的 定时 任务 之后， 就 将它 从 链 表中 删除， 并 重置 链 表头 结点*/
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
        /*一个 重载 的 辅助 函数， 它被 公有 的 add_timer 函数 和 adjust_ timer 函数 调用。 该 函数 表示 将 目标 定时器 timer 添加 到 节点 lst_ head 之后 的 部分 链 表中*/
        void add_timer(util_timer *timer, util_timer *lst_head)
        {
            util_timer *prev = lst_head;
            util_timer *tmp = prev->next; /*遍历 lst_ head 节点 之后 的 部分 链 表， 直到 找到 一个 超时 时间 大于 目标 定时器 的 超时 时间 的 节点， 并将 目标 定时器 插入 该 节点 之前*/
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
            } /*如果 遍历 完 lst_ head 节点 之后 的 部分 链 表， 仍未 找到 超时 时间 大于 目标 定时器 的 超时 时间 的 节点， 则 将 目标 定时器 插入 链 表尾 部， 并把 它 设置 为 链 表 新的 尾
                 节点*/
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
    /*利用 代码 清单 11- 2 中的 升序 链 表 来 管理 定时器*/
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
        /*定时处理任务，实际上就是调用 tick 函数*/
        timer_lst.tick();
        /*因为一次alarm 调用只会引起一次SIGALRM信号， 所以我们要重新定时， 以不断触发SIGALRM 信号*/
        alarm(TIMESLOT);
    }
    /*定时器回调函数， 它删除非活动连接 socket 上注册事件， 并关闭之*/
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
        /*设置 信号 处理 函数*/
        addsig(SIGALRM);
        addsig(SIGTERM);

        bool stop_server = false;
        client_data *users = new client_data[FD_LIMIT];
        bool timeout = false;
        alarm(TIMESLOT);
        /* 定时*/
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
                /*处理 新到 的 客户 连接*/
                if (sockfd == listenfd)
                {
                    struct sockaddr_in client_address;
                    socklen_t client_addrlength = sizeof(client_address);
                    int connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_addrlength);
                    addfd(epollfd, connfd);

                    users[connfd].address = client_address;
                    users[connfd].sockfd = connfd;

                    /*创建 定时器， 设置 其 回 调 函数 与 超时 时间， 然后 绑 定 定时器 与 用户 数据， 最后 将 定时器 添 加到 链 表 timer_ lst 中*/
                    util_timer *timer = new util_timer;
                    timer->user_data = &users[connfd];
                    timer->cb_func = cb_func;
                    time_t cur = time(NULL);
                    timer->expire = cur + 3 * TIMESLOT;

                    users[connfd].timer = timer;
                    timer_lst.add_timer(timer);
                }
                /*处理 信号*/
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
                                /*用 timeout 变量 标记 有 定时 任务 需要 处理， 但不 立即 处理 定时 任务。 这是 因为 定时 任务 的 优先级 不是 很高， 我们 优先 处理 其他 更重 要的 任务*/
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
                /*处理 客户 连接 上 接 收到 的 数据*/
                else if (events[i].events & EPOLLIN)
                {
                    memset(users[sockfd].buf, '\ 0', BUFFER_SIZE);
                    ret = recv(sockfd, users[sockfd].buf, BUFFER_SIZE - 1, 0);
                    printf(" get %d bytes of client data %s from %d\ n", ret, users[sockfd].buf, sockfd);
                    util_timer *timer = users[sockfd].timer;
                    if (ret < 0)
                    {
                        /*如果发生读错误， 则关闭连接， 并移除其对应的定时器*/
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
                        /*如果对方已经关闭连接， 则我们也关闭连接， 并移除对应的定时器*/
                        cb_func(&users[sockfd]);
                        if (timer)
                        {
                            timer_lst.del_timer(timer);
                        }
                    }
                    else
                    {
                        /*如果某个客户连接上有数据可读， 则我们要调整该连接对应的定时器， 以延迟该连接被关闭的时间*/
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
            } /*最后处理定时事件， 因为 I/O 事件有更高的优先级。 当然， 这样做将导致定时任务不能精确地按照预期的时间执行*/
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
    /*绑 定 socket 和 定时器*/
    struct client_data
    {
        sockaddr_in address;
        int sockfd;
        char buf[BUFFER_SIZE];
        tw_timer *timer;
    };
    /*定时器 类*/
    class tw_timer
    {
    public:
        tw_timer(int rot, int ts) : next(NULL), prev(NULL), rotation(rot), time_slot(ts) {}

    public:
        int rotation;                   /* 记录 定时器 在时 间 轮转 多少 圈 后 生效*/
        int time_slot;                  /* 记录 定时器 属于 时间 轮上 哪个 槽（ 对应 的 链 表， 下同）*/
        void (*cb_func)(client_data *); /* 定时器 回 调 函数*/
        client_data *user_data;         /* 客户 数据*/
        tw_timer *next;                 /* 指向 下一个 定时器*/
        tw_timer *prev;                 /* 指向 前 一个 定时器*/
    };
    class time_wheel
    {
    private:                     /*时间 轮上 槽 的 数目*/
        static const int N = 60; /*每 1 s 时间 轮转 动 一次， 即 槽 间隔 为 1 s*/
        static const int SI = 1; /*时间 轮 的 槽， 其中 每个 元素 指向 一个 定时器 链 表， 链 表 无序*/
        tw_timer *slots[N];
        int cur_slot; /* 时间 轮 的 当前 槽*/
    public:
        time_wheel() : cur_slot(0)
        {
            for (int i = 0; i < N; ++i)
            {
                slots[i] = NULL; /* 初始化 每个 槽 的 头 结点*/
            }
        }
        ~time_wheel()
        { /*遍历 每个 槽， 并 销毁 其中 的 定时器*/
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
        /*根据 定 时值 timeout 创建 一个 定时器， 并把 它 插入 合适 的 槽中*/
        tw_timer *add_timer(int timeout)
        {
            if (timeout < 0)
            {
                return NULL;
            }
            int ticks = 0;
            /*
            下面根据待插入定时器 的 超时 值 计算 它将 在时 间 轮转 动 多少 个 滴答 后被 触发，并将 该 滴答 数 存储 于 变量 ticks 中。 
            如果 待 插入 定时器 的 超时 值 小于 时间 轮 的 槽 间隔 SI， 则 将 ticks 向上 折合 为 1， 否则 就 将 ticks 向下 折合 为 timeout/ SI
            */
            if (timeout < SI)
            {
                ticks = 1;
            }
            else
            {
                ticks = timeout / SI;
            }                                             /*计算 待 插入 的 定时器 在时 间 轮转 动 多少 圈 后被 触发*/
            int rotation = ticks / N;                     /*计算 待 插入 的 定时器 应该 被 插入 哪个 槽中*/
            int ts = (cur_slot + (ticks % N)) % N;        /*创建 新的 定时器， 它在 时间 轮转 动 rotation 圈之 后被 触发， 且 位于 第 ts 个 槽 上*/
            tw_timer *timer = new tw_timer(rotation, ts); /*如果 第 ts 个 槽中 尚无 任何 定时器， 则 把 新建 的 定时器 插入 其中， 并将 该 定时器 设置 为 该 槽 的 头 结点*/
            if (!slots[ts])
            {
                printf(" add timer, rotation is% d, ts is% d, cur_slot is% d\ n", rotation, ts, cur_slot);
                slots[ts] = timer;
            }
            /*否则， 将 定时器 插入 第 ts 个 槽中*/
            else
            {
                timer->next = slots[ts];
                slots[ts]->prev = timer;
                slots[ts] = timer;
            }
            return timer;
        }
        /*删除 目标 定时器 timer*/
        void del_timer(tw_timer *timer)
        {
            if (!timer)
            {
                return;
            }
            int ts = timer->time_slot;
            /*slots[ ts] 是 目标 定时器 所在 槽 的 头 结点。 如果 目标 定时器 就是 该 头 结点， 则需 要 重置 第 ts 个 槽 的 头 结点*/
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
        } /*SI 时间 到 后， 调用 该 函数， 时间 轮 向前 滚动 一个 槽 的 间隔*/
        void tick()
        {
            tw_timer *tmp = slots[cur_slot]; /* 取得 时间 轮上 当前 槽 的 头 结点*/
            printf(" current slot is% d\ n", cur_slot);
            while (tmp)
            {
                printf(" tick the timer once\ n"); /*如果 定时器 的 rotation 值 大于 0， 则 它在 这一 轮 不起作用*/
                if (tmp->rotation > 0)
                {
                    tmp->rotation--;
                    tmp = tmp->next;
                }
                /*否则， 说明 定时器 已经 到期， 于是 执行 定时 任务， 然后 删除 该 定时器*/
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
            cur_slot = ++cur_slot % N; /* 更新 时间 轮 的 当前 槽， 以 反映 时间 轮 的 转动*/
        }
    };

} // namespace ns_timer_wheel

int main()
{
    ns_time1::main();
    std::find_if();
    return 0;
}