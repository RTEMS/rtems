/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief Destroy a RWLock
 *  @ingroup POSIXAPI
 */
/*
 *  COPYRIGHT (c) 1989-2007.
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

#include <rtems/posix/rwlockimpl.h>

int pthread_rwlock_destroy(
  pthread_rwlock_t *_rwlock
)
{
  POSIX_RWLock_Control *the_rwlock;
  Thread_queue_Context  queue_context;

  the_rwlock = _POSIX_RWLock_Get( _rwlock );
  POSIX_RWLOCK_VALIDATE_OBJECT( the_rwlock );

  _CORE_RWLock_Acquire( &the_rwlock->RWLock, &queue_context );

  /*
   *  If there is at least one thread waiting, then do not delete it.
   */

  if ( !_Thread_queue_Is_empty( &the_rwlock->RWLock.Queue.Queue ) ) {
    _CORE_RWLock_Release( &the_rwlock->RWLock, &queue_context );
    return EBUSY;
  }

  /*
   *  POSIX doesn't require behavior when it is locked.
   */

  the_rwlock->flags = ~the_rwlock->flags;
  _CORE_RWLock_Release( &the_rwlock->RWLock, &queue_context );
  return 0;
}
