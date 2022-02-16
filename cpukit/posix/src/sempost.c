/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief Unlock a Semaphore
 *  @ingroup POSIX_SEMAPHORE
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

#include <rtems/posix/semaphoreimpl.h>

#include <limits.h>

int sem_post( sem_t *_sem )
{
  Sem_Control          *sem;
  ISR_Level             level;
  Thread_queue_Context  queue_context;
  Thread_queue_Heads   *heads;
  unsigned int          count;

  POSIX_SEMAPHORE_VALIDATE_OBJECT( _sem );

  sem = _Sem_Get( &_sem->_Semaphore );
  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_ISR_disable( &queue_context, level );
  _Sem_Queue_acquire_critical( sem, &queue_context );

  heads = sem->Queue.Queue.heads;
  count = sem->count;

  if ( RTEMS_PREDICT_TRUE( heads == NULL && count < SEM_VALUE_MAX ) ) {
    sem->count = count + 1;
    _Sem_Queue_release( sem, level, &queue_context );
    return 0;
  }

  if ( RTEMS_PREDICT_TRUE( heads != NULL ) ) {
    _Thread_queue_Context_set_ISR_level( &queue_context, level );
    _Thread_queue_Surrender_no_priority(
      &sem->Queue.Queue,
      heads,
      &queue_context,
      SEMAPHORE_TQ_OPERATIONS
    );
    return 0;
  }

  _Sem_Queue_release( sem, level, &queue_context );
  rtems_set_errno_and_return_minus_one( EOVERFLOW );
}
