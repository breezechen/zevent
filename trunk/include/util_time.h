/**
 * @file  util_time.h
 * @brief Apache date-time handling functions
 *
 * @defgroup APACHE_CORE_TIME Date-time handling functions
 * @ingroup  APACHE_CORE
 * @{
 */

#ifndef APACHE_UTIL_TIME_H
#define APACHE_UTIL_TIME_H

#include "apr.h"
#include "apr_time.h"
#include "server.h"
#include "log.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum delta from the current time, in seconds, for a past time
 * to qualify as "recent" for use in the ap_explode_recent_*() functions:
 * (Must be a power of two minus one!)
 */
#define AP_TIME_RECENT_THRESHOLD 15

/**
 * convert a recent time to its human readable components in local timezone
 * @param tm the exploded time
 * @param t the time to explode: MUST be within the last
 *          AP_TIME_RECENT_THRESHOLD seconds
 * @note This is a faster alternative to apr_time_exp_lt that uses
 *       a cache of pre-exploded time structures.  It is useful for things
 *       that need to explode the current time multiple times per second,
 *       like loggers.
 * @return APR_SUCCESS iff successful
 */
AP_DECLARE(apr_status_t) ap_explode_recent_localtime(apr_time_exp_t *tm,
                                                     apr_time_t t);

/**
 * convert a recent time to its human readable components in GMT timezone
 * @param tm the exploded time
 * @param t the time to explode: MUST be within the last
 *          AP_TIME_RECENT_THRESHOLD seconds
 * @note This is a faster alternative to apr_time_exp_gmt that uses
 *       a cache of pre-exploded time structures.  It is useful for things
 *       that need to explode the current time multiple times per second,
 *       like loggers.
 * @return APR_SUCCESS iff successful
 */
AP_DECLARE(apr_status_t) ap_explode_recent_gmt(apr_time_exp_t *tm,
                                               apr_time_t t);


/**
 * format a recent timestamp in the ctime() format.
 * @param date_str String to write to.
 * @param t the time to convert 
 */
AP_DECLARE(apr_status_t) ap_recent_ctime(char *date_str, apr_time_t t);

/**
 * format a recent timestamp in the RFC822 format
 * @param date_str String to write to (must have length >= APR_RFC822_DATE_LEN)
 * @param t the time to convert 
 */
AP_DECLARE(apr_status_t) ap_recent_rfc822_date(char *date_str, apr_time_t t);

#ifdef __cplusplus
}
#endif

#endif  /* !APACHE_UTIL_TIME_H */
/** @} */
