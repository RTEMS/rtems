/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreRWLock
 *
 * @brief This source file contains the implementation of
 *   _CORE_RWLock_Seize_for_reading().
 */

/*
 *  COPYRIGHT (c) 1989-2006.
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

#include <rtems/score/corerwlockimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/watchdog.h>

Status_Control _CORE_RWLock_Seize_for_reading(
  CORE_RWLock_Control  *the_rwlock,
  bool                  wait,
  Thread_queue_Context *queue_context
)
{
  Thread_Control *executing;

  /*
   *  If unlocked, then OK to read.
   *  If locked for reading and no waiters, then OK to read.
   *  If any thread is waiting, then we wait.
   */

  executing = _CORE_RWLock_Acquire( the_rwlock, queue_context );

  switch ( the_rwlock->current_state ) {
    case CORE_RWLOCK_UNLOCKED:
      the_rwlock->current_state = CORE_RWLOCK_LOCKED_FOR_READING;
      the_rwlock->number_of_readers += 1;
      _CORE_RWLock_Release( the_rwlock, queue_context );
      return STATUS_SUCCESSFUL;

    case CORE_RWLOCK_LOCKED_FOR_READING:
      if ( _Thread_queue_Is_empty( &the_rwlock->Queue.Queue ) ) {
        the_rwlock->number_of_readers += 1;
        _CORE_RWLock_Release( the_rwlock, queue_context );
        return STATUS_SUCCESSFUL;
      }
      break;
    case CORE_RWLOCK_LOCKED_FOR_WRITING:
      break;
  }

  /*
   *  If the thread is not willing to wait, then return immediately.
   */

  if ( !wait ) {
    _CORE_RWLock_Release( the_rwlock, queue_context );
    return STATUS_UNAVAILABLE;
  }

  /*
   *  We need to wait to enter this critical section
   */

  executing->Wait.option = CORE_RWLOCK_THREAD_WAITING_FOR_READ;

  _Thread_queue_Context_set_thread_state(
    queue_context,
   STATES_WAITING_FOR_RWLOCK
  );
  _Thread_queue_Enqueue(
     &the_rwlock->Queue.Queue,
     CORE_RWLOCK_TQ_OPERATIONS,
     executing,
     queue_context
  );
  return _Thread_Wait_get_status( executing );
}
