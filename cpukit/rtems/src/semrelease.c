/**
 * @file
 *
 * @brief RTEMS Semaphore Release
 * @ingroup ClassicSem Semaphores
 *
 * This file contains the implementation of the Classic API directive
 * rtems_semaphore_release().
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
#include <rtems/rtems/statusimpl.h>

rtems_status_code rtems_semaphore_release( rtems_id id )
{
  Semaphore_Control    *the_semaphore;
  Thread_queue_Context  queue_context;
  Thread_Control       *executing;
  Status_Control        status;

  the_semaphore = _Semaphore_Get( id, &queue_context );

  if ( the_semaphore == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    return _Semaphore_MP_Release( id );
#else
    return RTEMS_INVALID_ID;
#endif
  }

  executing = _Thread_Executing;

  _Thread_queue_Context_set_MP_callout(
    &queue_context,
    _Semaphore_Core_mutex_mp_support
  );

  switch ( the_semaphore->variant ) {
    case SEMAPHORE_VARIANT_MUTEX_INHERIT_PRIORITY:
      status = _CORE_recursive_mutex_Surrender(
        &the_semaphore->Core_control.Mutex.Recursive,
        CORE_MUTEX_TQ_PRIORITY_INHERIT_OPERATIONS,
        executing,
        &queue_context
      );
      break;
    case SEMAPHORE_VARIANT_MUTEX_PRIORITY_CEILING:
      status = _CORE_ceiling_mutex_Surrender(
        &the_semaphore->Core_control.Mutex,
        executing,
        &queue_context
      );
      break;
    case SEMAPHORE_VARIANT_MUTEX_NO_PROTOCOL:
      status = _CORE_recursive_mutex_Surrender(
        &the_semaphore->Core_control.Mutex.Recursive,
        _Semaphore_Get_operations( the_semaphore ),
        executing,
        &queue_context
      );
      break;
    case SEMAPHORE_VARIANT_SIMPLE_BINARY:
      status = _CORE_semaphore_Surrender(
        &the_semaphore->Core_control.Semaphore,
        _Semaphore_Get_operations( the_semaphore ),
        1,
        &queue_context
      );
      _Assert(
        status == STATUS_SUCCESSFUL
          || status == STATUS_MAXIMUM_COUNT_EXCEEDED
      );
      status = STATUS_SUCCESSFUL;
      break;
#if defined(RTEMS_SMP)
    case SEMAPHORE_VARIANT_MRSP:
      status = _MRSP_Surrender(
        &the_semaphore->Core_control.MRSP,
        executing,
        &queue_context
      );
      break;
#endif
    default:
      _Assert( the_semaphore->variant == SEMAPHORE_VARIANT_COUNTING );
      status = _CORE_semaphore_Surrender(
        &the_semaphore->Core_control.Semaphore,
        _Semaphore_Get_operations( the_semaphore ),
        UINT32_MAX,
        &queue_context
      );
      break;
  }

  return _Status_Get( status );
}
