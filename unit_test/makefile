PROGRAMS = parse_test threadpool_test
all : $(PROGRAMS)
.PHONY : all

parse_test : ptt_http_parse_test.o ../src/http_parse.h ../src/http_parse.o ../src/http_request.h ../src/http_request.o
	gcc -o parse_test ptt_http_parse_test.c ../src/http_parse.c ../src/http_request.c
	rm ../src/*.o
threadpool_test : ptt_threadpool_test.o ../src/threadpool.h ../src/threadpool.o
	gcc -o threadpool_test ptt_threadpool_test.c ../src/threadpool.c -lpthread
	rm ../src/*.o
clean :
	rm $(PROGRAMS)
