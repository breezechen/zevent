#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "zcache.h"

#define SHARED_SIZE (16*1024*1024)
#define SHARED_FILENAME "testshm.shm"
#define MUTEX_FILENAME "testshm.mutex"
#define TEST_NUM (40000)


void func(char *str, void *arg)
{
	printf("%s\n",str);
}

apr_pool_t *p;
int main(int argc,const char *argv[])
{
	apr_status_t rv;
	apr_initialize();
	atexit(apr_terminate);
	rv = apr_pool_create(&p, NULL);
	if(rv != APR_SUCCESS)
		printf("error\n");

	MCConfigRecord mc;
	mc.pStorageDataMM = NULL;
	mc.pStorageDataRMM = NULL;
	mc.pPool = p;
	mc.nMutexMode = ZCACHE_MUTEXMODE_USED;
	mc.szStorageDataFile = SHARED_FILENAME;
	mc.nStorageDataSize = SHARED_SIZE;
        mc.nStorageMode = ZCACHE_SCMODE_SHMCB;
	mc.nMutexMech = APR_LOCK_DEFAULT;
	mc.szMutexFile = MUTEX_FILENAME;

  //    apr_shm_t *shm = NULL; 
  //	apr_shm_remove(SHARED_FILENAME, p);
        
	zcache_init(&mc,p);
	if(mc.nMutexMode == ZCACHE_MUTEXMODE_USED)
		zcache_mutex_init(&mc,p);

	char key[64];
	int klen ;

	char data[1024];
	int index =0;
	int i,len;
	for(i=0;i<TEST_NUM;++i)
	{
		memset(key,0,sizeof(key));
		sprintf(key,"%04d",i);
		klen = strlen(key);

	        //memset(data,0,sizeof(data));
                //sprintf(data,"%d-test data!",i);
                memset(data,0,sizeof(data));
		memset(data,'a',sizeof(data)-1);

		
                sprintf(data,"%d-test data!",i);
		index = strlen(data);
	        data[index] = '*';	

		len = strlen(data)+1;

		time_t expiry = time(NULL);
		expiry += 1000;
		if(!zcache_store(&mc,(UCHAR*)key,klen, expiry,(void *)data,len))
			printf("error store data key:%s\n",key);
	}
	printf("store data complete!\n");
	zcache_status(&mc,p,func,NULL);

	for(i=0;i<TEST_NUM; ++i)
	{
		sprintf(key,"%04d",i);
		klen = strlen(key);

		void *pdata = zcache_retrieve(&mc,(UCHAR*)key,klen,&len);
		printf("key:%s,data:%s\n",key,(const char *)pdata);
		zcache_remove(&mc,(UCHAR*)key,klen);
	}
	///////////update///////////////
	memset(key,0,sizeof(key));
	sprintf(key,"%04d",0);
	klen = strlen(key);

	memset(data,0,sizeof(data));
	sprintf(data,"0-newtest data!");
	len = strlen(data)+1;

	time_t expiry = time(NULL);
	expiry += 1000;
	if(!zcache_store(&mc,(UCHAR*)key,klen, expiry,(void *)data,len))
		printf("error store data key:%s\n",key);

	sprintf(key,"%04d",0);
	klen = strlen(key);

	void *pdata = zcache_retrieve(&mc,(UCHAR*)key,klen,&len);
	printf("key:%s,newdata:%s\n",key,(const char *)pdata);
	////////////////////////////////////////////
	zcache_status(&mc,p,func,NULL);

	zcache_kill(&mc);
	return 0;
}
