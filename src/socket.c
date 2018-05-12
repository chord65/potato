//
// Created by chord on 18-5-11.
//

#include "socket.h"

//绑定和监听端口
int ptt_sock_bind_listen(int port)
{
    int sock_fd;

    //创建socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd < 0)
        return -1;

    //设置socket的SO_REUSEADDR选项
    int optval = 1;
    if(setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (const void*)optval, sizeof(int)) < 0)
        return -1;

    //设置服务器地址
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(struct sockaddr_in));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons((unsigned short)port);

    //绑定并监听
    if(bind(sock_fd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
        return -1;
    if(listen(sock_fd, LISTENQ ) < 0){
        //监听失败则关闭socket
        close(sock_fd);
        return -1;
    }

    //返回套接字
    return sock_fd;
}

//设置套接字非阻塞
int ptt_set sock_nonblock(int sock_fd)
{
    //获取旧的描述符状态
    int flag = fcntl(sock_fd, F_GETFL, 0);
    if(flag < 0)
        return -1;

    //设置NONBLOCK
    flag |= O_NONBLOCK;
    if(fcntl(sock_fd, F_SETFL, flag) < 0)
        return -1;
    return 0;
}


//接收连接
int ptt_accept_connection(int listen_fd, int epoll_fd, char *path)
{
    int cnn_sock;
    struct sockaddr_in cnnaddr;
    socklen_t cnnlen = 0;

    memset(&cnnaddr, 0, sizeof(struct sockaddr_in));

    //接收连接
    if((cnn_sock = accept(listen_fd, (struct sockaddr *)&cnnaddr, &cnnlen)) < 0)
        return -1;

    //创建ptt_http_request_t变量并初始化
    ptt_http_request_t *request = malloc(sizeof(ptt_http_request_t));
    ptt_http_request_init(request, cnn_sock, epoll_fd, path);

    //在epoll注册描述符，设置边缘触发和ONESHOT标志
    if(ptt_epoll_add(epoll_fd, cnn_sock, request, (EPOLLIN | EPOLLET | EPOLLONESHOT)) < 0)
        return -1;

    return 0;
}

