/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Call to Function Enables Reinitializing of the Barrier
 */

/*
 *  POSIX Barrier Manager -- Initialize a Barrier Instance
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2017 embedded brains GmbH & Co. KG
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

#include <rtems/posix/barrierimpl.h>
#include <rtems/posix/posixapi.h>

RTEMS_STATIC_ASSERT(
  offsetof( POSIX_Barrier_Control, flags )
    == offsetof( pthread_barrier_t, _flags ),
  POSIX_BARRIER_CONTROL_FLAGS
);

RTEMS_STATIC_ASSERT(
  offsetof( POSIX_Barrier_Control, count )
    == offsetof( pthread_barrier_t, _count ),
  POSIX_BARRIER_CONTROL_COUNT
);

RTEMS_STATIC_ASSERT(
  offsetof( POSIX_Barrier_Control, waiting_threads )
    == offsetof( pthread_barrier_t, _waiting_threads ),
  POSIX_BARRIER_CONTROL_WAITING_THREADS
);

RTEMS_STATIC_ASSERT(
  offsetof( POSIX_Barrier_Control, Queue )
    == offsetof( pthread_barrier_t, _Queue ),
  POSIX_BARRIER_CONTROL_QUEUE
);

RTEMS_STATIC_ASSERT(
  sizeof( POSIX_Barrier_Control ) == sizeof( pthread_barrier_t ),
  POSIX_BARRIER_CONTROL_SIZE
);

int pthread_barrier_init(
  pthread_barrier_t           *_barrier,
  const pthread_barrierattr_t *attr,
  unsigned int                 count
)
{
  POSIX_Barrier_Control *barrier;

  barrier = _POSIX_Barrier_Get( _barrier );

  /*
   *  Error check parameters
   */
  if ( barrier == NULL ) {
    return EINVAL;
  }

  if ( count == 0 ) {
    return EINVAL;
  }

  if ( attr != NULL ) {
    /*
     * Now start error checking the attributes that we are going to use
     */
    if ( !attr->is_initialized )
      return EINVAL;

    if ( !_POSIX_Is_valid_pshared( attr->process_shared ) ) {
      return EINVAL;
    }
  }

  barrier->flags = (uintptr_t) barrier ^ POSIX_BARRIER_MAGIC;
  barrier->count = count;
  barrier->waiting_threads = 0;
  _Thread_queue_Queue_initialize( &barrier->Queue.Queue, NULL );
  return 0;
}
