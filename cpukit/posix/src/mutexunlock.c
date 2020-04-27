/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Locking and Unlocking a Mutex
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/muteximpl.h>
#include <rtems/posix/posixapi.h>

#include <string.h>

bool _POSIX_Mutex_Auto_initialization( POSIX_Mutex_Control *the_mutex )
{
  unsigned long zero;
  unsigned long flags;

  /* We cannot use memset() and memcmp() due to structure internal padding */
  zero = 0;
  zero |= the_mutex->flags;
#if defined(RTEMS_SMP)
  zero |= _Atomic_Load_uint(
    &the_mutex->Recursive.Mutex.Queue.Queue.Lock.next_ticket,
    ATOMIC_ORDER_RELAXED
  );
  zero |= _Atomic_Load_uint(
    &the_mutex->Recursive.Mutex.Queue.Queue.Lock.now_serving,
    ATOMIC_ORDER_RELAXED
  );
#else
  zero |= the_mutex->Recursive.Mutex.Queue.reserved[ 0 ];
  zero |= the_mutex->Recursive.Mutex.Queue.reserved[ 1 ];
#endif
  zero |= (unsigned long) the_mutex->Recursive.Mutex.Queue.Queue.heads;
  zero |= (unsigned long) the_mutex->Recursive.Mutex.Queue.Queue.owner;
  zero |= (unsigned long) the_mutex->Recursive.Mutex.Queue.Queue.name;
  zero |= the_mutex->Recursive.nest_level;
  zero |= (unsigned long) the_mutex->Priority_ceiling.Node.RBTree.Node.rbe_left;
  zero |= (unsigned long) the_mutex->Priority_ceiling.Node.RBTree.Node.rbe_right;
  zero |= (unsigned long) the_mutex->Priority_ceiling.Node.RBTree.Node.rbe_parent;
  zero |= (unsigned long) the_mutex->Priority_ceiling.Node.RBTree.Node.rbe_color;
  zero |= (unsigned long) the_mutex->Priority_ceiling.priority;
  zero |= (unsigned long) (the_mutex->Priority_ceiling.priority >> 32);
  zero |= (unsigned long) the_mutex->scheduler;

  if ( zero != 0 ) {
    return false;
  }

  flags = (uintptr_t) the_mutex ^ POSIX_MUTEX_MAGIC;
  flags &= ~POSIX_MUTEX_FLAGS_MASK;
  the_mutex->flags = flags;
  return true;
}

/*
 *  11.3.3 Locking and Unlocking a Mutex, P1003.1c/Draft 10, p. 93
 *
 *  NOTE: P1003.4b/D8 adds pthread_mutex_timedlock(), p. 29
 */

int pthread_mutex_unlock(
  pthread_mutex_t           *mutex
)
{
  POSIX_Mutex_Control  *the_mutex;
  unsigned long         flags;
  Thread_queue_Context  queue_context;
  Thread_Control       *executing;
  Status_Control        status;

  the_mutex = _POSIX_Mutex_Get( mutex );
  POSIX_MUTEX_VALIDATE_OBJECT( the_mutex, flags );

  executing = _POSIX_Mutex_Acquire( the_mutex, &queue_context );

  switch ( _POSIX_Mutex_Get_protocol( flags ) ) {
    case POSIX_MUTEX_PRIORITY_CEILING:
      status = _POSIX_Mutex_Ceiling_surrender(
        the_mutex,
        executing,
        &queue_context
      );
      break;
    case POSIX_MUTEX_NO_PROTOCOL:
      status = _POSIX_Mutex_Surrender(
        the_mutex,
        POSIX_MUTEX_NO_PROTOCOL_TQ_OPERATIONS,
        executing,
        &queue_context
      );
      break;
    default:
      _Assert(
        _POSIX_Mutex_Get_protocol( flags ) == POSIX_MUTEX_PRIORITY_INHERIT
      );
      status = _POSIX_Mutex_Surrender(
        the_mutex,
        POSIX_MUTEX_PRIORITY_INHERIT_TQ_OPERATIONS,
        executing,
        &queue_context
      );
      break;
  }

  return _POSIX_Get_error( status );
}
