//
// Created by chord on 18-5-7.
//

#include "epoll.h"
#include "http_request.h"
#include "socket.h"

//创建epoll
int ptt_epoll_create(int flags)
{
    int epoll_fd;
    epoll_fd = epoll_create1(flags);
    return epoll_fd;
}

//注册描述符
int ptt_epoll_add(int epoll_fd, int fd, ptt_http_request_t *request, int events)
{
    int ret;
    struct epoll_event event;
    event.data.ptr = (void*)request;
    event.events = events;
    ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
    return ret;
}

//修改描述符状态
int ptt_epoll_mod(int epoll_fd, int fd, ptt_http_request_t *request, int events)
{
    int ret;
    struct epoll_event event;
    event.data.ptr = (void*)request;
    event.events = events;
    ret = epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event);
    return ret;
}

//删除描述符
int ptt_epoll_del(int epoll_fd, int fd, ptt_http_request_t *request, int events)
{
    int ret = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    return ret;
}

//等待事件
int ptt_epoll_wait(int epoll_fd, struct epoll_event *events, int maxevents, int timeout)
{
    return epoll_wait(epoll_fd, events, maxevents, timeout);
}

//处理事件
int ptt_handle_events(int epoll_fd, ptt_threadpool_t *tp, int listen_fd, struct epoll_event *events, int events_num, char *root)
{
    ptt_http_request_t *request = NULL;
    for(int i = 0; i < events_num; i++){
        request = (ptt_http_request_t *)events[i].data.ptr;
        int fd = request->fd;

        //如果是监听描述符
        if(fd == listen_fd){
            if(ptt_accept_connection(listen_fd, epoll_fd, root) < 0)
                close(fd);
        }
        else{
            //若是其他描述符被激活

            //判断错误
            if((events[i].events &  EPOLLERR) || (events[i].events & EPOLLHUP) || !(events[i].events & EPOLLIN)){
                close(fd);
                continue;
            }
            //向线程池添加任务
            if(ptt_threadpool_add(tp, ptt_do_request, (void*)request) < 0)
                close(fd);
        }
    }
}


