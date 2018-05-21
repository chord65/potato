//
// Created by chord on 18-5-7.
//

#include "rio.h"

//无缓冲的RIO函数
ssize_t rio_readn(int fd, void *usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nread;
    char *bufp = usrbuf;

    while(nleft > 0){
        if((nread = read(fd, bufp, nleft)) < 0){
            if(errno = EINTR)
                nread = 0;  //如果read被信号中断，则重新启动
            else
                return -1;  //errno已经被read函数设置
        }
        else if(nread == 0)
            break;          //nread==0表示读到文件末尾，此时函数直接返回

        nleft -= nread;
        bufp += nread;
    }

    return (n - nleft);
}

ssize_t rio_writen(int fd, void *usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nwriten;
    char *bufp = usrbuf;

    while(nleft > 0){
        if((nwriten = write(fd, bufp, nleft)) <= 0){
            if(errno == EINTR)
                nwriten = 0; //若被中断，则重启
            else{

                log_err("errno = %d", errno);
                /*
                char error_msg[50];
                sprintf(error_msg, "errno = %d\nrio_writen error", errno);
                perror(error_msg);
                 */
                return -1;
            }
        }

        nleft -= nwriten;
        bufp += nwriten;
    }

    return (n - nleft);
}

//初始化rio_t结构
void rio_readinitb(rio_t *rp, int fd)
{
    rp->rio_fd = fd;
    rp->rio_cnt = 0;
    rp->rio_bufptr = rp->rio_buf;
}


//带缓冲的RIO函数
static ssize_t rio_read(rio_t *rp, void *usrbuf, size_t n)
{
    int cnt;

    while(rp->rio_cnt <= 0){
        rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf));
        if(rp->rio_cnt < 0) {
            //若被中断，则重启
            if(errno != EINTR)
                return -1;
        }
        else if(rp->rio_cnt == 0)
            return 0;
        else
            rp->rio_bufptr = rp->rio_buf;
    }

    cnt = n;
    if(rp->rio_cnt < n)
        cnt = rp->rio_cnt;
    //将缓冲区数据写入用户缓冲区
    memcpy(usrbuf, rp->rio_buf, cnt);

    rp->rio_bufptr += cnt;
    rp->rio_cnt -= cnt;

    return cnt;
}

ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen)
{
    int n, rc;
    char c, *bufp = usrbuf;

    for(n = 1; n < maxlen; n++){
        if(rc = rio_read(rp, &c, 1) == 1){
            *bufp++ = c;
            if(c == '\n') {
                n++;
                break;
            }
        }
        else if(rc == 0){
            if(n ==1)
                return 0; //EOF,没有读到数据
            else
                break; //EOF，读到了一些数据
        }
        else
            return -1; //error
    }
    *bufp = 0; //在一行数据的末尾添加空字符
    return n-1;
}

ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nread;
    char *bufp = usrbuf;

    while(nleft > 0){
        if((nread = rio_read(rp, bufp, nleft)) < 0)
            return -1;
        else if(nread == 0)
            break; //EOF

        nleft -= nread;
        bufp += nread;
    }

    return (n - nleft);
}
