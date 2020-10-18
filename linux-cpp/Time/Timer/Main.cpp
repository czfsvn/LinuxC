#include <iostream>

using namespace std;

namespace ns_time_socket
{
    int timeout_connect(const char* ip, int port, int time)
    {
        int                ret = 0;
        struct sockaddr_in address;
        bzero(&address, sizeof(address));
        address.sin_family = AF_INET;
        inet_pton(AF_ INET, ip, &address.sin_addr);
        address.sin_port = htons(port);
        int sockfd       = socket(PF_INET, SOCK_STREAM, 0);
        assert(sockfd >= 0); /*通过选项 SO_RCVTIMEO 和 SO_SNDTIMEO 所设置的超时时间的类型是 timeval， 这和 select 系统调用的超时参数类型相同*/
        struct timeval timeout;
        timeout.tv_sec  = time;
        timeout.tv_usec = 0;
        socklen_t len   = sizeof(timeout);
        ret             = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, len);
        assert(ret != -1);
        ret = connect(sockfd, ( struct sockaddr* )&address, sizeof(address));
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

    void main(int argc, char* argv[])
    {
        if (argc <= 2)
        {
            printf(" usage:% s ip_ address port_ number\ n", basename(argv[0]));
            return 1;
        }
        const char* ip     = argv[1];
        int         port   = atoi(argv[2]);
        int         sockfd = timeout_connect(ip, port, 10);
        if (sockfd < 0)
        {
            return;
        }
    }
}  // namespace ns_time_socket

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
        int         sockfd;
        char        buf[BUFFER_SIZE];
        util_timer* timer;
    }; /*定时器 类*/
    class util_timer
    {
    public:
        util_timer() : prev(NULL), next(NULL) {}

    public:
        time_t expire;                                      /* 任务的超时时间， 这里使用绝对时间*/
        void (*cb_func)(client_data*); /* 任务 回 调 函数*/ /*回 调 函数 处理 的 客户 数据， 由 定时器 的 执行者 传 递给 回 调 函数*/
        client_data* user_data;
        util_timer*  prev; /* 指向 前 一个 定时器*/
        util_timer*  next; /* 指向 下一个 定时器*/
    };
    /*定时器链表。 它是一个升序、 双向链表， 且带有头结点和尾节点*/
    class sort_timer_lst
    {
    private:
        util_timer* head;
        util_timer* tail;

    public:
        sort_timer_lst() : head(NULL), tail(NULL) {} /*链表被销毁时，删除其中所有的定时器*/
        ~sort_timer_lst()
        {
            util_timer* tmp = head;
            while (tmp)
            {
                head = tmp->next;
                delete tmp;
                tmp = head;
            }
        }
        /*将目标定时器 timer 添加到链表中*/
        void add_timer(util_timer* timer)
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
                add_ timer(util_timer* timer, util_timer* lst_head)， 把它插入链表中合适的位置， 以保证链表的升序特性*/
            if (timer->expire < head->expire)
            {
                timer->next = head;
                head->prev  = timer;
                head        = timer;
                return;
            }
            add_timer(timer, head);
        }
        /*当某个定时任务发生变化时， 调整对应的定时器在链表中的位置。 这个函数只考虑被调整的定时器的超时时间延长的情况， 即该定时器需要往链表的尾部移动*/
        void adjust_timer(util_timer* timer)
        {
            if (!timer)
            {
                return;
            }
            util_timer* tmp = timer->next;
            /*如果被调整的目标定时器处在链表尾部， 或者该定时器新的超时值仍然小于其下一个定时器的超时值， 则不用调整*/
            if (!tmp || (timer->expire < tmp->expire))
            {
                return;
            }
            /*如果目标定时器是链表的头节点， 则将该定时器从链表中取出并重新插入链表*/
            if (timer == head)
            {
                head        = head->next;
                head->prev  = NULL;
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
        void del_timer(util_timer* timer)
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
                    head       = head->next;
                    head->prev = NULL;
                    delete timer;
                    return;
                }
                /*如果链表中至少有两个定时器， 且目标定时器是链表的尾结点， 则将链表的尾结点重置为原尾节点的前一个节点， 然后删除目标定时器*/
                if (timer == tail)
                {
                    tail       = tail->prev;
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
            time_t      cur = time(NULL); /* 获得 系统 当前 的 时间*/
            util_timer* tmp = head;       /*从头 结点 开始 依次 处理 每个 定时器， 直到 遇到 一个 尚未 到期 的 定时器， 这就 是 定时器 的 核心 逻辑*/
            while (tmp)
            { /*因为 每个 定时器 都 使用 绝对 时间 作为 超 时值， 所以 我们 可以 把 定时器 的 超 时值 和 系统 当前 时间， 比较 以 判断 定时器 是否 到期*/
                if (cur < tmp->expire)
                {
                    break;
                } /*调用 定时器 的 回 调 函数， 以 执行 定时 任务*/

                delete timer;
                return;
            }
            /*如果 链 表中 至少 有 两个 定时器， 且 目标 定时器 是 链 表 的 尾 结点， 则 将 链 表 的 尾 结点 重置 为 原 尾 节点 的 前一 个 节点， 然后 删除 目标 定时器*/
            if (timer == tail)
            {
                tail       = tail->prev;
                tail->next = NULL;
                delete timer;
                return;
            }
            /*如果 目标 定时器 位于 链 表 的 中间， 则 把 它 前后 的 定时器 串联 起来， 然后 删除 目标 定时器*/
            timer->prev->next = timer->next;
            timer->next->prev = timer->prev;
            delete timer;
        }
        /*SIGALRM 信号 每次 被 触发 就在 其 信号 处理 函数（ 如果 使用 统一 事件 源， 则是 主 函数） 中 执行 一次 tick 函数， 以 处理 链 表上 到期 的 任务*/
        void tick()
        {
            if (!head)
            {
                return;
            }
            printf(" timer tick\ n");
            time_t      cur = time(NULL); /* 获得 系统 当前 的 时间*/
            util_timer* tmp = head;       /*从头 结点 开始 依次 处理 每个 定时器， 直到 遇到 一个 尚未 到期 的 定时器， 这就 是 定时器 的 核心 逻辑*/
            while (tmp)
            { /*因为 每个 定时器 都 使用 绝对 时间 作为 超 时值， 所以 我们 可以 把 定时器 的 超 时值 和 系统 当前 时间， 比较 以 判断 定时器 是否 到期*/
                if (cur < tmp->expire)
                {
                    break;
                }                              /*调用 定时器 的 回 调 函数， 以 执行 定时 任务*/
                tmp->cb_func(tmp->user_ data); /*执行 完 定时器 中的 定时 任务 之后， 就 将它 从 链 表中 删除， 并 重置 链 表头 结点*/
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
        /*一个 重载 的 辅助 函数， 它被 公有 的 add_ timer 函数 和 adjust_ timer 函数 调用。 该 函数 表示 将 目标 定时器 timer 添加 到 节点 lst_ head 之后 的 部分 链 表中*/
        void add_timer(util_timer* timer, util_timer* lst_head)
        {
            util_timer* prev = lst_head;
            util_timer* tmp  = prev->next; /*遍历 lst_ head 节点 之后 的 部分 链 表， 直到 找到 一个 超时 时间 大于 目标 定时器 的 超时 时间 的 节点， 并将 目标 定时器 插入 该 节点 之前*/
            while (tmp)
            {
                if (timer->expire < tmp->expire)
                {
                    prev->next  = timer;
                    timer->next = tmp;
                    tmp->prev   = timer;
                    timer->prev = prev;
                    break;
                }
                prev = tmp;
                tmp  = tmp->next;
            } /*如果 遍历 完 lst_ head 节点 之后 的 部分 链 表， 仍未 找到 超时 时间 大于 目标 定时器 的 超时 时间 的 节点， 则 将 目标 定时器 插入 链 表尾 部， 并把 它 设置 为 链 表 新的 尾
                 节点*/
            if (!tmp)
            {
                prev->next  = timer;
                timer->prev = prev;
                timer->next = NULL;
                tail        = timer;
            }
        }
    };

}  // namespace ns_time2_list

int main()
{
    ns_time1::main();
    return 0;
}