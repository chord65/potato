//
// Created by chord on 18-5-3.
//

#ifndef POTATO_HTTP_PARSE_H
#define POTATO_HTTP_PARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "http_request.h"


#define CR '\r'
#define LF '\n'

//向请求头链表添加新结点
void http_header_list_add(http_request_header_t **head, http_request_header_t *p);

//解析请求行
int http_parse_request_line(http_request_t *request);

//解析请求头部
int http_parse_request_header(http_request_t *request);

#endif //POTATO_HTTP_PARSE_H
