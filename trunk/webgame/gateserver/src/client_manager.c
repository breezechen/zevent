#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <pthread.h>
#include "znet.h"
#include "client_manager.h"
#include "json_helper.h"
#include "thread_mutex.h"
#include "hash.h"
#include "client.h"
#include "netcmd.h"
#include "cm_logic.h"

typedef struct client_manager_t client_manager_t;
struct client_manager_t{
    net_server_t *ns;

    thread_mutex_t *mutex_clients;
    hash_t *clients;

    pthread_t thread_id;
    int stop_daemon;
};

static client_manager_t *cm = NULL;

static int init_ns_arg(ns_arg_t *ns_arg)
{
    ns_arg->func = NULL;
    json_object *jfile = json_object_from_file("config.json");
    json_object *jip = json_util_get(jfile,"CONFIG.clients-server-address");
    if(!jip)
	return -1;
    const char *ip = json_object_get_string(jip);
    snprintf(ns_arg->ip,sizeof(ns_arg->ip),ip);

    json_object *jport = json_util_get(jfile,"CONFIG.clients-server-port");
    if(!jport)
	return -1;
    int port = json_object_get_int(jport);
    ns_arg->port = port;
    json_object_put(jfile);
    return 0;
}

#define HEADER_LEN (4)
static int process_msg(void *msg,int len,uint64_t peerid)
{
    char *body = (char *)msg + HEADER_LEN;
    json_object *jmsg = json_tokener_parse(body);
    if(!jmsg)
	return -1;
    json_object *jcmd = json_util_get(jmsg,"cmd");
    if(!jcmd)
	return -1;
    int cmd = json_object_get_int(jcmd);
    switch(cmd){
	case MSGID_REQUEST_NEWACCOUNT:
	    cm_logic_createaccount(peerid,jmsg);
	    break;
    }
    json_object_put(jmsg);
    return 0;
}

#define MS_PER_SECOND (1000000)
static void *thread_entry(void *arg)
{
    int rv;
    client_manager_t *cm = (client_manager_t *)arg;
    net_server_t *ns = cm->ns;
    void *msg;uint32_t len;

    while(!cm->stop_daemon)
    {
	uint64_t peer_id;
	rv = ns_recvmsg(ns,&msg,&len,&peer_id,MS_PER_SECOND);
	if(rv < 0)
	    continue;
	if(rv == 0)
	{
	    process_msg(msg,len,peer_id);
	    //process msg
	    ns_free(ns,msg);
	}
	if(rv == 1)
	{
	    //disconnect
	}
    }
    return NULL;
}

int cm_start()
{
    cm = (client_manager_t *)malloc(sizeof(client_manager_t));
    ns_arg_t ns_arg;
    int rv = init_ns_arg(&ns_arg);
    if(rv < 0)
	return -1;
    printf("cm ip:%s,port:%d\n",ns_arg.ip,ns_arg.port);
    rv = ns_start_daemon(&cm->ns,&ns_arg);
    if(rv < 0)
	return -1;

    thread_mutex_create(&cm->mutex_clients,THREAD_MUTEX_DEFAULT);
    cm->clients = hash_make();
    cm->stop_daemon = 0;
    //start the main loop thread
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&cm->thread_id, &attr, thread_entry, cm);
    pthread_attr_destroy(&attr);

    return 0;
}

int cm_send2clients(char *msg)
{
    return 0;
}

int cm_send2client(uint64_t peerid,void *buf,uint32_t len)
{
    ns_sendmsg(cm->ns,peerid,buf,len);
    return 0;
}

int cm_stop()
{
    cm->stop_daemon = 1;
    pthread_join(cm->thread_id,NULL);
    ns_stop_daemon(cm->ns);
    return 0;
}

int cm_destroy()
{
    hash_destroy(cm->clients);
    thread_mutex_destroy(cm->mutex_clients);

    free(cm);
    cm = NULL;
    return 0;
}
