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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/semimpl.h>

rtems_status_code rtems_semaphore_flush( rtems_id id )
{
  Semaphore_Control    *the_semaphore;
  Thread_queue_Context  queue_context;
  uintptr_t             flags;
  Semaphore_Variant     variant;

  the_semaphore = _Semaphore_Get( id, &queue_context );

  if ( the_semaphore == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    if ( _Semaphore_MP_Is_remote( id ) ) {
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
    }
#endif

    return RTEMS_INVALID_ID;
  }

  _Thread_queue_Acquire_critical(
    &the_semaphore->Core_control.Wait_queue,
    &queue_context
  );
  _Thread_queue_Context_set_MP_callout(
    &queue_context,
    _Semaphore_MP_Send_object_was_deleted
  );
  flags = _Semaphore_Get_flags( the_semaphore );
  variant = _Semaphore_Get_variant( flags );

  switch ( variant ) {
#if defined(RTEMS_SMP)
    case SEMAPHORE_VARIANT_MRSP:
      _Thread_queue_Release(
        &the_semaphore->Core_control.Wait_queue,
        &queue_context
      );
      return RTEMS_NOT_DEFINED;
#endif
    default:
      _Assert(
        variant == SEMAPHORE_VARIANT_MUTEX_INHERIT_PRIORITY
          || variant == SEMAPHORE_VARIANT_MUTEX_PRIORITY_CEILING
          || variant == SEMAPHORE_VARIANT_MUTEX_NO_PROTOCOL
          || variant == SEMAPHORE_VARIANT_SIMPLE_BINARY
          || variant == SEMAPHORE_VARIANT_COUNTING
      );
      _Thread_queue_Flush_critical(
        &the_semaphore->Core_control.Wait_queue.Queue,
        _Semaphore_Get_operations( flags ),
        _Thread_queue_Flush_status_unavailable,
        &queue_context
      );
      break;
  }

  return RTEMS_SUCCESSFUL;
}
