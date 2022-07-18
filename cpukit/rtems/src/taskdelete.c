/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicTask
 *
 * @brief This source file contains the implementation of
 *   rtems_task_delete().
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

#include <rtems/rtems/tasksimpl.h>
#include <rtems/rtems/statusimpl.h>
#include <rtems/score/threadimpl.h>

rtems_status_code rtems_task_delete(
  rtems_id id
)
{
  Thread_Control       *the_thread;
  Thread_queue_Context  queue_context;
  Per_CPU_Control      *cpu_self;
  Thread_Control       *executing;
  Status_Control        status;

  _Thread_queue_Context_initialize( &queue_context );
  the_thread = _Thread_Get( id, &queue_context.Lock_context.Lock_context );

  if ( the_thread == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    if ( _Thread_MP_Is_remote( id ) ) {
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
    }
#endif

    return RTEMS_INVALID_ID;
  }

  cpu_self = _Per_CPU_Get();

  if ( _Per_CPU_Is_ISR_in_progress( cpu_self ) ) {
    _ISR_lock_ISR_enable( &queue_context.Lock_context.Lock_context );
    return RTEMS_CALLED_FROM_ISR;
  }

  executing = _Per_CPU_Get_executing( cpu_self );

  if ( the_thread == executing ) {
    _ISR_lock_ISR_enable( &queue_context.Lock_context.Lock_context );

    /*
     * The Classic tasks are neither detached nor joinable.  In case of
     * self deletion, they are detached, otherwise joinable by default.
     */
    _Thread_Exit( NULL, THREAD_LIFE_TERMINATING | THREAD_LIFE_DETACHED );
  }

  status = _Thread_Close( the_thread, executing, &queue_context );
  return _Status_Get( status );
}
