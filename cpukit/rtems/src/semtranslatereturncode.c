/*
 *  Semaphore Manager
 *
 *  DESCRIPTION:
 *
 *  This package is the implementation of the Semaphore Manager.
 *  This manager utilizes standard Dijkstra counting semaphores to provide
 *  synchronization and mutual exclusion capabilities.
 *
 *  Directives provided are:
 *
 *     + create a semaphore
 *     + get an ID of a semaphore
 *     + delete a semaphore
 *     + acquire a semaphore
 *     + release a semaphore
 *
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

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/attr.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/sem.h>
#include <rtems/score/coremutex.h>
#include <rtems/score/coresem.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/mpci.h>
#endif
#include <rtems/score/sysstate.h>

#include <rtems/score/interr.h>

/*
 *  _Semaphore_Translate_core_mutex_return_code
 *
 *  Input parameters:
 *    status - mutex status code to translate
 *
 *  Output parameters:
 *    rtems status code - translated RTEMS status code
 *
 */

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

rtems_status_code _Semaphore_Translate_core_mutex_return_code (
  uint32_t   status
)
{
  /*
   *  If this thread is blocking waiting for a result on a remote operation.
   */
  #if defined(RTEMS_MULTIPROCESSING)
    if ( _Thread_Is_proxy_blocking(status) )
      return RTEMS_PROXY_BLOCKING;
  #endif

  /*
   *  Internal consistency check for bad status from SuperCore
   */
  #if defined(RTEMS_DEBUG)
    if ( status > CORE_MUTEX_STATUS_LAST )
      return RTEMS_INTERNAL_ERROR;
  #endif
  return _Semaphore_Translate_core_mutex_return_code_[status];
}

/*
 *  _Semaphore_Translate_core_semaphore_return_code
 *
 *  Input parameters:
 *    status - semaphore status code to translate
 *
 *  Output parameters:
 *    rtems status code - translated RTEMS status code
 *
 */

const rtems_status_code _Semaphore_Translate_core_semaphore_return_code_[] = {
  RTEMS_SUCCESSFUL,         /* CORE_SEMAPHORE_STATUS_SUCCESSFUL */
  RTEMS_UNSATISFIED,        /* CORE_SEMAPHORE_STATUS_UNSATISFIED_NOWAIT */
  RTEMS_OBJECT_WAS_DELETED, /* CORE_SEMAPHORE_WAS_DELETED */
  RTEMS_TIMEOUT,            /* CORE_SEMAPHORE_TIMEOUT  */
  RTEMS_INTERNAL_ERROR,     /* CORE_SEMAPHORE_MAXIMUM_COUNT_EXCEEDED */
};

rtems_status_code _Semaphore_Translate_core_semaphore_return_code (
  uint32_t   status
)
{
  #if defined(RTEMS_MULTIPROCESSING)
    if ( _Thread_Is_proxy_blocking(status) )
      return RTEMS_PROXY_BLOCKING;
  #endif
  /*
   *  Internal consistency check for bad status from SuperCore
   */
  #if defined(RTEMS_DEBUG)
    if ( status > CORE_SEMAPHORE_STATUS_LAST )
      return RTEMS_INTERNAL_ERROR;
  #endif
  return _Semaphore_Translate_core_semaphore_return_code_[status];
}
