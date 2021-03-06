#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>
#include <semaphore.h>

/*Individual job*/
typedef struct thpool_job_t {
        void   (*function)(void* arg);    //żżżż
            void                    *arg ;    //żżżżż
                struct tpool_job_t     *next ;    //żżżżżżż
                    struct tpool_job_t     *prev ;    //żżżżżżż
}thpool_job_t ;

/*job queue as doubly linked list*/
typedef struct thpool_jobqueue {
        thpool_job_t    *head ;           //żżżżżż
            thpool_job_t    *tail;            //żżżżżż
                int             jobsN;           //żżżżżżżż
                        sem_t           *queueSem;        //żżżżż
}thpool_jobqueue;

/*thread pool*/

typedef struct thpool_t {
        pthread_t          *threads ;        //żżżID 
            int                 threadsN ;        //żżżżż
                thpool_jobqueue    *jobqueue;        //żżżżżżż
                    

}thpool_t;


/*żżżżżżżżżżżżżżżżżżżżżżżż*/
typedef struct thread_data{
        pthread_mutex_t     *mutex_p ;
            thpool_t            *tp_p ;
}thread_data;



/*
 *  *    żżżżżż
 *   *    żżżżż żżżżż żżżżżżżżżżżżżżżż
 *    *    @param  :żżżżżżżID 
 *     *    @return :żżżżżżżżżżżżżżżżnull
 *      */

thpool_t   *thpool_init (int threadsN);

/*
 *  * żżżżżżżżż
 *   * żżżżżżżżżżżżżżżżżżżżżżżżżżżżżż
 *    *@param: żżż
 *     *@return:żżżżżżż
 *      */

void  thpool_thread_do (thpool_t *tp_p);

/* 
 *  *żżżżżżżżżżż
 *   *żżżżżżżżżżżżżżżżżżżżżżżżżżżż
 *    * żżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżż
 *     * ATTENTIONżżżżżżżżżżżżżżżżżżżżżż
 *      *
 *       * @param: żżżżżżżżżż
 *        * @param: żżżżżżżżż
 *         * @param:żżżżż
 *          * @return : int 
 *           */

int thpool_t_add_work (thpool_t *tp_p ,void* (*function_p) (void *), void* arg_p );


/*
 *  *żżżżż
 *   *
 *    *żżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżż
 *     *żżżżżżżżżżżżżżżżżżżż
 *      * @param:żżżżżżżżżżżż
 *       */


void thpool_destory (thpool_t  *tp_p);

/*-----------------------Queue specific---------------------------------*/



/*
 *  * żżżżż
 *   * @param: żżżżżżżż
 *    * @return :żżżżżżżż 0 żżżżżżżżżżżżżż-1
 *     */
int thpool_jobqueue_init (thpool_t *tp_p);


/*
 *  *żżżżżżż
 *   *żżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżż
 *    *żż thpool_jobqueue_empty ()żżżżżżżżżżżżżżżżż
 *     *
 *      * @param: żżżżżżżż
 *       * @param:żżżżżżżżżżżżżżż
 *        * @return   nothing 
 *         */
void thpool_jobqueue_add (thpool_t * tp_p , thpool_job_t *newjob_p);

/*
 *  * żżżżżżżżżżż
 *   *żżżżżżżżżżżżżżżżżżżżżżż
 *    *
 *     *@param :żżżżżżżż
 *      *@return : żżżż0 żżżżżżżżżżżż-1 
 *       */
int thpool_jobqueue_removelast (thpool_t *tp_p);


/*
 *  *żżżżżżżżż
 *   *żżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżż
 *    *
 *     *żżżżżżżżżżżżżż
 *      *żżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżż
 *       */
thpool_job_t * thpool_jobqueue_peek (thpool_t *tp_p);

/*
 *  *żżżżżżżżżżżżżżż
 *   *żżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżżNULL żżżżżżżż= 0
 *    *
 *     *żżżżżżżżżżżżżż
 *      *
 *       */
void thpool_jobqueue_empty (thpool_t *tp_p);

#endif
