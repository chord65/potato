//
// Created by chord on 18-6-1.
//
#include "priority_queue.h"

void ptt_pq_init(ptt_pq_t *ptt_pq, size_t capacity, int (*comp)(void *, void *))
{
	ptt_pq->pq = (void **)malloc(sizeof(void *) * capacity);
    if(ptt_pq == NULL)
        log_err("malloc error");
    ptt_pq->capacity = capacity;
    ptt_pq->size = 0;
    ptt_pq->comp = comp;
}

static void exch(ptt_pq_t *ptt_pq, size_t i, size_t j)
{
    void *tmp = ptt_pq->pq[i];
    ptt_pq->pq[i] = ptt_pq->pq[j];
    ptt_pq->pq[j] = tmp;
}

int ptt_pq_is_empty(ptt_pq_t *ptt_pq)
{
    return ptt_pq->size == 0 ? 1 : 0;
}

static int reserve(ptt_pq_t *ptt_pq, size_t new_capacity)
{
    if(new_capacity > ptt_pq->size){
        void **new_pq = (void **)malloc(sizeof(void *) * new_capacity);
        if(new_pq == NULL){
            log_err("priority_queue realloc error");
            return -1;
        }

        memcpy(new_pq, ptt_pq->pq, sizeof(ptt_pq->pq)*ptt_pq->capacity);
        free(ptt_pq->pq);
        ptt_pq->pq = new_pq;
		ptt_pq->capacity = new_capacity;
    }
    return 0;
}

static void down(ptt_pq_t *ptt_pq, size_t i)
{
    size_t parent = i;
    size_t child;

    while((parent*2+1) < ptt_pq->size){
        child = parent*2+1;
        if(child+1 < ptt_pq->size && ptt_pq->comp(ptt_pq->pq[child+1], ptt_pq->pq[child]))
            child = child + 1;
        if(ptt_pq->comp(ptt_pq->pq[child], ptt_pq->pq[parent]))
            exch(ptt_pq, parent, child);
        else
            break;
        parent = child;
    }
}

static void up(ptt_pq_t *ptt_pq, size_t i)
{
	void *tmp = ptt_pq->pq[i];
    while(i > 0){
        if(ptt_pq->comp(tmp, ptt_pq->pq[(i-1)/2]))
            ptt_pq->pq[i] = ptt_pq->pq[(i-1)/2];
        else
            break;
        i = (i-1)/2;
    }
    ptt_pq->pq[i] = tmp;
}

int ptt_pq_insert(ptt_pq_t *ptt_pq, void *tmp)
{
    if((ptt_pq->size + 1) > (ptt_pq->capacity - 1)){
        int rc = reserve(ptt_pq, 2 * ptt_pq->capacity);
        if(rc < 0)
            return -1;
    }
    ptt_pq->size += 1;
    ptt_pq->pq[ptt_pq->size-1] = tmp;

	up(ptt_pq, ptt_pq->size-1);
    return 0;
}

int ptt_pq_deltop(ptt_pq_t *ptt_pq)
{
    if(ptt_pq_is_empty(ptt_pq))
        return 0;

    exch(ptt_pq, 0, ptt_pq->size-1);
    //free(ptt_pq->pq[ptt_pq->size-1]);
    //这里不能草率地调用free，因为我们并不能确定这里要释放的的内存
	//是由malloc在堆中申请的，若这里的内存是栈中的，调用free会导致错误
	//不过如果不能再这里进行内存释放的话，会引入潜在的内存泄漏的风险
	//所以如果使用了动态分配的内存，切记要在调用该函数后自行释放内存
	ptt_pq->size -= 1;
    down(ptt_pq, 0);

    if(ptt_pq->size <= (1/4 * ptt_pq->capacity)){
        if(reserve(ptt_pq, 1/2 * ptt_pq->capacity) < 0)
            return -1;
    }

    return 0;
}

void *ptt_pq_top(ptt_pq_t *ptt_pq)
{
    if(ptt_pq_is_empty(ptt_pq))
        return NULL;
    return ptt_pq->pq[0];
}

