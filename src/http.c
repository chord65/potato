//
// Created by chord on 18-5-7.
//

#include "http.h"
#include "http_request.h"

//存放根目录路径
char *ROOT = NULL;

ptt_mime_type_t ptt_mime[] =
        {
                {".html", "text/html"},
                {".xml", "text/xml"},
                {".xhtml", "application/xhtml+xml"},
                {".txt", "text/plain"},
                {".rtf", "application/rtf"},
                {".pdf", "application/pdf"},
                {".word", "application/msword"},
                {".png", "image/png"},
                {".gif", "image/gif"},
                {".jpg", "image/jpeg"},
                {".jpeg", "image/jpeg"},
                {".au", "audio/basic"},
                {".mpeg", "video/mpeg"},
                {".mpg", "video/mpeg"},
                {".avi", "video/x-msvideo"},
                {".gz", "application/x-gzip"},
                {".tar", "application/x-tar"},
                {".css", "text/css"},
                {NULL ,"text/plain"}
        };

void ptt_do_request(void *args)
{
    ptt_http_request_t *request = (ptt_http_request_t *)args;

    ROOT = request->root;
    int fd = request->fd;
    char *read_ptr = NULL;
    size_t remain_size;
    int n_read, ret;
    char filename[FILE_NAME_LEN];
    struct stat sbuf;
    ptt_http_out_t *out = NULL;

    //删除定时器,处理完一次请求后重置定时器
    ptt_del_timer(request);

    while(1){
        //read_ptr指向缓冲区中下一个可写入的位置，取模运算用于实现循环缓冲
        read_ptr = &request->buffer[request->check_index % BUFFER_SIZE];
        //remain_size存储缓冲区可写入长度
        remain_size = BUFFER_SIZE - (request->check_index % BUFFER_SIZE);
        //读取数据到缓冲区
        n_read = read(fd, read_ptr, remain_size);


        //连接关闭
        if(n_read == 0){
            log_err("the other end closed fd : %d", fd);
            goto have_closed;
        }
        //非阻塞IO，若errno不是EAGAIN，则关闭连接
        if(n_read < 0 && errno != PTT_AGAIN){
            log_err("read erro : errno = %d", errno);
            goto close;
        }
        //交出线程，不关闭socket连接，重新等待数据
        if(n_read < 0 && errno == PTT_AGAIN){
            log_err("read erro : errno = %d", errno);
            break;
        }

        //更新read_index
        request->read_index += n_read;

        log_info("new data from fd %d", fd);

        //解析报文请求行
        ret = ptt_http_parse_request_line(request);
        //如果报文头部不完整，则继续从套接字读取数据到用户缓冲区
        //注意这里的PTT_AGAIN是由于用户缓冲区buffer中数据不完全造成的，而不是由于套接字的缓冲区为空


        if(ret == PTT_AGAIN)
            continue;
        if(ret < 0)
            goto close;

        //解析报文头部
        ret = ptt_http_parse_request_header(request);
        if(ret == PTT_AGAIN)
            continue;
        if(ret < 0)
            goto close;

        //解析uri，获取文件名
        ptt_parse_uri(filename, request->uri, request->uri_end);

        log_info("uri = %s", filename);
        //printf("filename : %s\n", filename);

        //检查文件信息，判断是否存在，以及权限
        if(ptt_file_check(fd, filename, &sbuf) < 0)
            goto close;

        debug("ptt_file_check finish");

        //创建并初始化响应结构
        out = (ptt_http_out_t *)malloc(sizeof(ptt_http_out_t));
        ptt_http_out_init(out, fd);

        //处理请求头部,分发处理函数
        ptt_http_handle_header(request, out);

        //处理静态请求
        if(ptt_serve_static(fd, filename, (size_t)sbuf.st_size, out) < 0){
            if(errno == EPIPE){
                log_err("the other end closed fd : %d", fd);
                free(out);
                goto have_closed;
            }
            free(out);
            goto close;
        }

        log_info("send response successfully");

        //判断是否是长连接
        if(!out->keep_alive){
            log_info("short connection");
            free(out);
            goto close;
        }
        free(out);
    };
    //重新注册epoll事件
    ptt_epoll_mod(request->epoll_fd, request->fd, request, EPOLLIN | EPOLLET | EPOLLONESHOT);
    //重新添加定时器
    ptt_add_timer(request, ptt_close_conn, DEFAULT_TIMEOUT);

    //不关闭连接，交还worker线程使用权，等待epoll再次通知
    return;

    have_closed:
    //若描述符已被关闭,则释放request并返回
    free(request);
    return;

    close:

    //关闭连接，释放请求结构
    ptt_close_conn(request);

    return;
}

void ptt_parse_uri(char *filename, char *uri_start, char *uri_end)
{
    *uri_end = '\0';

    char *delim_char;
    //向filename添加路径
    strcpy(filename, ROOT);
    //若uri为空，则写入默认html文件名
    if(uri_start == uri_end){
        strncat(filename, DEFAULT_HTML, strlen(DEFAULT_HTML));
        return;
    }
    //查找'?'的位置
    delim_char = strchr(uri_start, '?');
    //如果没有'?',则将uri整体剪切后返回
    if(delim_char == NULL) {
        strncat(filename, uri_start,(size_t)(uri_end-uri_start));
        return;
    }
    strncat(filename, uri_start,(size_t)(delim_char-uri_start));
    return;
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

void ptt_do_error(int fd, char *filename, int status, char *short_msg, char *long_msg)
{
    char head[HTTP_HEAD_LEN];
    char body[HTTP_BODY_LEN];

    //写入响应报文主体
    sprintf(body, "<!DOCTYPE html>");
    sprintf(body, "%s<html><head><title>Potato error</title></head>", body);
    sprintf(body, "%s<body><p align=\"center\">%d %s</p>", body, status, short_msg);
    sprintf(body, "%s<p align=\"center\">%s : %s</p>", body, long_msg, filename);
    sprintf(body, "%s<hr/><p align=\"center\"><em>Potato web server</em></p></body></html>", body);

    //写入响应报文首部
    sprintf(head, "HTTP/1.1 %d %s\r\n", status, short_msg);
    sprintf(head, "%sServer: Potato\r\n", head);
    sprintf(head, "%sContent-type: text/html\r\n", head);
    sprintf(head, "%sContent-length: %d\r\n", head, (int)strlen(body));
    sprintf(head, "%sConnection: Close\r\n\r\n", head);

    rio_writen(fd, head, strlen(head));
    rio_writen(fd, body, strlen(body));
}

int ptt_serve_static(int fd, char *filename, size_t file_size, ptt_http_out_t *out)
{
    char head[HTTP_HEAD_LEN];

    //写入响应报文首部
    sprintf(head, "HTTP/1.1 %d %s\r\n", out->status, ptt_get_shortmsg_from_status_code(out->status));
    sprintf(head, "%sContent-type: %s\r\n", head, ptt_get_file_type(filename));
    sprintf(head, "%sContent-length: %d\r\n", head, (int)file_size);

    if(out->keep_alive){
        sprintf(head, "%sConnection: keep-alive\r\n", head);
        sprintf(head, "%sKeep-Alive: timeout=%d\r\n", head, DEFAULT_TIMEOUT);
    }
    else{
        sprintf(head, "%sConnection: close\r\n", head);
    }

    sprintf(head, "%sServer: Potato\r\n", head);
    sprintf(head, "%s\r\n", head);

    //发送响应头部并验证是否完整
    ssize_t head_len = rio_writen(fd, head, strlen(head));
    if(head_len != strlen(head)){
        perror("head send failed");
        return -1;
    }

    //打开文件并映射到内存
    int src_fd = open(filename, O_RDONLY, 0);
    void *src_addr = mmap(0, file_size,PROT_READ, MAP_PRIVATE, src_fd, 0);
    close(src_fd);

    //发送文件并校验完整性
    ssize_t body_len = rio_writen(fd, src_addr, file_size);
    if(body_len != file_size){
        /* 输出log在rio_writen中完成
        char error_msg[ERROR_MSG_LEN];
        sprintf(error_msg, "filename = %s\nbodylen = %ld\nfile send failed", filename, body_len);
        perror(error_msg);
         */
        munmap(src_addr, file_size);
        return -1;
    }
    munmap(src_addr, file_size);

    return 0;
}

//获取文件的mime类型
const char*ptt_get_file_type(char *filename)
{
    char *type, *t;

    do{
        //找到文件名中最后一个'.'
        type = strchr(filename, '.');
        t = type;
        if(t == NULL)
            break;

        while((type = strchr(type+1, '.')) != NULL)
            t = type;
        if(t == NULL)
            break;
        type = t;

        //匹配文件后缀和mime类型
        for(int i = 0; ptt_mime[i].key != NULL; i++){
            if(strcasecmp(type, ptt_mime[i].key) == 0)
                return ptt_mime[i].type;
        }
    }while(0);

    return "text/plain";
}

// 根据状态码返回shortmsg
const char* ptt_get_shortmsg_from_status_code(int status){
    if(status == PTT_HTTP_OK){
        return "OK";
    }
    if(status == PTT_HTTP_NOT_FOUND){
        return "Not Found";
    }
    return "Unknown";
}
