#include <stdio.h>
#include "../src/http_request.h"
#include "../src/http_parse.h"

int main(int argc, char *argv[])
{
    if(argc != 2){
	        printf("syntex error!\n");
	        return 1;
	    }

    char *filename = argv[1];
    int fd = open(filename, O_RDONLY);
    int rd;
    ptt_http_request_t *request = (ptt_http_request_t *)malloc(sizeof(ptt_http_request_t));

    ptt_http_request_init(request, fd, 0, NULL);

    rd = read(request->fd, request->buffer, BUFFER_SIZE);
    request->read_index += rd;

    if(rd <= 0){
	        printf("read error!\n");
	        return 1;
	    }

    if(ptt_http_parse_request_line(request) != 0){
	        printf("line parse error!\n");
	        return 1;
	    }

    if(ptt_http_parse_request_header(request) != 0){
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
    for(ptt_http_request_header_t *h = request->header_list; h != NULL; h = h->next ){
	        for(void *p = h->key; p != h->key_end; p++)
	            printf("%c", *(char*)p);
	        printf(" : ");
	        for(void *p = h->value; p != h->value_end; p++)
	            printf("%c", *(char*)p);
	        printf("\n");
	    }
}
