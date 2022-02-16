/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Allocate resources to use the read-write lock and Initialize it
 */

/*
 *  POSIX RWLock Manager -- Destroy a RWLock Instance
 *
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

#include <rtems/posix/rwlockimpl.h>
#include <rtems/posix/posixapi.h>

RTEMS_STATIC_ASSERT(
  offsetof( POSIX_RWLock_Control, flags )
    == offsetof( pthread_rwlock_t, _flags ),
  POSIX_RWLOCK_CONTROL_FLAGS
);

RTEMS_STATIC_ASSERT(
  offsetof( POSIX_RWLock_Control, RWLock.current_state )
    == offsetof( pthread_rwlock_t, _current_state ),
  POSIX_RWLOCK_CONTROL_CURRENT_STATE
);

RTEMS_STATIC_ASSERT(
  offsetof( POSIX_RWLock_Control, RWLock.number_of_readers )
    == offsetof( pthread_rwlock_t, _number_of_readers ),
  POSIX_RWLOCK_CONTROL_NUMBER_OF_READERS
);

RTEMS_STATIC_ASSERT(
  offsetof( POSIX_RWLock_Control, RWLock.Queue )
    == offsetof( pthread_rwlock_t, _Queue ),
  POSIX_RWLOCK_CONTROL_QUEUE
);

RTEMS_STATIC_ASSERT(
  sizeof( POSIX_RWLock_Control ) == sizeof( pthread_rwlock_t ),
  POSIX_RWLOCK_CONTROL_SIZE
);

int pthread_rwlock_init(
  pthread_rwlock_t           *rwlock,
  const pthread_rwlockattr_t *attr
)
{
  POSIX_RWLock_Control *the_rwlock;

  the_rwlock = _POSIX_RWLock_Get( rwlock );

  if ( the_rwlock == NULL ) {
    return EINVAL;
  }

  if ( attr != NULL ) {
    if ( !attr->is_initialized ) {
      return EINVAL;
    }

    if ( !_POSIX_Is_valid_pshared( attr->process_shared ) ) {
      return EINVAL;
    }
  }

  the_rwlock->flags = (uintptr_t) the_rwlock ^ POSIX_RWLOCK_MAGIC;
  _CORE_RWLock_Initialize( &the_rwlock->RWLock );
  return 0;
}
