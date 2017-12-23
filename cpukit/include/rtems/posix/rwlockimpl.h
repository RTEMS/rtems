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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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

RTEMS_INLINE_ROUTINE POSIX_RWLock_Control *_POSIX_RWLock_Get(
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
