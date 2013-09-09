/**
 * @file
 *
 * @brief POSIX Semaphore Code Translation Error
 * @ingroup POSIXAPI
 */

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

#include <rtems/posix/semaphoreimpl.h>

const int _POSIX_Semaphore_Return_codes[CORE_SEMAPHORE_STATUS_LAST + 1] = {
  0,                   /* CORE_SEMAPHORE_STATUS_SUCCESSFUL */
  EAGAIN,              /* CORE_SEMAPHORE_STATUS_UNSATISFIED_NOWAIT */
  EAGAIN,              /* CORE_SEMAPHORE_WAS_DELETED */
  ETIMEDOUT,           /* CORE_SEMAPHORE_TIMEOUT */
  /* The next error can not occur since we set the maximum
   * count to the largest value the count can hold.
   */
  ENOSYS,              /* CORE_SEMAPHORE_MAXIMUM_COUNT_EXCEEDED */
};
