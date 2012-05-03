/*
 *  POSIX Semaphore Error Translation
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
#include <rtems/score/coresem.h>
#include <rtems/posix/semaphore.h>

/*
 *  _POSIX_Semaphore_Translate_core_semaphore_return_code
 *
 *  Input parameters:
 *    the_semaphore_status - semaphore status code to translate
 *
 *  Output parameters:
 *    status code - translated POSIX status code
 *
 */

static int _POSIX_Semaphore_Return_codes[CORE_SEMAPHORE_STATUS_LAST + 1] = {
  0,                   /* CORE_SEMAPHORE_STATUS_SUCCESSFUL */
  EAGAIN,              /* CORE_SEMAPHORE_STATUS_UNSATISFIED_NOWAIT */
  EAGAIN,              /* CORE_SEMAPHORE_WAS_DELETED */
  ETIMEDOUT,           /* CORE_SEMAPHORE_TIMEOUT */
  /* The next error can not occur since we set the maximum
   * count to the largest value the count can hold.
   */
  ENOSYS,              /* CORE_SEMAPHORE_MAXIMUM_COUNT_EXCEEDED */
};


int _POSIX_Semaphore_Translate_core_semaphore_return_code(
  CORE_semaphore_Status  the_semaphore_status
)
{
  /*
   *  Internal consistency check for bad status from SuperCore
   */
  #if defined(RTEMS_DEBUG)
    if ( the_semaphore_status > CORE_SEMAPHORE_STATUS_LAST )
      return EINVAL;
  #endif
  return _POSIX_Semaphore_Return_codes[the_semaphore_status];
}
