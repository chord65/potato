//
// Created by chord on 18-5-3.
//

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

//任务队列结构
typedef struct ptt_task_s{
    void (*func)(void *); //回调函数，任务执行时被调用
    void *args;           //函数参数
    struct ptt_task_s *next;
} ptt_task_t;

//线程池结构
typedef struct{
    pthread_mutex_t lock; //互斥量
    pthread_cond_t cond; //条件变量
    ptt_task_t *head; //任务队列头结点
    ptt_task_t *tail; //任务队列尾结点
    pthread_t *threads; //线程数组
    int thread_count; //线程总数
    int queue_size; //任务队列长度
    int shutdown; //关机模式
    int started; //正在运行的线程数
}ptt_threadpool_t;



//初始化线程池
ptt_threadpool_t *ptt_threadpool_init(int thread_num);
//添加任务
int ptt_threadpool_add(ptt_threadpool_t *pool, void (*func)(void *), void *args);
//释放线程
int ptt_threadpool_destroy(ptt_threadpool_t *pool);
//工作线程
void *ptt_threadpool_worker(void *args);
//释放线程池资源
int ptt_threadpool_free(ptt_threadpool_t *pool);

#endif //THREADPOOL_H
