//
// Created by chord on 18-5-7.
//

#ifndef EPOLL_H
#define EPOLL_H

#include <sys/epoll.h>
#include "http_request.h"
#include "threadpool.h"
#include "http.h"

#define MAXEVENTS 1000

int ptt_epoll_create(int flags);
int ptt_epoll_add(int epoll_fd, int fd, ptt_http_request_t *request, int events);
int ptt_epoll_mod(int epoll_fd, int fd, ptt_http_request_t *request, int events);
int ptt_epoll_del(int epoll_fd, int fd, ptt_http_request_t *request, int events);
int ptt_epoll_wait(int epoll_fd, struct epoll_event *events, int maxevents, int timeout);
int ptt_handle_events(int epoll_fd, ptt_threadpool_t *tp, int listen_fd, struct epoll_event *events, int events_num, char *root);

#endif //EPOLL_H
