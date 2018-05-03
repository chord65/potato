//
// Created by chord on 18-5-3.
//

#ifndef POTATO_HTTP_REQUEST_H
#define POTATO_HTTP_REQUEST_H

#include <stdlib.h>

#define BUFFER_SIZE 4096

#define HTTP_INVALID_METHOD  1
#define HTTP_INVALID_REQUEST 2
#define HTTP_INVALID_HEAD    3

#define HTTP_UNKNOWN 1
#define HTTP_GET     2
#define HTTP_HEAD    3
#define HTTP_POST    4

//请求头结构
typedef struct http_request_header_s{
    void *key;
    void *key_end;
    void *value;
    void *value_end;
    struct http_request_header_s *next;
} http_request_header_t;

//请求结构
typedef struct {
    int fd;
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
    http_request_header_t *header_list; //头部字段链表
    void *cur_key;
    void *cur_key_end;
    void *cur_value;
    void *cur_value_end;
} http_request_t;

//初始化请求结构
void http_request_init(http_request_t *q, int fd);

#endif //POTATO_HTTP_REQUEST_H
