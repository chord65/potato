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

#define LISTENQ 1024

//绑定和监听端口
int ptt_sock_bind_listen(int port);
//设置套接字非阻塞
int ptt_set sock_nonblock(int sock_fd);
//接收连接
int ptt_accept_connection(int listen_fd, int epoll_fd, char *path);

#endif //SOCKET_H
