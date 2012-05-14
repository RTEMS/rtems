/*
 *  Barrier Manager -- Translate SuperCore Status
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
#include <rtems/posix/barrier.h>

/*
 *  _POSIX_Barrier_Translate_core_barrier_return_code
 *
 *  Input parameters:
 *    the_barrier_status - barrier status code to translate
 *
 *  Output parameters:
 *    status code - translated POSIX status code
 *
 */

static int _POSIX_Barrier_Return_codes[CORE_BARRIER_STATUS_LAST + 1] = {
  0,                        /* CORE_BARRIER_STATUS_SUCCESSFUL */
  PTHREAD_BARRIER_SERIAL_THREAD,
                            /* CORE_BARRIER_STATUS_AUTOMATICALLY_RELEASED */
  -1,                       /* CORE_BARRIER_WAS_DELETED */
  0                         /* CORE_BARRIER_TIMEOUT */
};


int _POSIX_Barrier_Translate_core_barrier_return_code(
  CORE_barrier_Status  the_barrier_status
)
{
  /*
   *  Internal consistency check for bad status from SuperCore
   */
  #if defined(RTEMS_DEBUG)
    if ( the_barrier_status > CORE_BARRIER_STATUS_LAST )
      return EINVAL;
  #endif
  return _POSIX_Barrier_Return_codes[the_barrier_status];
}
