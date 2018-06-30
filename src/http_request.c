//
// Created by chord on 18-5-3.
//

#include "http_request.h"

#define  PTT_HANDLE_IN_ARRAY_LENGTH 2

ptt_http_header_handle_t ptt_http_header_handle_in[] = {
        {"Host", ptt_http_header_handle_ignore},
        {"Connection", ptt_http_header_handle_connection},
        {"If-Modified-Since", ptt_http_header_handle_modified}
};

//初始化请求结构
void ptt_http_request_init(ptt_http_request_t *q, int fd, int epoll_fd, char *path)
{
    q->fd = fd;
    q->epoll_fd = epoll_fd;
    q->check_index = q->read_index = 0;
    q->state = 0;
    q->root = path;

    q->request_start = NULL;
    q->method = 0;
    q->method_end = NULL;
    q->uri = q->uri_end = NULL;
    q->major_digit = q->minor_digit = 0;

    q->header_list = NULL;
    q->cur_key = q->cur_key_end = NULL;
    q->cur_value = q->cur_value_end = NULL;

    q->timer = NULL;
}

//初始化响应结构
void ptt_http_out_init(ptt_http_out_t *out, int fd, time_t tm)
{
    out->fd = fd;
    out->keep_alive = 0;
    out->modified = 1;
    out->mtime = tm;
    //默认为OK
    out->status = PTT_HTTP_OK;
}

//忽略头部字段
int ptt_http_header_handle_ignore(ptt_http_out_t *out, const char *data, int len)
{
    return 0;
}

//处理头部字段Connection
int ptt_http_header_handle_connection(ptt_http_out_t *out, const char *data, int len)
{
    debug("handle header Connection");
    if(strncasecmp("keep-alive", data, len) == 0)
        out->keep_alive = 1;
    return 0;
}

//处理头部字段If-Modified-Since
int ptt_http_header_handle_modified(ptt_http_out_t *out, const char *data, int len)
{
    struct tm mtm;
    if(strptime(data, "%a, %d %b %Y %H:%M:%S GMT", &mtm) == NULL)
        return -1;

    time_t t = mktime(&mtm);
    if(difftime(t, out->mtime) < 1e-6){
        out->modified = 0;
        out->status = PTT_HTTP_NOT_MODIFIED;
    }

    return 0;
}

//为不同头部分发处理函数
void ptt_http_handle_header(ptt_http_request_t *request, ptt_http_out_t *out)
{
    ptt_http_request_header_t *p = NULL, *t = NULL;
    int len = 0;

    p = request->header_list;

    debug("in ptt_http_handle_header");
    if(p == NULL)
        debug("head_list is NULL");

    while(p){
        len = p->key_end - p->key;

        for(int i = 0; i < PTT_HANDLE_IN_ARRAY_LENGTH; i++){
            if(strncmp(p->key, ptt_http_header_handle_in[i].name, len) == 0)
                ptt_http_header_handle_in[i].handler(out, p->value, (int)(p->value_end - p->value));
        }

        t = p;
        p = p->next;
        //释放该节点的内存，减少内存占用
        free(t);
        t = NULL;
    }
    request->header_list = NULL;
}

