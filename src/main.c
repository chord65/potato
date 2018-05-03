#include <stdio.h>
#include "http_request.h"
#include "http_parse.h"

//测试
int main(int argc, char *argv[])
{
    if(argc != 2)
        printf("syntex error!\n");

    char *filename = argv[1];
    int fd = open(filename, O_RDONLY);
    int rd;
    http_request_t *request = (http_request_t *)malloc(sizeof(http_request_t));

    http_request_init(request, fd);

    rd = read(request->fd, request->buffer, BUFFER_SIZE);
    request->read_index += rd;

    if(rd <= 0){
        printf("read error!\n");
        return 1;
    }

    if(http_parse_request_line(request) != 0){
        printf("line parse error!\n");
        return 1;
    }

    if(http_parse_request_header(request) != 0){
        printf("header parse error!\n");
        return 1;
    }

    printf("request:\n%s", request->buffer);
    printf("method = %d\n", request->method);
    printf("uri = ");
    for(void *p = request->uri; p != request->uri_end; p++)
        printf("%c", *(char *)p);
    printf("\n");
    printf("version: %d.%d\n", request->major_digit, request->minor_digit);
    printf("headers:\n");
    for(http_request_header_t *h = request->header_list; h != NULL; h = h->next ){
        for(void *p = h->key; p != h->key_end; p++)
            printf("%c", *(char*)p);
        printf(" : ");
        for(void *p = h->value; p != h->value_end; p++)
            printf("%c", *(char*)p);
        printf("\n");
    }
}