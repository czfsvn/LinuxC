#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>


#include "thread_pool.h"


static int thpool_keepalive = 1 ;   //�̳߳ر��ִ��
pthread_mutex_t  mutex  = PTHREAD_MUTEX_INITIALIZER ;  //��̬��ֵ����ʼ��������


thpool_t * thpool_init (int threadsN){
    thpool_t  *tp_p ; 

    if (!threadsN || threadsN < 1){
        threadsN = 1 ;
        
    }

    tp_p =  (thpool_t *)malloc (sizeof (thpool_t)) ;
    if (tp_p == NULL){
            fprintf (stderr ,"thpool_init (): could not allocate memory for thread pool\n");
        return NULL ;
    }
    tp_p->threads = (pthread_t *)malloc (threadsN * sizeof (pthread_t));
    if (tp_p->threads == NULL){
        fprintf( stderr , "could not allocation memory for thread id\n");
        return NULL;
    }
    tp_p->threadsN = threadsN ;
    

    if (thpool_jobqueue_init (tp_p) == -1){
        fprintf (stderr ,"could not allocate memory for job queue\n");
        return NULL;
        }

    /*��ʼ���ź�*/
    tp_p->jobqueue->queueSem = (sem_t *)malloc (sizeof (sem_t));

    /*��λһ�������ź������ڶ���������1��ʾ������ź������ڽ����ڵ��߳��ǹ���ģ��������������ź����ĳ�ʼֵ*/
    sem_init (tp_p->jobqueue->queueSem, 0 , 0 );
        
    int  t ; 
    
       

    for (t = 0 ; t < threadsN ; t++){
        printf ("Create thread %d in pool\n", t);

        //���ĸ������Ǵ��ݸ�����ָ���һ���������������ָ�����������˵���߳�ָ��
        if (pthread_create (&(tp_p->threads[t]) , NULL , (void *) thpool_thread_do , (void *)tp_p)){
            free (tp_p->threads);
            
            free (tp_p->jobqueue->queueSem);
            free (tp_p->jobqueue);
            free (tp_p);
        } 
    }
    return  tp_p ;
}



/*
 * ��ʼ�����߳�Ӧ�ô��������
 * ������������ź�����
 */

void thpool_thread_do (thpool_t *tp_p){
    while (thpool_keepalive)
    {
        if (sem_wait (tp_p->jobqueue->queueSem))  //�������������û�й�������ô���е��̶߳����������������������óɹ���ʱ���ź���-1
        {
            fprintf(stderr , "Waiting for semaphore\n");
            exit (1);
        }

        if (thpool_keepalive)
        {
            void *(*func_buff) (void *arg);
            void *arg_buff;
            thpool_job_t *job_p;

            pthread_mutex_lock (&mutex);
            job_p = thpool_jobqueue_peek (tp_p);
                func_buff = job_p->function ;
            arg_buff= job_p->arg ;
            thpool_jobqueue_removelast (tp_p);
            pthread_mutex_unlock (&mutex);
            
            func_buff (arg_buff);
        
            free (job_p);
        }
        else 
        {
            return ;
        }
    }
    return ;



    
}


int thpool_add_work (thpool_t *tp_p ,void * (*function_p )(void *), void *arg_p){

    thpool_job_t   *newjob ;

    newjob = (thpool_job_t *)malloc (sizeof (thpool_job_t));
    if (newjob == NULL)
    {
        fprintf (stderr,"couldnot allocate memory for new job\n");
        exit (1);
    }
    newjob->function = function_p ;
    newjob->arg = arg_p ;

    pthread_mutex_lock (&mutex);
    thpool_jobqueue_add (tp_p ,newjob);
    pthread_mutex_unlock (&mutex);
    return 0 ;
}


void thpool_destory (thpool_t *tp_p){
    int    t ;

    thpool_keepalive = 0 ;  //�����е��߳����е��̶߳��˳�ѭ��

    for (t = 0 ; t < (tp_p->threadsN) ; t++ ){

        //sem_post ��ʹ������߳����������̣߳���������
        if (sem_post (tp_p->jobqueue->queueSem) ){
            fprintf (stderr,"thpool_destory () : could not bypass sem_wait ()\n");
        }
        
    }
    if (sem_destroy (tp_p->jobqueue->queueSem)!= 0){
        fprintf (stderr, "thpool_destory () : could not destroy semaphore\n");
    }

    for (t = 0 ; t< (tp_p->threadsN) ; t++)
    {
        pthread_join (tp_p->threads[t], NULL);
    }
    thpool_jobqueue_empty (tp_p);
    free (tp_p->threads);
    free (tp_p->jobqueue->queueSem);
    free (tp_p->jobqueue);
    free (tp_p);



}


int thpool_jobqueue_init (thpool_t *tp_p)
{
    tp_p->jobqueue = (thpool_jobqueue *)malloc (sizeof (thpool_jobqueue));
    if (tp_p->jobqueue == NULL)
    {
        fprintf (stderr ,"thpool_jobqueue malloc is error\n");
        return -1 ;
    }
    tp_p->jobqueue->tail = NULL ;
    tp_p->jobqueue->head = NULL ;
    tp_p->jobqueue->jobsN = 0 ;
    return 0 ;

}

void thpool_jobqueue_add (thpool_t *tp_p , thpool_job_t *newjob_p){
    newjob_p->next = NULL ;
    newjob_p->prev = NULL ;

    thpool_job_t   *oldfirstjob ;
    oldfirstjob = tp_p->jobqueue->head;


    switch (tp_p->jobqueue->jobsN)
    {
        case 0 : 
             tp_p->jobqueue->tail = newjob_p;
             tp_p->jobqueue->head = newjob_p;
             break;
        default :
             oldfirstjob->prev= newjob_p ;
             newjob_p->next = oldfirstjob ;
             tp_p->jobqueue->head= newjob_p;
             break;
    }

        (tp_p->jobqueue->jobsN)++ ;
    sem_post (tp_p->jobqueue->queueSem);  //ԭ�Ӳ������ź�������1 ����֤�̰߳�ȫ

    int sval ;
    sem_getvalue (tp_p->jobqueue->queueSem , &sval);   //sval��ʾ��ǰ�����������߳�����

}

int thpool_jobqueue_removelast (thpool_t *tp_p){
    thpool_job_t *oldlastjob  , *tmp;
    oldlastjob = tp_p->jobqueue->tail ;


    switch (tp_p->jobqueue->jobsN)
    {
        case 0 :
            return -1 ;
            break;
        case 1 :
            tp_p->jobqueue->head = NULL ;
            tp_p->jobqueue->tail = NULL ;
            break;
        default :
            tmp = oldlastjob->prev ;
            tmp->next = NULL ;
            tp_p->jobqueue->tail = oldlastjob->prev;

    }
    (tp_p->jobqueue->jobsN) -- ;
    int sval ;
    sem_getvalue (tp_p->jobqueue->queueSem, &sval);
    return 0 ;
}
thpool_job_t * thpool_jobqueue_peek (thpool_t *tp_p){
    return tp_p->jobqueue->tail ;
}


void thpool_jobqueue_empty (thpool_t *tp_p)
{
    thpool_job_t *curjob;
    curjob = tp_p->jobqueue->tail ;
    while (tp_p->jobqueue->jobsN){
        tp_p->jobqueue->tail = curjob->prev ;
        free (curjob);
        curjob = tp_p->jobqueue->tail ;
        tp_p->jobqueue->jobsN -- ;
    }
    tp_p->jobqueue->tail = NULL ;
    tp_p->jobqueue->head = NULL ;
}