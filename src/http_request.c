//
// Created by chord on 18-5-3.
//

#include "http_request.h"

//初始化请求结构
void http_request_init(http_request_t *q, int fd)
{
    q->fd = fd;
    q->check_index = q->read_index = 0;
    q->state = 0;

    q->request_start = NULL;
    q->method = 0;
    q->method_end = NULL;
    q->uri = q->uri_end = NULL;
    q->major_digit = q->minor_digit = 0;

    q->header_list = NULL;
    q->cur_key = q->cur_key_end = NULL;
    q->cur_value = q->cur_value_end = NULL;
}