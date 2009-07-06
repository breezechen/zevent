#include "apr.h"
#include "apr_general.h"        /* for signal stuff */
#include "apr_strings.h"
#include "apr_errno.h"
#include "apr_thread_proc.h"
#include "apr_lib.h"
#include "apr_signal.h"

#define APR_WANT_STDIO
#define APR_WANT_STRFUNC
#include "apr_want.h"

#if APR_HAVE_STDARG_H
#include <stdarg.h>
#endif
#if APR_HAVE_UNISTD_H
#include <unistd.h>
#endif

#define CORE_PRIVATE

#include "ap_config.h"
#include "util_time.h"

static apr_file_t *logfile = NULL;

AP_DECLARE(apr_status_t) ap_open_log(apr_pool_t *p,const char *filename)
{
	apr_status_t rc;
	if(!filename){
		return APR_EBADPATH;
	}

	if ((rc = apr_file_open(&logfile, filename,
					APR_APPEND | APR_WRITE | APR_CREATE | APR_LARGEFILE,
					APR_OS_DEFAULT, p)) != APR_SUCCESS) {
		return rc;
	}

	return rc;

}

AP_DECLARE(apr_status_t) ap_open_stderr_log(apr_pool_t *p)
{
	apr_file_open_stderr(&logfile,p);
	return APR_SUCCESS;
}

AP_DECLARE(apr_status_t) ap_replace_stderr_log(apr_pool_t *p,
                                               const char *filename)
{
    apr_file_t *stderr_file;
    apr_status_t rc;
    if (!filename) {
        return APR_EBADPATH;

    }
    if ((rc = apr_file_open(&stderr_file, filename,
                            APR_APPEND | APR_WRITE | APR_CREATE | APR_LARGEFILE,
                            APR_OS_DEFAULT, p)) != APR_SUCCESS) {
        return rc;
    }
    if ((rc = apr_file_open_stderr(&logfile, p)) 
            == APR_SUCCESS) {
        apr_file_flush(logfile);
        if ((rc = apr_file_dup2(logfile, stderr_file, p)) 
                == APR_SUCCESS) {
            apr_file_close(stderr_file);
        }
    }

    return rc;
}


static void log_error_core(const char *file, int line,apr_pool_t *pool,
                           const char *fmt, va_list args)
{
	char errstr[MAX_STRING_LEN];
	apr_size_t len=0;

	if (file) {
#if defined(_OSD_POSIX) || defined(WIN32) || defined(__MVS__)
		char tmp[256];
		char *e = strrchr(file, '/');
#ifdef WIN32
		if (!e) {
			e = strrchr(file, '\\');
		}
#endif

		/* In OSD/POSIX, the compiler returns for __FILE__
		 * a string like: __FILE__="*POSIX(/usr/include/stdio.h)"
		 * (it even returns an absolute path for sources in
		 * the current directory). Here we try to strip this
		 * down to the basename.
		 */
		if (e != NULL && e[1] != '\0') {
			apr_snprintf(tmp, sizeof(tmp), "%s", &e[1]);
			e = &tmp[strlen(tmp)-1];
			if (*e == ')') {
				*e = '\0';
			}
			file = tmp;
		}
#else /* _OSD_POSIX || WIN32 */
		const char *p;
		/* On Unix, __FILE__ may be an absolute path in a
		 * VPATH build. */
		if (file[0] == '/' && (p = strrchr(file, '/')) != NULL) {
			file = p + 1;
		}
#endif /*_OSD_POSIX || WIN32 */
		char time_str[APR_CTIME_LEN];                                          
		apr_ctime(time_str, apr_time_now());
		len += apr_snprintf(errstr + len, MAX_STRING_LEN - len,
				"[%s]%s(%d): ",time_str,file, line);
	}

	len += apr_vsnprintf(errstr+len, MAX_STRING_LEN-len, fmt, args);

	if (logfile) {
		/* Truncate for the terminator (as apr_snprintf does) */
		if (len > MAX_STRING_LEN - sizeof(APR_EOL_STR)) {
			len = MAX_STRING_LEN - sizeof(APR_EOL_STR);
		}
		strcpy(errstr + len, APR_EOL_STR);
		apr_file_puts(errstr, logfile);
		apr_file_flush(logfile);
	}
}

AP_DECLARE(void) ap_log_error(const char *file, int line, apr_pool_t *p,
                               const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    log_error_core(file, line, p, fmt, args);
    va_end(args);
}

AP_DECLARE(void) ap_log_close()
{
	apr_file_close(logfile);
}

