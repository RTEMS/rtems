/*
 *  rtems_semaphore_flush
 *
 *  DESCRIPTION:
 *
 *  This package is the implementation of the flush directive
 *  of the Semaphore Manager.
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
 *  rtems_semaphore_flush
 *
 *  This directive allows a thread to flush the threads
 *  pending on the semaphore.
 *
 *  Input parameters:
 *    id         - semaphore id
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */

#if defined(RTEMS_MULTIPROCESSING)
#define SEND_OBJECT_WAS_DELETED _Semaphore_MP_Send_object_was_deleted
#else
#define SEND_OBJECT_WAS_DELETED NULL
#endif

rtems_status_code rtems_semaphore_flush(
  rtems_id        id
)
{
  register Semaphore_Control *the_semaphore;
  Objects_Locations           location;

  the_semaphore = _Semaphore_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      if ( !_Attributes_Is_counting_semaphore(the_semaphore->attribute_set) ) {
        _CORE_mutex_Flush(
          &the_semaphore->Core_control.mutex,
          SEND_OBJECT_WAS_DELETED,
          CORE_MUTEX_STATUS_UNSATISFIED_NOWAIT
        );
      } else {
        _CORE_semaphore_Flush(
          &the_semaphore->Core_control.semaphore,
          SEND_OBJECT_WAS_DELETED,
          CORE_SEMAPHORE_STATUS_UNSATISFIED_NOWAIT
        );
      }
      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
#endif

    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
