#ifndef __PTHREAD_POOL_H_
#define __PTHREAD_POOL_H_

#include <pthread.h>




typedef struct threadpool_t
{
    /* 线程池里面有多少的线程 */
    pthread_t * threradId;

    /* 最小线程数 */
    int minthreadSize;

    /* 最大线程数 */
    int maxthreadSize;


} threadpool_t;






/* 线程函数的初始化 */
int threadPoolInit (threadpool_t * pool, int minthreadSize, int maxthreadSize);

/* 线程的销毁 */
int theeadPoolDstory(threadpool_t * pool);

#endif