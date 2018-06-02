//
// Created by chord on 18-6-2.
//

#ifndef POTATO_TIMER_H
#define POTATO_TIMER_H

#include <time.h>
#include "priority_queue.h"
#include "http_request.h"
#include "dbg.h"

typedef int (*timer_handler_pt)(ptt_http_request_t *request);

//定时器结构
typedef struct {
    time_t expire_time;
    int delete;
    timer_handler_pt handler;
    ptt_http_request_t *request;
}ptt_timer_node_t;

void ptt_timer_init();
ptt_timer_node_t *ptt_timer_node_init(ptt_http_request_t *request, timer_handler_pt handler, time_t outtime);
int ptt_add_timer(ptt_http_request_t *request, timer_handler_pt handler, time_t outtime);
time_t ptt_get_waiting_time();
void ptt_handle_expire_timer();


#endif //POTATO_TIMER_H
