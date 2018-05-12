//
// Created by chord on 18-5-7.
//

#ifndef HTTP_H
#define HTTP_H

#include "http_request.h"
#include "http_parse.h"


//存储文件后缀与mime类型的映射
typedef struct {
    char *key;
    char *type;
} ptt_mime_type_t;


void ptt_do_request(void *args);

#endif //HTTP_H
