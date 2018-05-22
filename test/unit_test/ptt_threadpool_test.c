#include <stdio.h>
#include <unistd.h>
#include "../src/threadpool.h"

void func1(void *args)
{
	//sleep(3);
	printf("thread1 work\n");
	return;
}

void func2(void *args)
{
	//sleep(2);
	printf("thread2 work\n");
	return;

}

void func3(void *args)
{
	//sleep(1);
	printf("thread3 work\n");
	return;
}

int main()
{
	ptt_threadpool_t *pool = NULL;
	int ret;

	//初始化线程池
	if((pool = ptt_threadpool_init(4)) == NULL){
		printf("pool init error\n");
		return 1;
	}
	
	//添加任务
	if((ret = ptt_threadpool_add(pool, func1, NULL)) != 0){
		printf("func1 add error : %d\n", ret);
		return 1;
	}
	if(ptt_threadpool_add(pool, func2, NULL) != 0){
		printf("func2 add error\n");
		return 1;
	}
	if(ptt_threadpool_add(pool, func3, NULL) != 0){
		printf("func3 add error\n");
		return 1;
	}

	printf("destroy begining\n");

	//sleep(3);
	//销毁线程池
	ptt_threadpool_destroy(pool);

	printf("done\n");

	return 0;
}
