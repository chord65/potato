//
// Created by chord on 18-5-7.
//

#include "epoll.h"

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
int tk_epoll_wait(int epoll_fd, struct epoll_event *events, int maxevents, int timeout)
{
    return epoll_wait(epoll_fd, &events, MAXEVENTS, timeout);
}


