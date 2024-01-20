#include <stdio.h>
#include "pthreadPool.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void *taskFunc(void *arg)
{
  int num = *(int *)arg;
  printf("threadid %ld , worknums: %d\n", pthread_self(), num);
}

int main()
{
  int min = 6;
  int max = 10;
  int queueCapacity = 30;
  threadpool_t pool;
  int num = 1;

  /* 创建一个线程 */
  threadPoolInit(&pool, min, max, queueCapacity);
  for (int idx = 0; idx < queueCapacity; idx++)
  {
    threadPoolAddTask(&pool, taskFunc, &num);
    sleep(1);
    num++;
    printf("live %d\n",pool.liveThreadNums);
    printf("buzy %d\n",pool.busyThreadNums);
  }  

}