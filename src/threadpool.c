//
// Created by chord on 18-5-3.
//

#include "threadpool.h"

ptt_threadpool_t *ptt_threadpool_init(int thread_num)
{
    ptt_threadpool_t *pool;
    do{
        pool = (ptt_threadpool_t *)malloc(sizeof(ptt_threadpool_t));
        if(pool == NULL)
            break;

        //初始化互斥量
        if(pthread_mutex_init(&pool->lock, NULL) != 0)
            break;
        //初始化条件变量
        if(pthread_cond_init(&pool->cond, NULL) != 0)
            break;
        //初始化任务队列
        pool->head = NULL;
        pool->tail = NULL;
        pool->queue_size = 0;
        //初始化线程池
        pool->threads = (pthread_t *)malloc(thread_num * sizeof(pthread_t));
        for(int i = 0; i < thread_num; i++){
            if(pthread_create(&pool->threads[i], NULL, ptt_threadpool_worker, (void *)pool) != 0){
                //这里调用destroy而不是free是因为线程可能已经被创建并开始执行
                //所以不能使用free强行释放线程资源
                ptt_threadpool_destroy(pool);
                return NULL;
            }
        }
        pool->thread_count = thread_num;
        pool->started = thread_num;

        //设置关机模式
        pool->shutdown = 0;
        return pool;
    }while(0);

    if(pool)
        ptt_threadpool_free(pool);
    return NULL;

}

int ptt_threadpool_free(ptt_threadpool_t *pool)
{
    //如果有线程正在运行，则不能强行释放线程资源
    if(pool == NULL || pool->started > 0)
        return -1;

    //释放线程数组
    if(pool->threads != NULL)
        free(pool->threads);

    //释放任务队列
    ptt_task_t *p = NULL;
    while(pool->head){
        p = pool->head;
        pool->head = pool->head->next;
        free(p);
    }

    return 0;
}

int ptt_threadpool_destroy(ptt_threadpool_t *pool)
{
    //首先获取锁，防止其他线程继续向任务队列中添加或取出任务
    if(pthread_mutex_lock(&pool->lock) != 0)
        return -1;
    //判断线程池是否已被关闭
    if(pool->shutdown != 0)
        return -1;
    //设置关机模式，并通过条件变量向各个线程广播
    pool->shutdown = 1;
    if(pthread_cond_broadcast(&pool->cond) != 0)
        return -1;
    //释放锁
    pthread_mutex_unlock(&pool->lock);
    //等待各线程终止
    for(int i = 0; i < pool->thread_count; i++)
        if(pthread_join(pool->threads[i], NULL) != 0)
            return -1;
    //撤销锁和条件变量
    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->cond);
    //释放线程池资源
    if(ptt_threadpool_free(pool) != 0)
        return -1;
}

int ptt_threadpool_add(ptt_threadpool_t *pool, void (*func)(void *), void *args)
{
    if(pool == NULL || func == NULL)
        return -1;

    //获取锁
    if(pthread_mutex_lock(&(pool->lock)) != 0)
        return -2;

    //创建任务结点
    ptt_task_t *t;
    if((t = (ptt_task_t *)malloc(sizeof(ptt_task_t))) == NULL)
        return -3;
    t->func = func;
    t->args = args;
    t->next = NULL;

    //向队列尾部添加结点
    if(pool->queue_size == 0){
        pool->head = t;
        pool->tail = t;
        pool->queue_size++;
    }
    else if(pool->queue_size > 0){
        pool->tail->next = t;
        pool->tail = t;
        pool->queue_size++;
    }
    else
        return -4;

    //从线程池中唤醒一个工作线程
    pthread_cond_signal(&pool->cond);

    //释放锁
    if(pthread_mutex_unlock(&(pool->lock)) != 0)
        return -5;

    return 0;
}

void *ptt_threadpool_worker(void *args)
{
    ptt_threadpool_t *pool = (ptt_threadpool_t *)args;
    ptt_task_t *task;

    while(1){
        //首先获取锁
        pthread_mutex_lock(&pool->lock);

        //如果任务队列为空且不处于关机模式，等待条件变量
        while(pool->queue_size == 0 && pool->shutdown == 0)
            pthread_cond_wait(&pool->cond, &pool->lock);

        //若处于关机模式，则退出线程
        if(pool->shutdown)
            break;

        //从任务队列头部取出一个结点
        if(pool->head == NULL){
            pthread_mutex_unlock(&pool->lock);
            continue;
        }
        task = pool->head;
        if(pool->head == pool->tail)
            pool->tail = NULL;
        pool->head = pool->head->next;
        pool->queue_size--;

        //释放锁
        pthread_mutex_unlock(&pool->lock);

        //执行任务中的回调函数
        task->func(task->args);

        //释放任务结点
        free(task);
    }

    //将正在运行的线程数减1
    pool->started--;
    //释放锁
    pthread_mutex_unlock(&pool->lock);
    //线程退出
    printf("thread_%lx exited\n", pthread_self());
    pthread_exit(NULL);

    return NULL;
}