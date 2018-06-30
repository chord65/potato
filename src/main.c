#include <stdio.h>
#include <signal.h>
#include "http.h"
#include "conf.h"
#include "timer.h"

#define DEFAULT_CONF "Potato.conf"


int main(int argc, char *argv[])
{
    //配置信息结构体
    ptt_conf_t *conf = (ptt_conf_t *)malloc(sizeof(ptt_conf_t));
    //请求结构
    ptt_http_request_t *request = (ptt_http_request_t *)malloc(sizeof(ptt_http_request_t));
    //线程池
    ptt_threadpool_t *tdpool = NULL;
    //epoll事件数组
    struct epoll_event events[MAXEVENTS];
    //epoll描述符
    int epoll_fd;
    //监听套接字
    int listen_fd;

    //超时时间
    time_t outtime = 50;
    //等待时间
    time_t waiting_time = 0;

    //忽略信号SIGPIPE,防止在对端关闭的情况下向socket写数据引起SIGPIE信号，导致进程终止
    if(signal(SIGPIPE, SIG_IGN) == SIG_ERR){
        perror("SIGPIPE signal error");
        return -1;
    }

    //初始化配置
    int rc = ptt_read_conf(conf, DEFAULT_CONF);
    if(rc < 0)
        return -1;

    printf("port = %d\nthread_num = %d\nroot=%s\n", conf->port, conf->thread_num, conf->root);

    //初始化线程池
    if((tdpool = ptt_threadpool_init(conf->thread_num)) == NULL){
        perror("threadpool init error");
        return -1;
    }

    //初始化epoll
    epoll_fd = ptt_epoll_create(0);
    if(epoll_fd == -1){
        perror("epoll create error");
        return -1;
    }

    //绑定监听套接字
    listen_fd = ptt_sock_bind_listen(conf->port);
    if(listen_fd < 0){
        perror("socket bind or listen error");
        return -1;
    }

    //设置监听套接字非阻塞
    ptt_set_sock_nonblock(listen_fd);

    //初始化请求结构
    ptt_http_request_init(request, listen_fd, epoll_fd, conf->root);

    //注册listen_fd到epoll
    ptt_epoll_add(epoll_fd, listen_fd, request, EPOLLIN  );

    //初始化定时器
    ptt_timer_init();

    //epoll响应事件数
    int events_num;

    printf("Potato start working!\n");
    while(1){
        //获取距下次超时的等待时间
        waiting_time = ptt_get_waiting_time();
        //等待epoll响应事件
        events_num = ptt_epoll_wait(epoll_fd, events, MAXEVENTS, waiting_time);
        //处理事件,并分发操作
        ptt_handle_events(epoll_fd, tdpool, listen_fd, events, events_num, conf->root);
        //处理超时连接,该操作优先级相对较低，所以放在最后
        ptt_handle_expire_timer();
    }

}
