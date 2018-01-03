#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>
#include <semaphore.h>

/*Individual job*/
typedef struct thpool_job_t {
        void   (*function)(void* arg);    //¿¿¿¿
            void                    *arg ;    //¿¿¿¿¿
                struct tpool_job_t     *next ;    //¿¿¿¿¿¿¿
                    struct tpool_job_t     *prev ;    //¿¿¿¿¿¿¿
}thpool_job_t ;

/*job queue as doubly linked list*/
typedef struct thpool_jobqueue {
        thpool_job_t    *head ;           //¿¿¿¿¿¿
            thpool_job_t    *tail;            //¿¿¿¿¿¿
                int             jobsN;           //¿¿¿¿¿¿¿¿
                        sem_t           *queueSem;        //¿¿¿¿¿
}thpool_jobqueue;

/*thread pool*/

typedef struct thpool_t {
        pthread_t          *threads ;        //¿¿¿ID 
            int                 threadsN ;        //¿¿¿¿¿
                thpool_jobqueue    *jobqueue;        //¿¿¿¿¿¿¿
                    

}thpool_t;


/*¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿*/
typedef struct thread_data{
        pthread_mutex_t     *mutex_p ;
            thpool_t            *tp_p ;
}thread_data;



/*
 *  *    ¿¿¿¿¿¿
 *   *    ¿¿¿¿¿ ¿¿¿¿¿ ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
 *    *    @param  :¿¿¿¿¿¿¿ID 
 *     *    @return :¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿null
 *      */

thpool_t   *thpool_init (int threadsN);

/*
 *  * ¿¿¿¿¿¿¿¿¿
 *   * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
 *    *@param: ¿¿¿
 *     *@return:¿¿¿¿¿¿¿
 *      */

void  thpool_thread_do (thpool_t *tp_p);

/* 
 *  *¿¿¿¿¿¿¿¿¿¿¿
 *   *¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
 *    * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
 *     * ATTENTION¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
 *      *
 *       * @param: ¿¿¿¿¿¿¿¿¿¿
 *        * @param: ¿¿¿¿¿¿¿¿¿
 *         * @param:¿¿¿¿¿
 *          * @return : int 
 *           */

int thpool_t_add_work (thpool_t *tp_p ,void* (*function_p) (void *), void* arg_p );


/*
 *  *¿¿¿¿¿
 *   *
 *    *¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
 *     *¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
 *      * @param:¿¿¿¿¿¿¿¿¿¿¿¿
 *       */


void thpool_destory (thpool_t  *tp_p);

/*-----------------------Queue specific---------------------------------*/



/*
 *  * ¿¿¿¿¿
 *   * @param: ¿¿¿¿¿¿¿¿
 *    * @return :¿¿¿¿¿¿¿¿ 0 ¿¿¿¿¿¿¿¿¿¿¿¿¿¿-1
 *     */
int thpool_jobqueue_init (thpool_t *tp_p);


/*
 *  *¿¿¿¿¿¿¿
 *   *¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
 *    *¿¿ thpool_jobqueue_empty ()¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
 *     *
 *      * @param: ¿¿¿¿¿¿¿¿
 *       * @param:¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
 *        * @return   nothing 
 *         */
void thpool_jobqueue_add (thpool_t * tp_p , thpool_job_t *newjob_p);

/*
 *  * ¿¿¿¿¿¿¿¿¿¿¿
 *   *¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
 *    *
 *     *@param :¿¿¿¿¿¿¿¿
 *      *@return : ¿¿¿¿0 ¿¿¿¿¿¿¿¿¿¿¿¿-1 
 *       */
int thpool_jobqueue_removelast (thpool_t *tp_p);


/*
 *  *¿¿¿¿¿¿¿¿¿
 *   *¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
 *    *
 *     *¿¿¿¿¿¿¿¿¿¿¿¿¿¿
 *      *¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
 *       */
thpool_job_t * thpool_jobqueue_peek (thpool_t *tp_p);

/*
 *  *¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
 *   *¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿NULL ¿¿¿¿¿¿¿¿= 0
 *    *
 *     *¿¿¿¿¿¿¿¿¿¿¿¿¿¿
 *      *
 *       */
void thpool_jobqueue_empty (thpool_t *tp_p);

#endif
