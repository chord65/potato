//
// Created by chord on 18-5-11.
//

#ifndef CONF_H
#define CONF_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PATHLEN 128
#define BUFLINE 64

#define DELI_CHAR '='

//配置结构
typedef struct{
    int port; //监听端口号
    int thread_num; //线程数
    char root[PATHLEN]; //文件根目录
}ptt_conf_t;

int ptt_read_conf(ptt_conf_t *conf, char *filename);

#endif //CONF_H
