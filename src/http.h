//
// Created by chord on 18-5-7.
//

#ifndef HTTP_H
#define HTTP_H

#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "http_request.h"
#include "http_parse.h"
#include "rio.h"
#include "socket.h"
#include "epoll.h"

#define FILE_NAME_LEN 255
#define HTTP_HEAD_LEN 256
#define HTTP_BODY_LEN 512
#define ERROR_MSG_LEN 200
#define DEFAULT_HTML "index.html"

#define MIN(a,b) {(a) > (b) ? (a) : (b)}

//存储文件后缀与mime类型的映射
typedef struct {
    char *key;
    char *type;
}ptt_mime_type_t;

//处理请求报文并返回响应
void ptt_do_request(void *args);
//解析uri，获取文件名
void ptt_parse_uri(char *filename, char *uri_start, char *uri_end);
//检查文件，判断是否存在及访问权限
int ptt_file_check(int fd, char *filename, struct stat *sbuf);
//处理错误信息，并返回响应报文
void ptt_do_error(int fd, char *filename, int status, char *short_mag, char *long_msg);
//处理静态请求
int ptt_serve_static(int fd, char *filename, size_t file_size, ptt_http_out_t *out);
// 根据状态码返回shortmsg
const char* ptt_get_shortmsg_from_status_code(int status_code);
//获取文件的mime类型
const char*ptt_get_file_type(char *filename);

#endif //HTTP_H
