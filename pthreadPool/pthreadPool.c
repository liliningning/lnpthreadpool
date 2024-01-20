#include "pthreadPool.h"

#include <stdio.h>


enum STATUS_CODE
{
    ON_SUCESS,
    NULL_PTR,
    MALLOC_ERROR,

};




/* 线程函数的初始化 */
int threadPoolInit (pthread_t * pool, int minthreadSize, int maxthreadSize)
{
    if(pool == NULL)
    {
        return NULL_PTR;
    }

    /* 判断合法性 */
  
}



/* 线程的销毁 */
int theeadPoolDstory(pthread_t * pool)
{

    
}