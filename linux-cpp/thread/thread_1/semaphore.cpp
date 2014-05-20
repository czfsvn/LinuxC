//from http://www.embedhq.org/html/jsbw/2013/1014/3559.html


#include<stdio.h>
#include<pthread.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include <semaphore.h>

void* ticketport1(void*);
void* ticketport2(void*);

int tickets=100;
sem_t mutex,full;              //���������ź���


int main()
{
    int ret;
    pthread_t id1,id2;

    ret=sem_init(&mutex,0,1);                       //��ʼ��mutex�ź���Ϊ1
    ret+=sem_init(&full,0,0);                       //��ʼ��full�ź���Ϊ0
    if(ret!=0)
    {
        perror("sem_init");
    }
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
        sem_wait(&mutex);                  //mutex�ź�������P����
        if(tickets>0)
        {
            usleep(1000);
            printf("thread1 sell ticket: %d\n",tickets--);
            sem_post(&full);                //full�ź�������V����
        }
        else
        {
            sem_post(&full);                //full�ź�������V����
            break;
        }
    }
    return (void*)0;
}

void* ticketport2(void* arg)
{
    while(1)
    {
        sem_wait(&full);               //full�ź�������P����
        if(tickets>0)
        {
            usleep(1000);
            printf("thread2 sell ticket: %d\n",tickets--);
            sem_post(&mutex);         //mutex�ź�������V����
        }
        else
        {
            sem_post(&mutex);         //mutex�ź�������V����
            break;
        }
    }
    return (void*)0;
}
