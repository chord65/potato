//
// Created by chord on 18-5-11.
//

#ifndef SOCKET_H
#define SOCKET_H

#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <fcntl.h>
#include "socket.h"
#include "http_request.h"
#include "epoll.h"
#include "timer.h"
#include "dbg.h"

#define LISTENQ 1024

//绑定和监听端口
int ptt_sock_bind_listen(int port);
//设置套接字非阻塞
int ptt_set_sock_nonblock(int sock_fd);
//接收连接
int ptt_accept_connection(int listen_fd, int epoll_fd, char *path);
//关闭连接，释放请求结构
int ptt_close_conn(ptt_http_request_t *request);
//

#endif //SOCKET_H
