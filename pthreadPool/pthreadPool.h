#ifndef __PTHREAD_POOL_H_
#define __PTHREAD_POOL_H_

#include <pthread.h>


typedef struct task_t
{
    void * (* worker_hander)(void * arg);
    void * arg;

} task_t;


typedef struct threadpool_t
{
    /* 线程池里面有多少的线程 */
    pthread_t * threradId;

    /* 线程中的管理者 */
    pthread_t  mangerthreadId;

    /* 最小线程数 */
    int minthreadSize;
    /* 最大线程数 */
    int maxthreadSize;

    /* 忙碌的线程数 */
    int busyThreadNums;

    /* 存活的线程数 */
    int liveThreadNums;

    /* 离开的线程数 */
    int exitThreadNums;


    /* 创建任务队列 */

    /* 任务队列 */
    task_t * taskQueue;
    /* 任务队列的容量*/
    int queueCapacity;
    /* 任务里面的任务数*/
    int queueSize;
    /* 队头 -> 放入数据 */
    int queuehead;
    /* 队尾 -> 取出数据 */
    int queuetail;

    /* 需要销毁的线程的标志位 要销毁为1 不销毁为0  */
    int destorynum ;

    


    /* 条件变量 */

    /* 锁的条件变量 */
    pthread_mutex_t mutexpool;

    /* 忙碌的线程的锁 */
    pthread_mutex_t busymutex;

    /* 任务队列有东西可拿 */
    pthread_cond_t notEmpty;

    /* 任务队列不满了需要填充东西进任务队列 */
    pthread_cond_t notFull;




} threadpool_t;






/* 线程函数的初始化 */
int threadPoolInit (threadpool_t * pool, int minthreadSize, int maxthreadSize, int queueCapacity);


/* 添加任务 */
int threadAdd(threadpool_t * pool, void *(worker_hander)(void * arg), void *arg);

/* 线程的销毁 */
int theeadPoolDstory(threadpool_t * pool);

#endif