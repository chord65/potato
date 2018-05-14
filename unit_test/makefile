PROGRAMS = parse_test threadpool_test conf_test
all : $(PROGRAMS)
.PHONY : all

parse_test : ptt_http_parse_test.c ../src/http_parse.h ../src/http_parse.c ../src/http_request.h ../src/http_request.c
	gcc -o parse_test ptt_http_parse_test.c ../src/http_parse.c ../src/http_request.c

threadpool_test : ptt_threadpool_test.c ../src/threadpool.h ../src/threadpool.c
	gcc -o threadpool_test ptt_threadpool_test.c ../src/threadpool.c -lpthread

conf_test : ptt_conf_test.c ../src/conf.h ../src/conf.c
	gcc -o conf_test ptt_conf_test.c ../src/conf.c

clean :
	rm $(PROGRAMS)
