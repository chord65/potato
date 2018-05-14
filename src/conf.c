//
// Created by chord on 18-5-11.
//

#include "conf.h"

//读取配置文件
int ptt_read_conf(ptt_conf_t *conf, char *filename)
{
    FILE *fp = fopen(filename, "r");
    if(fp == NULL)
        return -1;

    char line_buf[BUFLINE];
    char* deli_pos;

    while(fgets(line_buf, BUFLINE, fp)){
        //确定界符‘=’的位置
        deli_pos = strchr(line_buf, DELI_CHAR);

        //获取port值
        if(strncmp(line_buf, "port", 4) == 0){
            conf->port = atoi(deli_pos+1);
            //continue;
        }
        //获取thread_num值
        if(strncmp(line_buf, "thread_num", 10) == 0){
            conf->thread_num = atoi(deli_pos+1);
            //continue;
        }
        //获取root
        if(strncmp(line_buf, "root", 4) == 0){
            int i = 0;
            deli_pos += 1;
            //逐字符复制root
            //配置文件中root字段值必须以#结束
            while(*deli_pos != '#'){
                conf->root[i++] = *deli_pos;
                deli_pos++;
            }
            conf->root[i] = '\0';

            //continue;
        }

        memset(line_buf, 0, BUFLINE);
    }

    fclose(fp);
    return 0;
}