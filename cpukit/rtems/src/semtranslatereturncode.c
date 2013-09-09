/**
 *  @file
 *
 *  @brief Semaphore Translate Core Mutex and Semaphore Return Code
 *  @ingroup ClassicSem
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/semimpl.h>

const rtems_status_code _Semaphore_Translate_core_mutex_return_code_[] = {
  RTEMS_SUCCESSFUL,         /* CORE_MUTEX_STATUS_SUCCESSFUL */
  RTEMS_UNSATISFIED,        /* CORE_MUTEX_STATUS_UNSATISFIED_NOWAIT */
#if defined(RTEMS_POSIX_API)
  RTEMS_UNSATISFIED,        /* CORE_MUTEX_STATUS_NESTING_NOT_ALLOWED */
#endif
  RTEMS_NOT_OWNER_OF_RESOURCE, /* CORE_MUTEX_STATUS_NOT_OWNER_OF_RESOURCE */
  RTEMS_OBJECT_WAS_DELETED, /* CORE_MUTEX_WAS_DELETED */
  RTEMS_TIMEOUT,            /* CORE_MUTEX_TIMEOUT */
#if defined(__RTEMS_STRICT_ORDER_MUTEX__)
    123,
#endif
  RTEMS_INVALID_PRIORITY   /* CORE_MUTEX_STATUS_CEILING_VIOLATED */
};

const rtems_status_code _Semaphore_Translate_core_semaphore_return_code_[] = {
  RTEMS_SUCCESSFUL,         /* CORE_SEMAPHORE_STATUS_SUCCESSFUL */
  RTEMS_UNSATISFIED,        /* CORE_SEMAPHORE_STATUS_UNSATISFIED_NOWAIT */
  RTEMS_OBJECT_WAS_DELETED, /* CORE_SEMAPHORE_WAS_DELETED */
  RTEMS_TIMEOUT,            /* CORE_SEMAPHORE_TIMEOUT  */
  RTEMS_INTERNAL_ERROR,     /* CORE_SEMAPHORE_MAXIMUM_COUNT_EXCEEDED */
};
