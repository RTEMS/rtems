/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreThread
 *
 * @brief This source file contains the implementation of
 *   _Thread_Continue() and _Thread_Timeout().
 */

/*
 *  COPYRIGHT (c) 1989-2008.
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

#include <rtems/score/threadimpl.h>
#include <rtems/score/status.h>

void _Thread_Continue( Thread_Control *the_thread, Status_Control status )
{
  Thread_queue_Context queue_context;
  Thread_Wait_flags    wait_flags;
  bool                 unblock;

  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_clear_priority_updates( &queue_context );
  _Thread_Wait_acquire( the_thread, &queue_context );

  wait_flags = _Thread_Wait_flags_get( the_thread );

  if ( wait_flags != THREAD_WAIT_STATE_READY ) {
    Thread_Wait_flags wait_class;
    bool              success;

    _Thread_Wait_cancel( the_thread, &queue_context );

    the_thread->Wait.return_code = status;

    wait_class = wait_flags & THREAD_WAIT_CLASS_MASK;
    success = _Thread_Wait_flags_try_change_release(
      the_thread,
      wait_class | THREAD_WAIT_STATE_INTEND_TO_BLOCK,
      THREAD_WAIT_STATE_READY
    );

    if ( success ) {
      unblock = false;
    } else {
      _Assert(
        _Thread_Wait_flags_get( the_thread )
          == ( wait_class | THREAD_WAIT_STATE_BLOCKED )
      );
      _Thread_Wait_flags_set( the_thread, THREAD_WAIT_STATE_READY );
      unblock = true;
    }
  } else {
    unblock = false;
  }

  _Thread_Wait_release( the_thread, &queue_context );
  _Thread_Priority_update( &queue_context );

  if ( unblock ) {
    _Thread_Wait_tranquilize( the_thread );
    _Thread_Unblock( the_thread );

#if defined(RTEMS_MULTIPROCESSING)
    if ( !_Objects_Is_local_id( the_thread->Object.id ) ) {
      _Thread_MP_Free_proxy( the_thread );
    }
#endif
  }
}

void _Thread_Timeout( Watchdog_Control *the_watchdog )
{
  Thread_Control *the_thread;

  the_thread = RTEMS_CONTAINER_OF(
    the_watchdog,
    Thread_Control,
    Timer.Watchdog
  );
  _Thread_Continue( the_thread, STATUS_TIMEOUT );
}
