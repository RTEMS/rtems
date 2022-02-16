/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicSemaphore
 *
 * @brief This source file contains the implementation of
 *   rtems_semaphore_release().
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

rtems_status_code rtems_semaphore_release( rtems_id id )
{
  Semaphore_Control    *the_semaphore;
  Thread_queue_Context  queue_context;
  Thread_Control       *executing;
  uintptr_t             flags;
  Semaphore_Variant     variant;
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
  flags = _Semaphore_Get_flags( the_semaphore );
  variant = _Semaphore_Get_variant( flags );

  switch ( variant ) {
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
        _Semaphore_Get_operations( flags ),
        executing,
        &queue_context
      );
      break;
    case SEMAPHORE_VARIANT_SIMPLE_BINARY:
      status = _CORE_semaphore_Surrender(
        &the_semaphore->Core_control.Semaphore,
        _Semaphore_Get_operations( flags ),
        1,
        &queue_context
      );
      _Assert(
        status == STATUS_SUCCESSFUL
          || status == STATUS_MAXIMUM_COUNT_EXCEEDED
      );
      (void) status;
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
      _Assert( variant == SEMAPHORE_VARIANT_COUNTING );
      status = _CORE_semaphore_Surrender(
        &the_semaphore->Core_control.Semaphore,
        _Semaphore_Get_operations( flags ),
        UINT32_MAX,
        &queue_context
      );
      break;
  }

  return _Status_Get( status );
}
