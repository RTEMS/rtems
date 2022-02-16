/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Function Releases a lock held on RWLock object referenced by rwlock 
 */

/*
 *  POSIX RWLock Manager -- Release a lock held on a RWLock Instance
 *
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
#include <rtems/posix/posixapi.h>

#include <string.h>

bool _POSIX_RWLock_Auto_initialization( POSIX_RWLock_Control *the_rwlock )
{
  POSIX_RWLock_Control zero;

  memset( &zero, 0, sizeof( zero ) );

  if ( memcmp( the_rwlock, &zero, sizeof( *the_rwlock ) ) != 0 ) {
    return false;
  }

  the_rwlock->flags = (uintptr_t) the_rwlock ^ POSIX_RWLOCK_MAGIC;
  return true;
}

int pthread_rwlock_unlock(
  pthread_rwlock_t  *rwlock
)
{
  POSIX_RWLock_Control *the_rwlock;
  Status_Control        status;

  the_rwlock = _POSIX_RWLock_Get( rwlock );
  POSIX_RWLOCK_VALIDATE_OBJECT( the_rwlock );

  status = _CORE_RWLock_Surrender( &the_rwlock->RWLock );
  return _POSIX_Get_error( status );
}
