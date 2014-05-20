//from : http://www.embedhq.org/html/jsbw/2013/1014/3559.html
//


#include<stdio.h>
#include<pthread.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
void* ticketport1(void*);//�̺߳�������
void* ticketport2(void*);//�̺߳�������
int tickets= 100;                           //��Ʊ����ʼֵ

int main()
{
    pthread_t id1,id2;
    int ret;
    ret=pthread_create(&id1,NULL,ticketport1,NULL);       //�����߳�1
    if(ret<0)
    {
        perror("creat thread1:");
        exit(-1);
    }
    ret=pthread_create(&id2,NULL,ticketport2,NULL);       //�����߳�2
    if(ret<0)
    {
        perror("creat thread2:");
        exit(-1);
    }
    pthread_join(id1,NULL);                    //�ȴ��߳�1����
    pthread_join(id2,NULL);                    //�ȴ��߳�2����
    return  0;
}

void* ticketport1(void* arg)
{
    while(1)
    {
        if(tickets>0)
        {
            //usleep(1000);
            //��Ʊ��1ÿ��һ��Ʊ���Լ�һ
            printf("ticketport1 sells ticket: %d\n",tickets--);
        }
        else
        {
            break;
        }
    }
    return (void*)0;
}

void* ticketport2(void* arg)
{
    while(1)
    {
        if(tickets>0)
        {
            //usleep(1000);
            //��Ʊ��2ÿ��һ��Ʊ���Լ�һ
            printf("ticketport2 sells ticket: %d\n",tickets--);
        }
        else
        {
            break;
        }
    }
    return (void*)0;
}
