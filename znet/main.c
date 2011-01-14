#include <unistd.h>
#include <sys/select.h>

#include <stdio.h>
#include <string.h>
#include "znet.h"
int main()
{
	int rv;
	net_server_t *ns;
	ns_arg_t sinfo;
	sinfo.func = NULL;
	strcpy(sinfo.ip,"127.0.0.1");
	sinfo.port = 8899;
	sinfo.nworkers = 3;
	ns_start_daemon(&ns,&sinfo);
	void *msg;uint32_t len;
	char buf[256];
	memset(buf,0,sizeof(buf));
	int count = 0;


	struct timeval delay;

	while(1)
	{
		delay.tv_sec = 0;
		delay.tv_usec = 50000;//50ms
//		select(0,NULL,NULL,NULL,&delay); 
		rv = ns_recvmsg(ns,&msg,&len);
		if(rv == 0)
		{
			memcpy(buf,(char *)msg,len);
			++count;
			printf("count:%d\n",count);
			ns_free(ns,msg);
		}
	}
	ns_stop_daemon(ns);
	return 0;
}
