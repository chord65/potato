//
// Created by chord on 18-5-3.
//

#include "http_parse.h"
#include "http_request.h"

//向请求头链表添加新结点
void ptt_http_header_list_add(ptt_http_request_header_t **head, ptt_http_request_header_t *p)
{
    if(*head == NULL){
        *head = p;
        p->next = NULL;
        return;
    }
    ptt_http_request_header_t *t = (*head)->next;
    (*head)->next = p;
    p->next = t;
}

//解析请求行
int ptt_http_parse_request_line(ptt_http_request_t *request)
{
    enum {
        sw_start = 0,
        sw_method,
        sw_spaces_before_uri,
        sw_after_slash_in_uri,
        sw_http,
        sw_http_H,
        sw_http_HT,
        sw_http_HTT,
        sw_http_HTTP,
        sw_first_major_digit,
        sw_major_digit,
        sw_first_minor_digit,
        sw_minor_digit,
        sw_spaces_after_digit,
        sw_almost_done
    } state;

    state = request->state;


    char ch, *p;
    size_t pi;
    for(pi = request->check_index; pi < request->read_index; pi++){
        p = &request->buffer[pi];
        ch = *p;

        switch(state){
            case sw_start :{
                request->request_start = p;
                if(ch == CR || ch == LF)
                    break;
                if(ch == ' ')
                    break;
                if(ch < 'A' || ch > 'Z')
                    return HTTP_INVALID_REQUEST;

                request->method_start = p;
                state = sw_method;
                break;
            }

            case sw_method :{
                if(ch == ' '){
                    char *m = request->method_start;
                    if(p-m == 3){
                        if(m[0] == 'G' && m[1] == 'E' && m[2] == 'T'){
                            request->method_end = p;
                            request->method = HTTP_GET;
                        }
                    }
                    else if(p-m == 4){
                        if(m[0] == 'H' && m[1] == 'E' && m[2] == 'A' && m[3] == 'D'){
                            request->method_end = p;
                            request->method = HTTP_HEAD;
                        }
                        else if(m[0] == 'P' && m[1] == 'O' && m[2] == 'S' && m[3] == 'T'){
                            request->method_end = p;
                            request->method = HTTP_POST;
                        }
                    }
                    else{
                        request->method_end = p;
                        request->method = HTTP_UNKNOWN;
                    }
                    state = sw_spaces_before_uri;
                    break;
                }
                if(ch < 'A' || ch > 'Z')
                    return HTTP_INVALID_METHOD;
                break;
            }

            case sw_spaces_before_uri :{
                if(ch == '/'){
                    request->uri = p+1;
                    state = sw_after_slash_in_uri;
                    break;
                }

                if(ch == ' ')
                    break;
                else
                    return HTTP_INVALID_REQUEST;
                break;
            }

            case sw_after_slash_in_uri :{
                if(ch == ' '){
                    request->uri_end = p;
                    state = sw_http;
                    break;
                }
                break;
            }

            case sw_http:{
                if(ch == ' ')
                    break;
                if(ch == 'H'){
                    state = sw_http_H;
                    break;
                }
                return HTTP_INVALID_REQUEST;
                break;
            }

            case sw_http_H:{
                if(ch == 'T'){
                    state = sw_http_HT;
                    break;
                }
                return HTTP_INVALID_REQUEST;
                break;
            }

            case sw_http_HT:{
                if(ch == 'T'){
                    state = sw_http_HTT;
                    break;
                }
                return HTTP_INVALID_REQUEST;
                break;
            }

            case sw_http_HTT:{
                if(ch == 'P'){
                    state = sw_http_HTTP;
                    break;
                }
                return HTTP_INVALID_REQUEST;
                break;
            }

            case sw_http_HTTP:{
                if(ch == '/'){
                    state = sw_first_major_digit;
                    break;
                }
                return HTTP_INVALID_REQUEST;
                break;
            }

            case sw_first_major_digit:{
                if(ch >= '0' && ch <= '9'){
                    request->major_digit = ch - '0';
                    state = sw_major_digit;
                    break;
                }
                return HTTP_INVALID_REQUEST;
                break;
            }

            case sw_major_digit:{
                if(ch == '.'){
                    state = sw_first_minor_digit;
                    break;
                }
                if(ch >= '0' && ch <= '9'){
                    request->major_digit = request->major_digit * 10 + ch - '0';
                    break;
                }
                return HTTP_INVALID_REQUEST;
                break;
            }

            case sw_first_minor_digit:{
                if(ch >= '0' && ch <= '9'){
                    request->minor_digit = ch - '0';
                    state = sw_minor_digit;
                    break;
                }
                return HTTP_INVALID_REQUEST;
                break;
            }

            case sw_minor_digit:{
                if(ch == ' '){
                    state = sw_spaces_after_digit;
                    break;
                }
                if(ch == CR){
                    state = sw_almost_done;
                    break;
                }
                if(ch == LF)
                    goto done;
                if(ch >= '0' && ch <= '9'){
                    request->minor_digit = request->minor_digit * 10 + ch - '0';
                    break;
                }
                return HTTP_INVALID_REQUEST;
                break;
            }

            case sw_spaces_after_digit:{
                if(ch == ' ')
                    break;
                if(ch == CR){
                    state = sw_almost_done;
                    break;
                }
                if(ch == LF){
                    goto done;
                    break;
                }
                return HTTP_INVALID_REQUEST;
                break;
            }

            case sw_almost_done:{
                request->request_end = p-1;
                if(ch == LF)
                    goto done;
                return HTTP_INVALID_REQUEST;
                break;
            }
        }
    }

    printf("parse_line state = %d\n", state);

    request->check_index = pi;
    request->state = state;
    return PTT_AGAIN;

    done:
    request->check_index = pi + 1;
    if(request->request_end == NULL)
        request->request_end = p;
    request->state = sw_start;
    return 0;
}

//解析请求头部
int ptt_http_parse_request_header(ptt_http_request_t *request)
{
    enum {
        sw_start = 0,
        sw_key,
        sw_spaces_before_colon,
        sw_spaces_after_colon,
        sw_value,
        sw_cr,
        sw_crlf,
        sw_crlfcr
    } state;

    state = request->state;

    char ch, *p;
    size_t pi;
    ptt_http_request_header_t *hd;

    for(pi = request->check_index; pi < request->read_index; pi++){
        p = &request->buffer[pi];
        ch = *p;

        switch(state){
            case sw_start:{
                if(ch == CR || ch == LF)
                    break;
                request->cur_key = p;
                state = sw_key;
                break;
            }

            case sw_key:{
                if(ch == ' '){
                    request->cur_key_end = p;
                    state = sw_spaces_before_colon;
                    break;
                }
                if(ch == ':'){
                    request->cur_key_end = p;
                    state = sw_spaces_after_colon;
                    break;
                }
                break;
            }

            case sw_spaces_before_colon:{
                if(ch == ' ')
                    break;
                if(ch == ':'){
                    state = sw_spaces_after_colon;
                    break;
                }
                return	HTTP_INVALID_HEAD;
                break;
            }

            case sw_spaces_after_colon:{
                if(ch == ' ')
                    break;
                request->cur_value = p;
                state = sw_value;
                break;
            }

            case sw_value:{
                if(ch == CR){
                    request->cur_value_end = p;
                    state = sw_cr;
                    break;
                }
                if(ch == LF){
                    request->cur_value_end = p;
                    state = sw_crlf;
                    break;
                }
                break;
            }

            case sw_cr:{
                if(ch == LF){
                    state = sw_crlf;
                    hd = (ptt_http_request_header_t *)malloc(sizeof(ptt_http_request_header_t));
                    hd->key = request->cur_key;
                    hd->key_end = request->cur_key_end;
                    hd->value = request->cur_value;
                    hd->value_end = request->cur_value_end;
                    ptt_http_header_list_add(&request->header_list, hd);
                    hd = NULL;
                    break;
                }
                return HTTP_INVALID_HEAD;
                break;
            }

            case sw_crlf:{
                if(ch == CR){
                    state = sw_crlfcr;
                    break;
                }
                else if(ch == LF){
                    hd = (ptt_http_request_header_t *)malloc(sizeof(struct ptt_http_request_header_s));
                    hd->key = request->cur_key;
                    hd->key_end = request->cur_key_end;
                    hd->value = request->cur_value;
                    hd->value_end = request->cur_value_end;
                    ptt_http_header_list_add(&request->header_list, hd);
                    hd = NULL;
                    goto done;
                }
                else{
                    request->cur_key = p;
                    state = sw_key;
                }
                break;
            }

            case sw_crlfcr:{
                if(ch == LF)
                    goto done;
                return HTTP_INVALID_HEAD;
                break;
            }
        }
    }
    request->check_index = pi;
    request->state = state;
    return PTT_AGAIN;

    done:
    request->check_index = pi+1;
    state = sw_start;
    return 0;
}



