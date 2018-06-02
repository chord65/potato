//
// Created by chord on 18-6-1.
//

#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <string.h>
#include <stdlib.h>
#include "dbg.h"

#define  PTT_PQ_DEFAULT_CAPACITY 8

typedef struct{
    void **pq;
    size_t capacity;
    size_t size;
    int (*comp)(void *p1, void *p2);
}ptt_pq_t;

extern ptt_pq_t *ptt_timer;

void ptt_pq_init(ptt_pq_t *ptt_pq, size_t capacity, int (*comp)(void *, void *));
int ptt_pq_is_empty(ptt_pq_t *ptt_pq);
int ptt_pq_insert(ptt_pq_t *ptt_pq, void *tmp);
int ptt_pq_deltop(ptt_pq_t *ptt_pq);
void *ptt_pq_top(ptt_pq_t *ptt_pq);

#endif //PRIORITY_QUEUE_H
