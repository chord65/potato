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
                {".txt", "text/plain"},
                {".png", "image/png"},
                {".gif", "image/gif"},
                {".jpg", "image/jpeg"},
                {".jpeg", "image/jpeg"},
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
    ptt_http_out_t *out = (ptt_http_out_t *)malloc(sizeof(ptt_http_out_t));

    while{
        //read_ptr指向缓冲区中下一个可写入的位置，取模运算用于实现循环缓冲
        read_ptr = &request->buffer[request->check_index % BUFFER_SIZE];
        //remain_size存储缓冲区可写入长度
        remain_size = BUFFER_SIZE - (request->check_index % BUFFER_SIZE);
        //读取数据到缓冲区
        n_read = read(fd, read_ptr, remain_size);

        //连接关闭
        if(n_read == 0)
            goto close;
        //非阻塞IO，若errno不是EAGAIN，则关闭连接
        if(n_read < 0 && errno != PTT_AGAIN)
            goto close;
        //交出线程，不关闭socket连接，重新等待数据
        if(n_read < 0 && errno == PTT_AGAIN)
            break;

        //解析报文请求行
        ret = ptt_http_parse_request_line(request);
        //如果报文头部不完整，则继续从套接字读取数据到用户缓冲区
        //注意这里的PTT_AGAIN是由于用户缓冲区buffer中数据不完全造成的，而不是由于套接字的缓冲区为空
        if(ret == PTT_AGAIN)
            continue;
        if(ret < 0)
            goto close;

        //解析报文请部
        ret = ptt_http_parse_request_header(request);
        if(ret == PTT_AGAIN)
            continue;
        if(ret < 0)
            goto close;

        //创建并初始化响应结构
        ptt_http_out_t *out = (ptt_http_out_t *)malloc(sizeof(ptt_http_out_t));
        ptt_http_out_init(out, fd);

        //解析uri，获取文件名
        ptt_parse_uri(filename, request->uri, request->uri_end);

        //检查文件信息，判断是否存在，以及权限
        if(ptt_file_check(request->fd, filename, &sbuf) < 0)
            goto close;

        //处理请求头部,分发处理函数
        ptt_http_handle_header(request->header_list, out);

        //处理静态请求


    };
    //重新注册epoll事件
    ptt_epoll_mod(request->epoll_fd, request->fd, request, EPOLLIN | EPOLLET | EPOLLONESHOT);
    //一次响应未完成的情况下，交还worker线程使用权，等待epoll再次通知
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

void ptt_do_error(int fd, char *filename, int status, char *short_mag, char *long_msg)
{

}


