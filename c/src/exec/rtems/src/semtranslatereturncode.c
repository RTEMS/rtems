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
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

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

/*PAGE
 *
 *  _Semaphore_Translate_core_mutex_return_code
 *
 *  Input parameters:
 *    the_mutex_status - mutex status code to translate
 *
 *  Output parameters:
 *    rtems status code - translated RTEMS status code
 *
 */
 
rtems_status_code _Semaphore_Translate_core_mutex_return_code (
  unsigned32 the_mutex_status
)
{
  switch ( the_mutex_status ) {
    case  CORE_MUTEX_STATUS_SUCCESSFUL:
      return RTEMS_SUCCESSFUL;
    case CORE_MUTEX_STATUS_UNSATISFIED_NOWAIT:
      return RTEMS_UNSATISFIED;
    case CORE_MUTEX_STATUS_NESTING_NOT_ALLOWED:
      return RTEMS_UNSATISFIED;
    case CORE_MUTEX_STATUS_NOT_OWNER_OF_RESOURCE:
      return RTEMS_NOT_OWNER_OF_RESOURCE;
    case CORE_MUTEX_WAS_DELETED:
      return RTEMS_OBJECT_WAS_DELETED;
    case CORE_MUTEX_TIMEOUT:
      return RTEMS_TIMEOUT;
    case THREAD_STATUS_PROXY_BLOCKING:
      return RTEMS_PROXY_BLOCKING;
  }
  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  _Semaphore_Translate_core_semaphore_return_code
 *
 *  Input parameters:
 *    the_semaphore_status - semaphore status code to translate
 *
 *  Output parameters:
 *    rtems status code - translated RTEMS status code
 *
 */
 
rtems_status_code _Semaphore_Translate_core_semaphore_return_code (
  unsigned32 the_semaphore_status
)
{
  switch ( the_semaphore_status ) {
    case  CORE_SEMAPHORE_STATUS_SUCCESSFUL:
      return RTEMS_SUCCESSFUL;
    case CORE_SEMAPHORE_STATUS_UNSATISFIED_NOWAIT:
      return RTEMS_UNSATISFIED;
    case CORE_SEMAPHORE_WAS_DELETED:
      return RTEMS_OBJECT_WAS_DELETED;
    case CORE_SEMAPHORE_TIMEOUT:
      return RTEMS_TIMEOUT;
    
    /*
     *  An overflow should not occur in the Classic API.
     */

    case CORE_SEMAPHORE_MAXIMUM_COUNT_EXCEEDED:
      return RTEMS_INTERNAL_ERROR;

    case THREAD_STATUS_PROXY_BLOCKING:
      return RTEMS_PROXY_BLOCKING;
  }
  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
