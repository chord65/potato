//
// Created by chord on 18-5-7.
//

#ifndef EPOLL_H
#define EPOLL_H

#include <sys/epoll.h>
#include "http_request.h"
#include "threadpool.h"

#define MAXEVENTS 1000

int ptt_epoll_create(int flags);
int ptt_epoll_add(int epoll_fd, int fd, ptt_http_request_t *request, int events);
int ptt_epoll_mod(int epoll_fd, int fd, ptt_http_request_t *request, int events);
int ptt_epoll_del(int epoll_fd, int fd, ptt_http_request_t *request, int events);
int tk_epoll_wait(int epoll_fd, struct epoll_event *events, int maxevents, int timeout);

#endif //EPOLL_H
