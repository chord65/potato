#include "../src/conf.h"

int main()
{
	ptt_conf_t *conf = (ptt_conf_t *)malloc(sizeof(ptt_conf_t));

	int ret;
	ret = ptt_read_conf(conf, "conf.txt");
	//printf("ret = %d\n", ret);

	printf("port = %d\nthread_num = %d\nroot = %s\n", conf->port, conf->thread_num, conf->root);
}
