/*
 *  POSIX Mutex Error Translation
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/score/coremutex.h>
#include <rtems/posix/mutex.h>

/*
 *  _POSIX_Mutex_Translate_core_mutex_return_code
 *
 *  Input parameters:
 *    the_mutex_status - mutex status code to translate
 *
 *  Output parameters:
 *    status code - translated POSIX status code
 *
 */

static int _POSIX_Mutex_Return_codes[CORE_MUTEX_STATUS_LAST + 1] = {
  0,                      /* CORE_MUTEX_STATUS_SUCCESSFUL */
  EBUSY,                  /* CORE_MUTEX_STATUS_UNSATISFIED_NOWAIT */
  EDEADLK,                /* CORE_MUTEX_STATUS_NESTING_NOT_ALLOWED */
  EPERM,                  /* CORE_MUTEX_STATUS_NOT_OWNER_OF_RESOURCE */
  EINVAL,                 /* CORE_MUTEX_WAS_DELETED */
  ETIMEDOUT,              /* CORE_MUTEX_TIMEOUT */
#ifdef __RTEMS_STRICT_ORDER_MUTEX__
  EDEADLK,                /* CORE_MUTEX_RELEASE_NOT_ORDER */
#endif
  EINVAL                  /* CORE_MUTEX_STATUS_CEILING_VIOLATED */
};


int _POSIX_Mutex_Translate_core_mutex_return_code(
  CORE_mutex_Status  the_mutex_status
)
{
  /*
   *  Internal consistency check for bad status from SuperCore
   */
  #if defined(RTEMS_DEBUG)
    if ( the_mutex_status > CORE_MUTEX_STATUS_LAST )
      return EINVAL;
  #endif
  return _POSIX_Mutex_Return_codes[the_mutex_status];
}
