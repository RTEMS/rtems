/**
 * @file
 *
 * @ingroup ScoreSMPLockI386
 *
 * @brief i386 SMP Lock Implementation
 */

/*
 * COPYRIGHT (c) 1989-2011.
 * On-Line Applications Research Corporation (OAR).
 *
 * Copyright (c) 2013 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_I386_SMPLOCK_H
#define _RTEMS_SCORE_I386_SMPLOCK_H

#include <rtems/score/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup ScoreSMPLockI386 i386 SMP Locks
 *
 * @ingroup ScoreSMPLock
 *
 * The implementation is Test and Swap.
 *
 * @{
 */

typedef struct {
  uint32_t locked;
} CPU_SMP_lock_Control;

#define CPU_SMP_LOCK_INITIALIZER { 0 }

static inline void _CPU_SMP_lock_Initialize( CPU_SMP_lock_Control *lock )
{
  lock->locked = 0;
}

static inline uint32_t _I386_Atomic_swap(
  volatile uint32_t *address,
  uint32_t value
)
{
  uint32_t previous;

  asm volatile(
    "lock; xchgl %0, %1"
    : "+m" (*address), "=a" (previous)
    : "1" (value)
    : "cc"
  );

  return previous;
}

static inline void _CPU_SMP_lock_Acquire( CPU_SMP_lock_Control *lock )
{
  do {
    while ( lock->locked ) {
      RTEMS_COMPILER_MEMORY_BARRIER();
    }
  } while ( _I386_Atomic_swap( &lock->locked, 1 ) );
}

static inline void _CPU_SMP_lock_Release( CPU_SMP_lock_Control *lock )
{
  RTEMS_COMPILER_MEMORY_BARRIER();
  lock->locked = 0;
}

#define _CPU_SMP_lock_ISR_disable_and_acquire( lock, isr_cookie ) \
  do { \
    _CPU_ISR_Disable( isr_cookie ); \
    _CPU_SMP_lock_Acquire( lock ); \
  } while (0)

#define _CPU_SMP_lock_Release_and_ISR_enable( lock, isr_cookie ) \
  do { \
    _CPU_SMP_lock_Release( lock ); \
    _CPU_ISR_Enable( isr_cookie ); \
  } while (0)

/**@}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_I386_SMPLOCK_H */
