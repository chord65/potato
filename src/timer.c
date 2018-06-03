//
// Created by chord on 18-6-2.
//

#include "timer.h"
#include "http_request.h"

//定时器容器
ptt_pq_t *ptt_timer;
//当前时间
time_t ptt_cur_time;

//更新当前时间
static void time_update()
{
    ptt_cur_time = time(NULL);
    check(ptt_cur_time != -1, "update time error");
}

//比较超时值，p1<p2返回1
static int comp(void *p1, void *p2)
{
    ptt_timer_node_t *timer1 = (ptt_timer_node_t*)p1;
    ptt_timer_node_t *timer2 = (ptt_timer_node_t*)p2;
    return (timer1->expire_time < timer2->expire_time) ? 1 : 0;
}

//初始化定时器容器
void ptt_timer_init()
{
    ptt_timer = (ptt_pq_t*)malloc(sizeof(ptt_pq_t));
    //初始化最小堆
    ptt_pq_init(ptt_timer, PTT_PQ_DEFAULT_CAPACITY, comp);
    //当前更新时间
    time_update();
}

//初始化定时器
ptt_timer_node_t *ptt_timer_node_init(ptt_http_request_t *request, timer_handler_pt handler, time_t outtime)
{
    ptt_timer_node_t *timer_node = (ptt_timer_node_t*)malloc(sizeof(ptt_timer_node_t));
    if(timer_node == NULL){
        log_err("malloc error");
        return NULL;
    }
    //更新当前时间
    time_update();
    //将当前时间加上outtime作为expire_time
    timer_node->expire_time = ptt_cur_time + outtime;
    timer_node->delete = 0;
    timer_node->handler = handler;
    timer_node->request = request;
    return timer_node;
}

//添加定时器
int ptt_add_timer(ptt_http_request_t *request, timer_handler_pt handler, time_t outtime)
{
    ptt_timer_node_t *timer_node = ptt_timer_node_init(request, handler, outtime);
    if(timer_node == NULL)
        return -1;
    //request中添加timer
    request->timer = timer_node;
    //向堆中插入定时器
    int rc = ptt_pq_insert(ptt_timer, timer_node);
    debug("add timer : expire_time = %ld", timer_node->expire_time);

    if(rc < 0)
        return -1;
}

//删除定时器
void ptt_del_timer(ptt_http_request_t *request)
{
    ptt_timer_node_t *timer_node = (ptt_timer_node_t*)(request->timer);
    //将delete为位置1，留待处理超时连接时再处理
    timer_node->delete = 1;
    debug("delete timer");
}

//获取堆顶定时器距离超时还有多少时间，将该值作为epoll的超时参数，来达到定时的效果
time_t ptt_get_waiting_time()
{
    ptt_timer_node_t *top_timer;
    time_t waiting_time = 0;
    int rc;

    while(!ptt_pq_is_empty(ptt_timer)){
        top_timer = (ptt_timer_node_t*)ptt_pq_top(ptt_timer);

        if(top_timer->delete == 1){
            rc = ptt_pq_deltop(ptt_timer);
            check(rc == 0, "ptt_pq_deltop error");
            continue;
        }

        time_update();
        waiting_time = top_timer->expire_time - ptt_cur_time;

        return waiting_time > 0 ? waiting_time : 0;
    }
    return 0;
}

//处理超时的定时器
void ptt_handle_expire_timer()
{
    ptt_timer_node_t *top_timer = NULL;
    int rc;

    while(!ptt_pq_is_empty(ptt_timer)){
        top_timer = (ptt_timer_node_t*)ptt_pq_top(ptt_timer);

        if(top_timer->delete == 1){
            rc = ptt_pq_deltop(ptt_timer);
            //ptt_pq_deltop函数并不执行释放内存的操作，
            //所以在这里要记得free掉定时器的内存，防止内存泄露
            free(top_timer);
            top_timer = NULL;
            check(rc == 0, "ptt_pq_deltop error");
            continue;
        }

        //判断是否超时
        time_update();
        if(ptt_cur_time < top_timer->expire_time)
            break;

        //调用超时回调函数
        top_timer->handler(top_timer->request);
        debug("handle expire timer");

        //删除堆顶定时器
        rc = ptt_pq_deltop(ptt_timer);
        free(top_timer);
        top_timer = NULL;

        if(rc < 0)
            log_err("ptt_pq_deltop error");
    }
}


