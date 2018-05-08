//
// Created by chord on 18-5-3.
//

#ifndef HTTP_PARSE_H
#define HTTP_PARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "http_request.h"


#define CR '\r'
#define LF '\n'

//向请求头链表添加新结点
void ptt_http_header_list_add(ptt_http_request_header_t **head, ptt_http_request_header_t *p);

//解析请求行
int ptt_http_parse_request_line(ptt_http_request_t *request);

//解析请求头部
int ptt_http_parse_request_header(ptt_http_request_t *request);

#endif //HTTP_PARSE_H
