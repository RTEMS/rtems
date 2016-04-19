/**
 *  @file
 *
 *  @brief RTEMS Semaphore Flush
 *  @ingroup ClassicSem
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/attrimpl.h>
#include <rtems/score/isr.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/semimpl.h>
#include <rtems/score/coremuteximpl.h>
#include <rtems/score/coresemimpl.h>
#include <rtems/score/thread.h>

#include <rtems/score/interr.h>

rtems_status_code rtems_semaphore_flush(
  rtems_id        id
)
{
  Semaphore_Control          *the_semaphore;
  Objects_Locations           location;
  ISR_lock_Context            lock_context;
  rtems_attribute             attribute_set;

  the_semaphore = _Semaphore_Get_interrupt_disable(
    id,
    &location,
    &lock_context
  );
  switch ( location ) {

    case OBJECTS_LOCAL:
      attribute_set = the_semaphore->attribute_set;
#if defined(RTEMS_SMP)
      if ( _Attributes_Is_multiprocessor_resource_sharing( attribute_set ) ) {
        _ISR_lock_ISR_enable( &lock_context );
        return RTEMS_NOT_DEFINED;
      } else
#endif
      if ( !_Attributes_Is_counting_semaphore( attribute_set ) ) {
        _CORE_mutex_Acquire_critical(
          &the_semaphore->Core_control.mutex,
          &lock_context
        );
        _CORE_mutex_Flush(
          &the_semaphore->Core_control.mutex,
          _CORE_mutex_Unsatisfied_nowait,
          _Semaphore_MP_Send_object_was_deleted,
          id,
          &lock_context
        );
      } else {
        _CORE_semaphore_Acquire_critical(
          &the_semaphore->Core_control.semaphore,
          &lock_context
        );
        _CORE_semaphore_Flush(
          &the_semaphore->Core_control.semaphore,
          _Semaphore_MP_Send_object_was_deleted,
          id,
          &lock_context
        );
      }
      return RTEMS_SUCCESSFUL;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
#endif

    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
