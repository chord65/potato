#include <stdio.h>
#include <string.h>

#define DEFAULT_HTML "index.html"

char ROOT[] = "./";

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

int main()
{
	char uri[] = "";
	char *uri_end = uri+strlen(uri);
	char filename[100];
	
	ptt_parse_uri(filename, uri, uri_end);
	printf("%s\n", filename);
	return 0;
}
