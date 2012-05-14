/*
 *  RWLock Manager -- Translate SuperCore Status
 *
 *  COPYRIGHT (c) 1989-2007.
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
#include <rtems/posix/rwlock.h>

/*
 *  _POSIX_RWLock_Translate_core_rwlock_return_code
 *
 *  Input parameters:
 *    the_rwlock_status - rwlock status code to translate
 *
 *  Output parameters:
 *    status code - translated POSIX status code
 *
 */

static int _POSIX_RWLock_Return_codes[CORE_RWLOCK_STATUS_LAST + 1] = {
  0,                        /* CORE_RWLOCK_SUCCESSFUL */
  EINVAL,                   /* CORE_RWLOCK_DELETED */
  EBUSY,                    /* CORE_RWLOCK_UNAVAILABLE */
  ETIMEDOUT,                /* CORE_RWLOCK_TIMEOUT */
};


int _POSIX_RWLock_Translate_core_RWLock_return_code(
  CORE_RWLock_Status  the_rwlock_status
)
{
  /*
   *  Internal consistency check for bad status from SuperCore
   */
  #if defined(RTEMS_DEBUG)
    if ( the_rwlock_status > CORE_RWLOCK_STATUS_LAST )
      return EINVAL;
  #endif
  return _POSIX_RWLock_Return_codes[the_rwlock_status];
}
