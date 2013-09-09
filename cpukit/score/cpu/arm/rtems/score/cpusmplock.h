/**
 * @file
 *
 * @ingroup ScoreSMPLockARM
 *
 * @brief ARM SMP Lock Implementation
 */

/*
 * Copyright (c) 2013 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_ARM_SMPLOCK_H
#define _RTEMS_SCORE_ARM_SMPLOCK_H

#include <rtems/score/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup ScoreSMPLockARM ARM SMP Locks
 *
 * @ingroup ScoreSMPLock
 *
 * A ticket lock implementation is used.
 *
 * @{
 */

typedef struct {
  uint32_t next_ticket;
  uint32_t now_serving;
} CPU_SMP_lock_Control;

#define CPU_SMP_LOCK_INITIALIZER { 0, 0 }

static inline void _CPU_SMP_lock_Initialize( CPU_SMP_lock_Control *lock )
{
  lock->next_ticket = 0;
  lock->now_serving = 0;
}

static inline void _CPU_SMP_lock_Acquire( CPU_SMP_lock_Control *lock )
{
  uint32_t my_ticket;
  uint32_t next_ticket;
  uint32_t status;

  __asm__ volatile (
    "1: ldrex %[my_ticket], [%[next_ticket_addr]]\n"
    "add %[next_ticket], %[my_ticket], #1\n"
    "strex %[status], %[next_ticket], [%[next_ticket_addr]]\n"
    "teq %[status], #0\n"
    "bne 1b"
    : [my_ticket] "=&r" (my_ticket),
      [next_ticket] "=&r" (next_ticket),
      [status] "=&r" (status)
    : [next_ticket_addr] "r" (&lock->next_ticket)
    : "cc", "memory"
  );

  while ( my_ticket != lock->now_serving ) {
    _ARM_Wait_for_event();
  }

  _ARM_Data_memory_barrier();
}

static inline void _CPU_SMP_lock_Release( CPU_SMP_lock_Control *lock )
{
  _ARM_Data_memory_barrier();
  ++lock->now_serving;
  _ARM_Data_synchronization_barrier();
  _ARM_Send_event();
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

#endif /* _RTEMS_SCORE_ARM_SMPLOCK_H */
