//
// Created by chord on 18-5-7.
//

#ifndef HTTP_H
#define HTTP_H

#include "sys/stat.h"
#include "http_request.h"
#include "http_parse.h"
#include "rio.h"
#include "socket.h"
#include "epoll.h"

#define FILE_NAME_LEN 255
#define DEFAULT_HTML "index.html"

#define MIN(a,b) {(a) > (b) ? (a) : (b)}

//存储文件后缀与mime类型的映射
typedef struct {
    char *key;
    char *type;
} ptt_mime_type_t;

//处理请求报文并返回响应
void ptt_do_request(void *args);
//解析uri，获取文件名
void ptt_parse_uri(char *filename, char *uri_start, char *uri_end);
//检查文件，判断是否存在及访问权限
int ptt_file_check(int fd, char *filename, struct stat *sbuf);
//处理错误信息，并返回响应报文
void ptt_do_error(int fd, char *filename, int status, char *short_mag, char *long_msg);
#endif //HTTP_H
