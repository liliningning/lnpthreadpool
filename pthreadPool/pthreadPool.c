#include "pthreadPool.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <error.h>

#define MIN_PTHREAD 3

#define MAX_PTHREAD 10

#define QUEUE_CAPACITY 100
enum STATUS_CODE
{
    ON_SUCESS,
    NULL_PTR,
    MALLOC_ERROR,

};

/*************静态函数前置声明*******************/

/* 工作的线程 */
static void *threadFunc(void *arg);

/**************静态函数的实现***********************/

/* 消费者函数 */
static void *threadFunc(void *arg)
{
    threadpool_t *pool = (threadpool_t *)arg;
    while (1)
    {
        /* 等一个线程进去就上锁 */
        pthread_mutex_lock(&(pool->mutexpool));
        while (pool->queueSize == 0)
        {
            /* 等待生产者发送的条件变量 */
            pthread_cond_wait(&(pool->notEmpty), &(pool->mutexpool));

            /* 有任务取任务 然后任务数减少  */
            task_t temtask = pool->taskQueue[pool->queuehead];
            pool->queuehead = (pool->queuehead + 1) % pool->queueCapacity;
            (pool->queueSize)--;

            /* 拿到资源后解锁 */
            pthread_mutex_unlock(&(pool->mutexpool));
            /* 给生产者发送信号 */
            pthread_cond_signal(&(pool->notFull));

            /* 给忙碌的线程上锁然后忙碌的线程数++ */
            pthread_mutex_lock(&(pool->busymutex));
            pool->busyThreadNums++;
            pthread_mutex_unlock(&(pool->busymutex));

            /* 执行回调函数后 线程完成任务变成空闲 */
            temtask.worker_hander(temtask.arg);

            pthread_mutex_lock(&(pool->busymutex));
            pool->busyThreadNums--;
            pthread_mutex_unlock(&(pool->busymutex));
        }
    }
    pthread_exit(NULL);
}

/* 线程函数的初始化 */
int threadPoolInit(threadpool_t *pool, int minthreadSize, int maxthreadSize, int queueCapacity)
{
    if (pool == NULL)
    {
        return NULL_PTR;
    }

    /* 判断合法性 */
    if (minthreadSize > maxthreadSize || minthreadSize <= 0 || maxthreadSize <= 0)
    {
        minthreadSize = MIN_PTHREAD;
        maxthreadSize = MAX_PTHREAD;
    }
    /* 合法后将值 给线程里面的值 */
    pool->minthreadSize = minthreadSize;
    pool->maxthreadSize = maxthreadSize;

    do
    {
        /* 任务队列的容量判断 */
        if (queueCapacity <= 0)
        {
            queueCapacity = QUEUE_CAPACITY;
        }
        /* 初始化任务队列的参数 */
        pool->queueCapacity = queueCapacity;
        pool->queueSize = 0;
        pool->queuehead = 0;
        pool->queuetail = 0;

        /* 忙碌的线程数初始化 */
        pool->busyThreadNums = 0;
        /* 存活的线程数初始化 */
        pool->liveThreadNums = minthreadSize;

        /* 初始化锁和条件变量 */
        pthread_mutex_init(&(pool->mutexpool), NULL);
        /* 初始化忙碌的线程的锁 */
        pthread_mutex_init(&(pool->busymutex), NULL);

        if (pthread_cond_init(&(pool->notEmpty), NULL) != 0 || pthread_cond_init(&(pool->notFull), NULL != 0))
        {
            perror("pthread_cond_init error");
            break;
        }

        /*给任务队列开辟空间 */
        pool->taskQueue = (task_t *)malloc(sizeof(task_t) * pool->queueCapacity);
        if (pool->taskQueue == NULL)
        {
            return MALLOC_ERROR;
        }

        memset(pool->taskQueue, 0, sizeof(task_t) * pool->queueCapacity);

        /* 给id 开辟空间 */
        pool->threradId = (threadpool_t *)malloc(sizeof(threadpool_t) * pool->maxthreadSize);
        if (pool->threradId == NULL)
        {
            return MALLOC_ERROR;
        }

        /* 清除脏数据 */
        memset(pool->threradId, 0, sizeof(threadpool_t) * pool->maxthreadSize);

        /* 创建线程池里面的线程 */
        for (int idx = 0; idx < pool->minthreadSize; idx++)
        {
            int ret = pthread_create(&(pool->threradId[idx]), NULL, threadFunc, NULL);
            if (ret != 0)
            {
                perror("pthread create  error ");
                break;
            }
        }

        /* 回收线程资源*/
        for (int idx = 0; idx < pool->minthreadSize; idx++)
        {
            if (pool->threradId[idx] != 0)
            {
                pthread_join(&pool->threradId[idx], NULL);
            }
        }

    } while (0);

    /* 释放锁和条件变量的资源*/
    pthread_mutex_destroy(&(pool->busymutex));
    pthread_mutex_destroy(&(pool->mutexpool));
    pthread_cond_destroy(&(pool->notFull));
    pthread_cond_destroy(&(pool->notEmpty));

    /* 释放创建线程id时开辟的空间 */
    if (pool->threradId != NULL)
    {
        free(pool->threradId);
        pool->threradId = NULL;
    }

    /* 回收任务队列开辟的空间 */
    if (pool->taskQueue != NULL)
    {
        free(pool->taskQueue);
        pool->taskQueue = NULL;
    }
}

/* 添加任务 */
int threadAdd(threadpool_t *pool, void *(worker_hander)(void *arg), void *arg)
{
    if (pool == NULL)
    {
        return NULL_PTR;
    }

    pthread_mutex_lock(&(pool->mutexpool));

    /* 当任务数 = 容量 就证明任务队列满了*/
    while (pool->queueSize == pool->queueCapacity)
    {
        pthread_cond_wait(&(pool->notFull), &(pool->mutexpool));
    }
    /* 添加任务   任务队列定义的结构体的的参数 */
    pool->taskQueue[pool->queuetail].worker_hander = worker_hander;
    pool->taskQueue[pool->queuetail].arg = arg;

    pool->queuetail = (pool->queuetail + 1) % pool->queueCapacity;
    pool->queueSize++;

    pthread_mutex_unlock(&(pool->mutexpool));
    pthread_cond_signal(&(pool->notEmpty));

    return ON_SUCESS;
}

/* 线程的销毁 */
int theeadPoolDstory(threadpool_t *pool)
{
}