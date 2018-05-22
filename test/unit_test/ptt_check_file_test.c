#include <stdio.h>
#include "../src/rio.h"
#include "../src/http.h"

void ptt_do_error(int fd, char *filename, int status, char *short_msg, char *long_msg)
{
    char head[HTTP_HEAD_LEN];
    char body[HTTP_BODY_LEN];

    //写入响应报文主体
    sprintf(body, "<!DOCTYPE html>\n");
    sprintf(body, "%s<html><head><title>Potato error</title></head>\n", body);
    sprintf(body, "%s<body><p align=\"center\">%d %s</p>\n", body, status, short_msg);
    sprintf(body, "%s<p align=\"center\">%s : %s</p>\n", body, long_msg, filename);
    sprintf(body, "%s<hr/><p align=\"center\"><em>Potato web server</em></p></body></html>\n", body);

    //写入响应报文首部
    sprintf(head, "HTTP/1.1 %d %s\r\n", status, short_msg);
    sprintf(head, "%sServer: Potato\r\n", head);
    sprintf(head, "%sContent-type: text/html\r\n", head);
    sprintf(head, "%sContent-length: %d\r\n", head, (int)strlen(body));
    sprintf(head, "%sConnection: close\r\n\r\n", head);

    rio_writen(fd, head, strlen(head));
    rio_writen(fd, body, strlen(body));
}


int ptt_file_check(int fd, char *filename, struct stat *sbuf)
{
    int ret;
    //判断文件是否存在
    if((ret = access(filename, F_OK)) < 0){
        ptt_do_error(fd, filename, PTT_HTTP_NOT_FOUND, "Not Found", "Potato can't find the file");
	    return -1;
	}
	//获取文件信息
	if(stat(filename, sbuf) < 0){
		ptt_do_error(fd, filename, PTT_HTTP_NOT_FOUND, "Not Found", "Potato can't find the file");	
		return -1;
	}
	//判断文件是不是普通文件和有没有读权限，
	if(!S_ISREG(sbuf->st_mode) || !(S_IRUSR & sbuf->st_mode)){
		ptt_do_error(fd, filename, PTT_HTTP_FORBIDDEN, "Forbidden", "Potato can't read the file");
		return -1;
	}
	
	return 0;
}

int main(int argc, char **argv)
{
	struct stat sbuf;
    ptt_file_check(1, argv[1], &sbuf);
	return 0;
}

