//from: http://www.embedhq.org/html/jsbw/2013/1014/3559.html


#include<stdio.h>
#include<pthread.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
void* ticketport1(void*);
void* ticketport2(void*);
int tickets=100;
pthread_mutex_t mutex;

int main()
{
    int ret;
    pthread_t id1,id2;
    pthread_mutex_init(&mutex,NULL);      //��ʼ��������
    ret=pthread_create(&id1,NULL,ticketport1,NULL);
    if(ret<0)
    {
        perror("creat thread1:");
        exit(-1);
    }
    ret=pthread_create(&id2,NULL,ticketport2,NULL);
    if(ret<0)
    {
        perror("creat thread2:");
        exit(-1);
    }
    pthread_join(id1,NULL);
    pthread_join(id2,NULL);
    return 0;
}

void* ticketport1(void* arg)
{
    while(1)
    {
        pthread_mutex_lock(&mutex);                //������������
        if(tickets>0)
        {
            usleep(1000);
            printf("thread1 sell ticket: %d\n",tickets--);
            pthread_mutex_unlock(&mutex);            //������������
        }
        else
        {
            pthread_mutex_unlock(&mutex);            //������������
            break;
        }
        pthread_yield();      //�̵߳ĵ��Ⱥ�����ʹ�����̶߳���ִ�л���
    }
    return (void*)0;
}

void* ticketport2(void* arg)
{
    while(1)
    {
        pthread_mutex_lock(&mutex);                //������������
        if(tickets>0)
        {
            usleep(1000);
            printf("thread2 sell ticket: %d\n",tickets--);
            pthread_mutex_unlock(&mutex);            //������������
        }
        else
        {
            pthread_mutex_unlock(&mutex);            //������������
            break;
        }
        pthread_yield();      //�̵߳ĵ��Ⱥ�����ʹ�����̶߳���ִ�л���
    }
    return (void*)0;
}
