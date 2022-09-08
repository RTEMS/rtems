/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * 
 * @brief Inlined Routines from the POSIX RWLock Manager
 *
 * This file contains the static inlin implementation of the inlined
 * routines from the POSIX RWLock Manager.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
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

#ifndef _RTEMS_POSIX_RWLOCKIMPL_H
#define _RTEMS_POSIX_RWLOCKIMPL_H

#include <rtems/score/corerwlockimpl.h>

#include <errno.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define POSIX_RWLOCK_MAGIC 0x9621dabdUL

typedef struct {
  unsigned long flags;
  CORE_RWLock_Control RWLock;
} POSIX_RWLock_Control;

static inline POSIX_RWLock_Control *_POSIX_RWLock_Get(
  pthread_rwlock_t *rwlock
)
{
  return (POSIX_RWLock_Control *) rwlock;
}

bool _POSIX_RWLock_Auto_initialization( POSIX_RWLock_Control *the_rwlock );

#define POSIX_RWLOCK_VALIDATE_OBJECT( rw ) \
  do { \
    if ( ( rw ) == NULL ) { \
      return EINVAL; \
    } \
    if ( ( (uintptr_t) ( rw ) ^ POSIX_RWLOCK_MAGIC ) != ( rw )->flags ) { \
      if ( !_POSIX_RWLock_Auto_initialization( rw ) ) { \
        return EINVAL; \
      } \
    } \
  } while ( 0 )

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
