/**
 * @file
 *
 * @ingroup RTEMSImplClassicSemaphore
 *
 * @brief This source file contains the implementation of
 *   rtems_semaphore_obtain().
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/semimpl.h>
#include <rtems/rtems/optionsimpl.h>
#include <rtems/rtems/statusimpl.h>

THREAD_QUEUE_OBJECT_ASSERT(
  Semaphore_Control,
  Core_control.Wait_queue,
  SEMAPHORE_CONTROL_GENERIC
);

THREAD_QUEUE_OBJECT_ASSERT(
  Semaphore_Control,
  Core_control.Mutex.Recursive.Mutex.Wait_queue,
  SEMAPHORE_CONTROL_MUTEX
);

THREAD_QUEUE_OBJECT_ASSERT(
  Semaphore_Control,
  Core_control.Semaphore.Wait_queue,
  SEMAPHORE_CONTROL_SEMAPHORE
);

#if defined(RTEMS_SMP)
THREAD_QUEUE_OBJECT_ASSERT(
  Semaphore_Control,
  Core_control.MRSP.Wait_queue,
  SEMAPHORE_CONTROL_MRSP
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
  bool                  wait;
  uintptr_t             flags;
  Semaphore_Variant     variant;
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
  wait = !_Options_Is_no_wait( option_set );

  if ( wait ) {
    _Thread_queue_Context_set_enqueue_timeout_ticks( &queue_context, timeout );
  } else {
    _Thread_queue_Context_set_enqueue_do_nothing_extra( &queue_context );
  }

  flags = _Semaphore_Get_flags( the_semaphore );
  variant = _Semaphore_Get_variant( flags );

  switch ( variant ) {
    case SEMAPHORE_VARIANT_MUTEX_INHERIT_PRIORITY:
      status = _CORE_recursive_mutex_Seize(
        &the_semaphore->Core_control.Mutex.Recursive,
        CORE_MUTEX_TQ_PRIORITY_INHERIT_OPERATIONS,
        executing,
        wait,
        _CORE_recursive_mutex_Seize_nested,
        &queue_context
      );
      break;
    case SEMAPHORE_VARIANT_MUTEX_PRIORITY_CEILING:
      status = _CORE_ceiling_mutex_Seize(
        &the_semaphore->Core_control.Mutex,
        executing,
        wait,
        _CORE_recursive_mutex_Seize_nested,
        &queue_context
      );
      break;
    case SEMAPHORE_VARIANT_MUTEX_NO_PROTOCOL:
      status = _CORE_recursive_mutex_Seize(
        &the_semaphore->Core_control.Mutex.Recursive,
        _Semaphore_Get_operations( flags ),
        executing,
        wait,
        _CORE_recursive_mutex_Seize_nested,
        &queue_context
      );
      break;
#if defined(RTEMS_SMP)
    case SEMAPHORE_VARIANT_MRSP:
      status = _MRSP_Seize(
        &the_semaphore->Core_control.MRSP,
        executing,
        wait,
        &queue_context
      );
      break;
#endif
    default:
      _Assert(
        variant == SEMAPHORE_VARIANT_SIMPLE_BINARY
          || variant == SEMAPHORE_VARIANT_COUNTING
      );
      status = _CORE_semaphore_Seize(
        &the_semaphore->Core_control.Semaphore,
        _Semaphore_Get_operations( flags ),
        executing,
        wait,
        &queue_context
      );
      break;
  }

  return _Status_Get( status );
}
