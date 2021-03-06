//
// Created by chord on 18-5-3.
//

#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "dbg.h"

#define BUFFER_SIZE 4096

#define PTT_AGAIN EAGAIN

#define HTTP_INVALID_METHOD  -1
#define HTTP_INVALID_REQUEST -2
#define HTTP_INVALID_HEAD    -3

#define HTTP_UNKNOWN 1
#define HTTP_GET     2
#define HTTP_HEAD    3
#define HTTP_POST    4

#define PTT_HTTP_OK        200
#define PTT_HTTP_NOT_MODIFIED 304
#define PTT_HTTP_FORBIDDEN 403
#define PTT_HTTP_NOT_FOUND 404

//请求头结构
typedef struct ptt_http_request_header_s{
    void *key;
    void *key_end;
    void *value;
    void *value_end;
    struct ptt_http_request_header_s *next;
} ptt_http_request_header_t;

//请求结构
typedef struct {
    int fd;
    char *root;
    int epoll_fd;
    char buffer[BUFFER_SIZE];
    size_t check_index;
    size_t read_index;
    int state;

    //请求行
    void *request_start;
    void *request_end;
    int method;
    void *method_start;
    void *method_end;
    void *uri;
    void *uri_end;
    int major_digit;
    int minor_digit;

    //请求头部
    ptt_http_request_header_t *header_list; //头部字段链表
    void *cur_key;
    void *cur_key_end;
    void *cur_value;
    void *cur_value_end;

    //定时器
    void *timer;
} ptt_http_request_t;

//响应结构
typedef struct {
    int fd;
    int keep_alive; //标志是否长连接
    time_t mtime;   //记录文件修改时间
    int modified;   //标志文件是否被修改
    int status; //状态码
} ptt_http_out_t;

//http头部处理函数指针
typedef int (*ptt_http_header_handler_pt)(ptt_http_out_t *out, const char *data, int len);

//http头部字段与响应处理函数的映射结构
typedef struct{
    char *name;
    ptt_http_header_handler_pt handler;
} ptt_http_header_handle_t;

//映射结构数组
extern ptt_http_header_handle_t ptt_http_header_handle_in[];

//初始化请求结构
void ptt_http_request_init(ptt_http_request_t *q, int fd, int epoll_fd, char *path);
//初始化响应结构
void ptt_http_out_init(ptt_http_out_t *out, int fd, time_t tm);

//头部处理函数-忽略头部
int ptt_http_header_handle_ignore(ptt_http_out_t *out, const char *data, int len);
//处理头部字段Connection
int ptt_http_header_handle_connection(ptt_http_out_t *out, const char *data, int len);
//处理头部字段If-Modified-Since
int ptt_http_header_handle_modified(ptt_http_out_t *out, const char *data, int len);

//为不同头部分发处理函数
void ptt_http_handle_header(ptt_http_request_t *request, ptt_http_out_t *out);

#endif //HTTP_REQUEST_H
