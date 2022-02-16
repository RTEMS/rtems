/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicSemaphore
 *
 * @brief This source file contains the implementation of
 *   rtems_semaphore_delete().
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/semimpl.h>
#include <rtems/rtems/statusimpl.h>

rtems_status_code rtems_semaphore_delete(
  rtems_id   id
)
{
  Semaphore_Control    *the_semaphore;
  Thread_queue_Context  queue_context;
  uintptr_t             flags;
  Semaphore_Variant     variant;
  Status_Control        status;

  _Objects_Allocator_lock();
  the_semaphore = _Semaphore_Get( id, &queue_context );

  if ( the_semaphore == NULL ) {
    _Objects_Allocator_unlock();

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
  flags = _Semaphore_Get_flags( the_semaphore );
  variant = _Semaphore_Get_variant( flags );

  switch ( variant ) {
    case SEMAPHORE_VARIANT_MUTEX_INHERIT_PRIORITY:
    case SEMAPHORE_VARIANT_MUTEX_PRIORITY_CEILING:
    case SEMAPHORE_VARIANT_MUTEX_NO_PROTOCOL:
      if (
        _CORE_mutex_Is_locked(
          &the_semaphore->Core_control.Mutex.Recursive.Mutex
        )
      ) {
        status = STATUS_RESOURCE_IN_USE;
      } else {
        status = STATUS_SUCCESSFUL;
      }

      break;
#if defined(RTEMS_SMP)
    case SEMAPHORE_VARIANT_MRSP:
      status = _MRSP_Can_destroy( &the_semaphore->Core_control.MRSP );
      break;
#endif
    default:
      _Assert(
        variant == SEMAPHORE_VARIANT_SIMPLE_BINARY
          || variant == SEMAPHORE_VARIANT_COUNTING
      );
      status = STATUS_SUCCESSFUL;
      break;
  }

  if ( status != STATUS_SUCCESSFUL ) {
    _Thread_queue_Release(
      &the_semaphore->Core_control.Wait_queue,
      &queue_context
    );
    _Objects_Allocator_unlock();
    return _Status_Get( status );
  }

  _Objects_Close( &_Semaphore_Information, &the_semaphore->Object );

  switch ( variant ) {
#if defined(RTEMS_SMP)
    case SEMAPHORE_VARIANT_MRSP:
      _MRSP_Destroy( &the_semaphore->Core_control.MRSP, &queue_context );
      break;
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
        _Thread_queue_Flush_status_object_was_deleted,
        &queue_context
      );
      _Thread_queue_Destroy( &the_semaphore->Core_control.Wait_queue );
      break;
  }

#if defined(RTEMS_MULTIPROCESSING)
  if ( _Semaphore_Is_global( flags ) ) {

    _Objects_MP_Close( &_Semaphore_Information, id );

    _Semaphore_MP_Send_process_packet(
      SEMAPHORE_MP_ANNOUNCE_DELETE,
      id,
      0,                         /* Not used */
      0                          /* Not used */
    );
  }
#endif

  _Semaphore_Free( the_semaphore );
  _Objects_Allocator_unlock();
  return RTEMS_SUCCESSFUL;
}
