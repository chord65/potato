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

    //rio_writen(fd, head, strlen(head));
    rio_writen(fd, body, strlen(body));
}

int main()
{
    ptt_do_error(1, "index.html", 404, "Not Found", "Potato can't find the file");
    return 0;
}

