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
        assert(sockfd >= 0); /*ͨ��ѡ�� SO_RCVTIMEO �� SO_SNDTIMEO �����õĳ�ʱʱ��������� timeval�� ��� select ϵͳ���õĳ�ʱ����������ͬ*/
        struct timeval timeout;
        timeout.tv_sec  = time;
        timeout.tv_usec = 0;
        socklen_t len   = sizeof(timeout);
        ret             = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, len);
        assert(ret != -1);
        ret = connect(sockfd, ( struct sockaddr* )&address, sizeof(address));
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
    class util_timer; /* ǰ������*/
    /*
    �û����� �ṹ��
    �ͻ� �� socket ��ַ�� socket �ļ� ���� ���� �� ���� �� ��ʱ��
    */
    struct client_data
    {
        sockaddr_in address;
        int         sockfd;
        char        buf[BUFFER_SIZE];
        util_timer* timer;
    }; /*��ʱ�� ��*/
    class util_timer
    {
    public:
        util_timer() : prev(NULL), next(NULL) {}

    public:
        time_t expire;                                      /* ����ĳ�ʱʱ�䣬 ����ʹ�þ���ʱ��*/
        void (*cb_func)(client_data*); /* ���� �� �� ����*/ /*�� �� ���� ���� �� �ͻ� ���ݣ� �� ��ʱ�� �� ִ���� �� �ݸ� �� �� ����*/
        client_data* user_data;
        util_timer*  prev; /* ָ�� ǰ һ�� ��ʱ��*/
        util_timer*  next; /* ָ�� ��һ�� ��ʱ��*/
    };
    /*��ʱ������ ����һ������ ˫������ �Ҵ���ͷ����β�ڵ�*/
    class sort_timer_lst
    {
    private:
        util_timer* head;
        util_timer* tail;

    public:
        sort_timer_lst() : head(NULL), tail(NULL) {} /*��������ʱ��ɾ���������еĶ�ʱ��*/
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
        /*��Ŀ�궨ʱ�� timer ��ӵ�������*/
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
            /*���Ŀ�궨ʱ���ĳ�ʱʱ��С�ڵ�ǰ���������ж�ʱ���ĳ�ʱʱ�䣬 ��Ѹö�ʱ����������ͷ���� ��Ϊ�����µ�ͷ�ڵ㡣 �������Ҫ�������غ���
                add_ timer(util_timer* timer, util_timer* lst_head)�� �������������к��ʵ�λ�ã� �Ա�֤�������������*/
            if (timer->expire < head->expire)
            {
                timer->next = head;
                head->prev  = timer;
                head        = timer;
                return;
            }
            add_timer(timer, head);
        }
        /*��ĳ����ʱ�������仯ʱ�� ������Ӧ�Ķ�ʱ���������е�λ�á� �������ֻ���Ǳ������Ķ�ʱ���ĳ�ʱʱ���ӳ�������� ���ö�ʱ����Ҫ�������β���ƶ�*/
        void adjust_timer(util_timer* timer)
        {
            if (!timer)
            {
                return;
            }
            util_timer* tmp = timer->next;
            /*�����������Ŀ�궨ʱ����������β���� ���߸ö�ʱ���µĳ�ʱֵ��ȻС������һ����ʱ���ĳ�ʱֵ�� ���õ���*/
            if (!tmp || (timer->expire < tmp->expire))
            {
                return;
            }
            /*���Ŀ�궨ʱ���������ͷ�ڵ㣬 �򽫸ö�ʱ����������ȡ�������²�������*/
            if (timer == head)
            {
                head        = head->next;
                head->prev  = NULL;
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
        void del_timer(util_timer* timer)
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
                    head       = head->next;
                    head->prev = NULL;
                    delete timer;
                    return;
                }
                /*���������������������ʱ���� ��Ŀ�궨ʱ���������β��㣬 �������β�������Ϊԭβ�ڵ��ǰһ���ڵ㣬 Ȼ��ɾ��Ŀ�궨ʱ��*/
                if (timer == tail)
                {
                    tail       = tail->prev;
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
            time_t      cur = time(NULL); /* ��� ϵͳ ��ǰ �� ʱ��*/
            util_timer* tmp = head;       /*��ͷ ��� ��ʼ ���� ���� ÿ�� ��ʱ���� ֱ�� ���� һ�� ��δ ���� �� ��ʱ���� ��� �� ��ʱ�� �� ���� �߼�*/
            while (tmp)
            { /*��Ϊ ÿ�� ��ʱ�� �� ʹ�� ���� ʱ�� ��Ϊ �� ʱֵ�� ���� ���� ���� �� ��ʱ�� �� �� ʱֵ �� ϵͳ ��ǰ ʱ�䣬 �Ƚ� �� �ж� ��ʱ�� �Ƿ� ����*/
                if (cur < tmp->expire)
                {
                    break;
                } /*���� ��ʱ�� �� �� �� ������ �� ִ�� ��ʱ ����*/

                delete timer;
                return;
            }
            /*��� �� ���� ���� �� ���� ��ʱ���� �� Ŀ�� ��ʱ�� �� �� �� �� β ��㣬 �� �� �� �� �� β ��� ���� Ϊ ԭ β �ڵ� �� ǰһ �� �ڵ㣬 Ȼ�� ɾ�� Ŀ�� ��ʱ��*/
            if (timer == tail)
            {
                tail       = tail->prev;
                tail->next = NULL;
                delete timer;
                return;
            }
            /*��� Ŀ�� ��ʱ�� λ�� �� �� �� �м䣬 �� �� �� ǰ�� �� ��ʱ�� ���� ������ Ȼ�� ɾ�� Ŀ�� ��ʱ��*/
            timer->prev->next = timer->next;
            timer->next->prev = timer->prev;
            delete timer;
        }
        /*SIGALRM �ź� ÿ�� �� ���� ���� �� �ź� ���� ������ ��� ʹ�� ͳһ �¼� Դ�� ���� �� ������ �� ִ�� һ�� tick ������ �� ���� �� ���� ���� �� ����*/
        void tick()
        {
            if (!head)
            {
                return;
            }
            printf(" timer tick\ n");
            time_t      cur = time(NULL); /* ��� ϵͳ ��ǰ �� ʱ��*/
            util_timer* tmp = head;       /*��ͷ ��� ��ʼ ���� ���� ÿ�� ��ʱ���� ֱ�� ���� һ�� ��δ ���� �� ��ʱ���� ��� �� ��ʱ�� �� ���� �߼�*/
            while (tmp)
            { /*��Ϊ ÿ�� ��ʱ�� �� ʹ�� ���� ʱ�� ��Ϊ �� ʱֵ�� ���� ���� ���� �� ��ʱ�� �� �� ʱֵ �� ϵͳ ��ǰ ʱ�䣬 �Ƚ� �� �ж� ��ʱ�� �Ƿ� ����*/
                if (cur < tmp->expire)
                {
                    break;
                }                              /*���� ��ʱ�� �� �� �� ������ �� ִ�� ��ʱ ����*/
                tmp->cb_func(tmp->user_ data); /*ִ�� �� ��ʱ�� �е� ��ʱ ���� ֮�� �� ���� �� �� ���� ɾ���� �� ���� �� ��ͷ ���*/
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
        /*һ�� ���� �� ���� ������ ���� ���� �� add_ timer ���� �� adjust_ timer ���� ���á� �� ���� ��ʾ �� Ŀ�� ��ʱ�� timer ��� �� �ڵ� lst_ head ֮�� �� ���� �� ����*/
        void add_timer(util_timer* timer, util_timer* lst_head)
        {
            util_timer* prev = lst_head;
            util_timer* tmp  = prev->next; /*���� lst_ head �ڵ� ֮�� �� ���� �� �� ֱ�� �ҵ� һ�� ��ʱ ʱ�� ���� Ŀ�� ��ʱ�� �� ��ʱ ʱ�� �� �ڵ㣬 ���� Ŀ�� ��ʱ�� ���� �� �ڵ� ֮ǰ*/
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
            } /*��� ���� �� lst_ head �ڵ� ֮�� �� ���� �� �� ��δ �ҵ� ��ʱ ʱ�� ���� Ŀ�� ��ʱ�� �� ��ʱ ʱ�� �� �ڵ㣬 �� �� Ŀ�� ��ʱ�� ���� �� ��β ���� ���� �� ���� Ϊ �� �� �µ� β
                 �ڵ�*/
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