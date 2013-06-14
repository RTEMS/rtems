/**
 * @file
 *
 * @ingroup ScoreSMPLockCPU
 *
 * @brief CPU SMP Lock Implementation
 */

/*
 * Copyright (c) 2013 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_NO_CPU_SMPLOCK_H
#define _RTEMS_SCORE_NO_CPU_SMPLOCK_H

#include <rtems/score/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup ScoreSMPLockCPU CPU SMP Locks
 *
 * @ingroup ScoreSMPLock
 *
 * This example will implement a ticket lock.
 *
 * @{
 */

/**
 * @brief CPU SMP lock control.
 */
typedef struct {
  unsigned int next_ticket;
  unsigned int now_serving;
} CPU_SMP_lock_Control;

/**
 * @brief CPU SMP lock control initializer for static initialization.
 */
#define CPU_SMP_LOCK_INITIALIZER { 0, 0 }

/**
 * @brief Initializes a CPU SMP lock control.
 *
 * @param[out] lock The CPU SMP lock control.
 */
static inline void _CPU_SMP_lock_Initialize( CPU_SMP_lock_Control *lock )
{
  lock->next_ticket = 0;
  lock->now_serving = 0;
}

/**
 * @brief Acquires a CPU SMP lock.
 *
 * @param[in,out] lock The CPU SMP lock control.
 */
static inline void _CPU_SMP_lock_Acquire( CPU_SMP_lock_Control *lock )
{
  unsigned int my_ticket = _Atomic_Fetch_and_increment( &lock->next_ticket );

  while ( _Atomic_Load_and_acquire( &lock->now_serving ) != my_ticket ) {
    _Wait_some_time();
  }
}

/**
 * @brief Releases a CPU SMP lock.
 *
 * @param[in,out] lock The CPU SMP lock control.
 */
static inline void _CPU_SMP_lock_Release( CPU_SMP_lock_Control *lock )
{
  _Atomic_Store_and_release( &lock->now_serving, lock->now_serving + 1 );
}

/**
 * @brief Disables interrupts and acquires the CPU SMP lock.
 *
 * @param[in,out] lock The CPU SMP lock control.
 * @param[out] isr_cookie The ISR cookie.
 */
#define _CPU_SMP_lock_ISR_disable_and_acquire( lock, isr_cookie ) \
  do { \
    _CPU_ISR_Disable( isr_cookie ); \
    _CPU_SMP_lock_Acquire( lock ); \
  } while (0)

/**
 * @brief Releases the CPU SMP lock and enables interrupts.
 *
 * @param[in,out] lock The CPU SMP lock control.
 * @param[in] isr_cookie The ISR cookie.
 */
#define _CPU_SMP_lock_Release_and_ISR_enable( lock, isr_cookie ) \
  do { \
    _CPU_SMP_lock_Release( lock ); \
    _CPU_ISR_Enable( isr_cookie ); \
  } while (0)

/**@}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_NO_CPU_SMPLOCK_H */
