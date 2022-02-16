/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Changes the Priority Ceiling of a Mutex and Releases it
 */

/*
 *  COPYRIGHT (c) 1989-2009.
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

#include <rtems/posix/muteximpl.h>
#include <rtems/posix/priorityimpl.h>

/*
 *  13.6.2 Change the Priority Ceiling of a Mutex, P1003.1c/Draft 10, p. 131
 */

int pthread_mutex_setprioceiling(
  pthread_mutex_t   *mutex,
  int                prioceiling,
  int               *old_ceiling
)
{
  POSIX_Mutex_Control *the_mutex;
  int                  error;
  int                  unlock_error;

  if ( old_ceiling == NULL ) {
    return EINVAL;
  }

  /*
   *  Must acquire the mutex before we can change it's ceiling.
   *  POSIX says block until we acquire it.
   */
  error = pthread_mutex_lock( mutex );
  if ( error != 0 ) {
    _Assert( error == EINVAL );
    return EINVAL;
  }

  the_mutex = _POSIX_Mutex_Get( mutex );

  if (
    _POSIX_Mutex_Get_protocol( the_mutex->flags )
      == POSIX_MUTEX_PRIORITY_CEILING
  ) {
    const Scheduler_Control *scheduler;
    bool                     valid;
    Priority_Control         new_priority;
    Priority_Control         old_priority;

    scheduler = _POSIX_Mutex_Get_scheduler( the_mutex );
    old_priority = _POSIX_Mutex_Get_priority( the_mutex );
    *old_ceiling = _POSIX_Priority_From_core( scheduler, old_priority );

    new_priority = _POSIX_Priority_To_core( scheduler, prioceiling, &valid );
    if ( valid ) {
      Thread_queue_Context  queue_context;
      Per_CPU_Control      *cpu_self;

      _Thread_queue_Context_initialize( &queue_context );
      _Thread_queue_Context_clear_priority_updates( &queue_context );
      _POSIX_Mutex_Set_priority(
        the_mutex,
        new_priority,
        &queue_context
      );
      cpu_self = _Thread_Dispatch_disable();
      _Thread_Priority_update( &queue_context );
      _Thread_Dispatch_enable( cpu_self );
      error = 0;
    } else {
      error = EINVAL;
    }
  } else {
    *old_ceiling = 0;
    error = 0;
  }

  unlock_error = pthread_mutex_unlock( mutex );
  _Assert( unlock_error == 0 );
  (void) unlock_error;
  return error;
}
