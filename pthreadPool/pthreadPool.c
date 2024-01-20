#include "pthreadPool.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MIN_PTHREAD 3

#define MAX_PTHREAD 10

enum STATUS_CODE
{
    ON_SUCESS,
    NULL_PTR,
    MALLOC_ERROR,

};

/*************静态函数前置声明****************/

/* 工作的线程 */
static void *  threadFunc(void * arg);







/* 线程函数的初始化 */
int threadPoolInit(threadpool_t *pool, int minthreadSize, int maxthreadSize)
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

    /* 给id 开辟空间 */

    pool->threradId = (threadpool_t *)malloc(sizeof(threadpool_t) * maxthreadSize);
    if (pool->threradId == NULL)
    {
        return MALLOC_ERROR;
    }

    /* 清除脏数据 */
    memset(pool->threradId, 0, sizeof(threadpool_t) * maxthreadSize);

    /* 创建线程池里面的线程 */
    int ret = pthread_create(&(pool->threradId), NULL, threadFunc, NULL);
    if (ret != 0)
    {
        perror("pthread error ");
    }


    /* 回收线程资源*/
    pthread_join(&pool->threradId , NULL);
}
/* 线程的销毁 */
int theeadPoolDstory(threadpool_t *pool)
{
}