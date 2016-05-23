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

#include <rtems/rtems/semimpl.h>
#include <rtems/rtems/attrimpl.h>

rtems_status_code rtems_semaphore_flush( rtems_id id )
{
  Semaphore_Control    *the_semaphore;
  Thread_queue_Context  queue_context;
  rtems_attribute       attribute_set;

  the_semaphore = _Semaphore_Get(
    id,
    &queue_context,
    _Semaphore_MP_Send_object_was_deleted
  );

  if ( the_semaphore == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    if ( _Semaphore_MP_Is_remote( id ) ) {
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
    }
#endif

    return RTEMS_INVALID_ID;
  }

  attribute_set = the_semaphore->attribute_set;

#if defined(RTEMS_SMP)
  if ( _Attributes_Is_multiprocessor_resource_sharing( attribute_set ) ) {
    _ISR_lock_ISR_enable( &queue_context.Lock_context );
    return RTEMS_NOT_DEFINED;
  } else
#endif
  if ( !_Attributes_Is_counting_semaphore( attribute_set ) ) {
    _CORE_mutex_Acquire_critical(
      &the_semaphore->Core_control.mutex,
      &queue_context
    );
    _CORE_mutex_Flush(
      &the_semaphore->Core_control.mutex,
      _CORE_mutex_Unsatisfied_nowait,
      &queue_context
    );
  } else {
    _CORE_semaphore_Acquire_critical(
      &the_semaphore->Core_control.semaphore,
      &queue_context
    );
    _CORE_semaphore_Flush(
      &the_semaphore->Core_control.semaphore,
      &queue_context
    );
  }
  return RTEMS_SUCCESSFUL;
}
