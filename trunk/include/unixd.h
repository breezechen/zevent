/**
 * @file  unixd.h
 * @brief common stuff that unix MPMs will want 
 *
 * @addtogroup APACHE_OS_UNIX
 * @{
 */

#ifndef UNIXD_H
#define UNIXD_H

#include "ap_listen.h"
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif
#include "apr_hooks.h"
#include "apr_thread_proc.h"
#include "apr_proc_mutex.h"
#include "apr_global_mutex.h"

#include <pwd.h>
#include <grp.h>
#ifdef APR_HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_IPC_H
#include <sys/ipc.h>
#endif

typedef struct {
    uid_t uid;
    gid_t gid;
    int userdir;
} ap_unix_identity_t;

//AP_DECLARE_HOOK(ap_unix_identity_t *, get_suexec_identity,(const request_rec *r))


/* Default user name and group name. These may be specified as numbers by
 * placing a # before a number */

#ifndef DEFAULT_USER
#define DEFAULT_USER "#-1"
#endif
#ifndef DEFAULT_GROUP
#define DEFAULT_GROUP "#-1"
#endif

typedef struct {
    const char *user_name;
    uid_t user_id;
    gid_t group_id;
    int suexec_enabled;
} unixd_config_rec;
AP_DECLARE_DATA extern unixd_config_rec unixd_config;

AP_DECLARE(int) unixd_setup_child(void);
AP_DECLARE(void) unixd_pre_config(apr_pool_t *ptemp);
/**
 * One of the functions to set mutex permissions should be called in
 * the parent process on platforms that switch identity when the 
 * server is started as root.
 * If the child init logic is performed before switching identity
 * (e.g., MPM setup for an accept mutex), it should only be called
 * for SysV semaphores.  Otherwise, it is safe to call it for all
 * mutex types.
 */
AP_DECLARE(apr_status_t) unixd_set_proc_mutex_perms(apr_proc_mutex_t *pmutex);
AP_DECLARE(apr_status_t) unixd_set_global_mutex_perms(apr_global_mutex_t *gmutex);
AP_DECLARE(apr_status_t) unixd_accept(void **accepted, ap_listen_rec *lr, apr_pool_t *ptrans);

#ifdef HAVE_KILLPG
#define unixd_killpg(x, y)	(killpg ((x), (y)))
#define ap_os_killpg(x, y)      (killpg ((x), (y)))
#else /* HAVE_KILLPG */
#define unixd_killpg(x, y)	(kill (-(x), (y)))
#define ap_os_killpg(x, y)      (kill (-(x), (y)))
#endif /* HAVE_KILLPG */

#define UNIX_DAEMON_COMMANDS	\
AP_INIT_TAKE1("User", unixd_set_user, NULL, RSRC_CONF, \
  "Effective user id for this server"), \
AP_INIT_TAKE1("Group", unixd_set_group, NULL, RSRC_CONF, \
  "Effective group id for this server")

#endif
/** @} */
