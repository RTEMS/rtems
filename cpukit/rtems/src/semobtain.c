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

THREAD_QUEUE_OBJECT_ASSERT(
  Semaphore_Control,
  Core_control.mutex.Wait_queue
);

THREAD_QUEUE_OBJECT_ASSERT(
  Semaphore_Control,
  Core_control.semaphore.Wait_queue
);

rtems_status_code rtems_semaphore_obtain(
  rtems_id        id,
  rtems_option    option_set,
  rtems_interval  timeout
)
{
  Semaphore_Control *the_semaphore;
  ISR_lock_Context   lock_context;
  Thread_Control    *executing;
  rtems_attribute    attribute_set;
  bool               wait;

  the_semaphore = _Semaphore_Get( id, &lock_context );

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
    MRSP_Status mrsp_status;

    mrsp_status = _MRSP_Seize(
      &the_semaphore->Core_control.mrsp,
      executing,
      wait,
      timeout,
      &lock_context
    );
    return _Semaphore_Translate_MRSP_status_code( mrsp_status );
  } else
#endif
  if ( !_Attributes_Is_counting_semaphore( attribute_set ) ) {
    _CORE_mutex_Seize(
      &the_semaphore->Core_control.mutex,
      executing,
      wait,
      timeout,
      &lock_context
    );
    return _Semaphore_Translate_core_mutex_return_code(
      executing->Wait.return_code
    );
  }

  /* must be a counting semaphore */
  _CORE_semaphore_Seize(
    &the_semaphore->Core_control.semaphore,
    executing,
    wait,
    timeout,
    &lock_context
  );
  return _Semaphore_Translate_core_semaphore_return_code(
    executing->Wait.return_code
  );
}
