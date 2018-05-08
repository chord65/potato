//
// Created by chord on 18-5-7.
//

#ifndef RIO_H
#define RIO_H

#include <unistd.h>
#include <errno.h>
#include <string.h>

#define PTT_RIO_BUFSIZE 8192

typedef struct {
    int rio_fd;                     //文件描述符
    int rio_cnt;                    //缓冲区中未读取的字节数
    char *rio_bufptr;               //指向缓冲区中下一个要读取的字节
    char rio_buf[PTT_RIO_BUFSIZE];  //缓冲区
} rio_t;

//无缓冲的RIO函数
ssize_t rio_readn(int fd, void *usrbuf, size_t n);
ssize_t rio_writen(int fd, void *usrbuff, size_t n);
                        //返回值：成功，返回传送的字节数；EOF返回0（仅针对rio_readn);出错，返回-1

void rio_readinitb(rio_t *rp, int fd);
//带缓冲的RIO函数
ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);
ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n);


#endif //RIO_H
