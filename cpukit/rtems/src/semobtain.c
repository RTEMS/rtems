/**
 *  @file
 *
 *  @brief RTEMS Obtain Semaphore
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
#include <rtems/rtems/optionsimpl.h>
#include <rtems/rtems/statusimpl.h>

THREAD_QUEUE_OBJECT_ASSERT(
  Semaphore_Control,
  Core_control.Wait_queue
);

THREAD_QUEUE_OBJECT_ASSERT(
  Semaphore_Control,
  Core_control.mutex.Wait_queue
);

THREAD_QUEUE_OBJECT_ASSERT(
  Semaphore_Control,
  Core_control.semaphore.Wait_queue
);

#if defined(RTEMS_SMP)
THREAD_QUEUE_OBJECT_ASSERT(
  Semaphore_Control,
  Core_control.mrsp.Wait_queue
);
#endif

rtems_status_code rtems_semaphore_obtain(
  rtems_id        id,
  rtems_option    option_set,
  rtems_interval  timeout
)
{
  Semaphore_Control    *the_semaphore;
  Thread_queue_Context  queue_context;
  Thread_Control       *executing;
  rtems_attribute       attribute_set;
  bool                  wait;
  Status_Control        status;

  the_semaphore = _Semaphore_Get( id, &queue_context );

  if ( the_semaphore == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    return _Semaphore_MP_Obtain( id, option_set, timeout );
#else
    return RTEMS_INVALID_ID;
#endif
  }

  executing = _Thread_Executing;
  attribute_set = the_semaphore->attribute_set;
  wait = !_Options_Is_no_wait( option_set );
#if defined(RTEMS_SMP)
  if ( _Attributes_Is_multiprocessor_resource_sharing( attribute_set ) ) {
    status = _MRSP_Seize(
      &the_semaphore->Core_control.mrsp,
      executing,
      wait,
      timeout,
      &queue_context
    );
  } else
#endif
  if ( !_Attributes_Is_counting_semaphore( attribute_set ) ) {
    status = _CORE_mutex_Seize(
      &the_semaphore->Core_control.mutex,
      executing,
      wait,
      timeout,
      &queue_context
    );
  } else {
    /* must be a counting semaphore */
    status = _CORE_semaphore_Seize(
      &the_semaphore->Core_control.semaphore,
      executing,
      wait,
      timeout,
      &queue_context
    );
  }

  return _Status_Get( status );
}
