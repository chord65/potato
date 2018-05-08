//
// Created by chord on 18-5-3.
//

#include "http_request.h"

#define  PTT_HANDLE_IN_ARRAY_LENGTH 1

ptt_http_header_handle_t ptt_http_header_handle_in[] = {
        {"Host", ptt_http_header_handle_ignore}
};

//初始化请求结构
void ptt_http_request_init(ptt_http_request_t *q, int fd)
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

//初始化响应结构
void ptt_http_out_init(ptt_http_out_t *out, int fd)
{
    out->fd = fd;
    //默认为OK
    out->status = PTT_HTTP_OK;
}

//忽略头部字段
int ptt_http_header_handle_ignore(ptt_http_out_t *out, const char *data, int len)
{
    return 0;
}

//为不同头部分发处理函数
void ptt_http_handle_header(ptt_http_request_header_t *head, ptt_http_out_t *out)
{
    ptt_http_request_header_t *p = NULL, *t = NULL;
    int len = 0;

    p = head;

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
}

