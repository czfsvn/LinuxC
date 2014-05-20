//from http://www.embedhq.org/html/jsbw/2013/1014/3559.html

#include<stdio.h>
#include<pthread.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
void* ticketport1(void*);
void* ticketport2(void*);
int tickets=100;
pthread_mutex_t mutex;
pthread_cond_t  qready=PTHREAD_COND_INITIALIZER;      //��̬��ʼ����������;

int main()
{
    int ret;
    pthread_t id1,id2;
    pthread_mutex_init(&mutex,NULL);                  //��ʼ��������
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
}

void* ticketport1(void* arg)
{
    pthread_mutex_lock(&mutex);                //������������
    while(tickets > 0)
    {
        if(tickets%2 == 1)
        {
            usleep(1000);
            printf("thread1 sell ticket: %d\n",tickets--);
            pthread_cond_signal(&qready); //�����ı䣬�����źţ�֪ͨticketport2
        }
        else
        {
            pthread_cond_wait(&qready,&mutex);    //�⿪mutex,���ȴ�qready�ı�
        }
        pthread_mutex_unlock(&mutex);            //������������
    }
    return (void*)0;
}

void* ticketport2(void* arg)
{
    pthread_mutex_lock(&mutex);                //������������
    while(tickets > 0)
    {
        if(tickets%2==0)
        {
            usleep(1000);
            printf("thread2 sell ticket: %d\n",tickets--);
            pthread_cond_signal(&qready); //�����ı䣬�����źţ�֪ͨticketport1
        }
        else
        {
            pthread_cond_wait(&qready,&mutex);   //�⿪mutex,���ȴ�qready�ı�
        }
        pthread_mutex_unlock(&mutex);            //������������
    }
    return (void*)0;
}
