/**
 *  @file
 *
 *  @brief Wait at a Spinlock
 *  @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2016 embedded brains GmbH
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/spinlockimpl.h>

RTEMS_STATIC_ASSERT(
#if defined(RTEMS_SMP)
  offsetof( POSIX_Spinlock_Control, Lock.next_ticket )
#else
  offsetof( POSIX_Spinlock_Control, reserved[ 0 ] )
#endif
    == offsetof( pthread_spinlock_t, _Lock._next_ticket ),
  POSIX_SPINLOCK_T_LOCK_NEXT_TICKET
);

RTEMS_STATIC_ASSERT(
#if defined(RTEMS_SMP)
  offsetof( POSIX_Spinlock_Control, Lock.now_serving )
#else
  offsetof( POSIX_Spinlock_Control, reserved[ 1 ] )
#endif
    == offsetof( pthread_spinlock_t, _Lock._now_serving ),
  POSIX_SPINLOCK_T_LOCK_NOW_SERVING
);

RTEMS_STATIC_ASSERT(
  offsetof( POSIX_Spinlock_Control, interrupt_state )
    == offsetof( pthread_spinlock_t, _interrupt_state ),
  POSIX_SPINLOCK_T_INTERRUPT_STATE
);

RTEMS_STATIC_ASSERT(
  sizeof( POSIX_Spinlock_Control ) == sizeof( pthread_spinlock_t ),
  POSIX_SPINLOCK_T_SIZE
);

int pthread_spin_lock( pthread_spinlock_t *spinlock )
{
  POSIX_Spinlock_Control *the_spinlock;
  ISR_Level               level;
#if defined(RTEMS_SMP) && defined(RTEMS_PROFILING)
  SMP_lock_Stats          unused_stats;
  SMP_lock_Stats_context  unused_context;
#endif

  the_spinlock = _POSIX_Spinlock_Get( spinlock );
  _ISR_Local_disable( level );
#if defined(RTEMS_SMP)
  _SMP_ticket_lock_Acquire(
    &the_spinlock->Lock,
    &unused_stats,
    &unused_context
  );
#endif
  the_spinlock->interrupt_state = level;
  return 0;
}

int pthread_spin_trylock( pthread_spinlock_t *spinlock )
  RTEMS_ALIAS( pthread_spin_lock );
