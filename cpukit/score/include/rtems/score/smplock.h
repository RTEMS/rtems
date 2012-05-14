/**
 *  @file  rtems/score/smplock.h
 *
 *  This include file defines the interface for atomic locks
 *  which can be used in multiprocessor configurations.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_LOCK_H
#define _RTEMS_LOCK_H

#include <rtems/score/isr.h>

/**
 *  @defgroup RTEMS Lock Interface
 *
 *  @ingroup Score
 *
 */

/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  This type is used to lock elements for atomic access.
 *  This spinlock is a simple non-nesting spinlock, and
 *  may be used for short non-nesting accesses.
 */
typedef uint32_t SMP_lock_spinlock_simple_Control;

/**
 *  This type is used to lock elements for atomic access.
 *  This spinlock supports nesting, but is slightly more
 *  complicated to use.  Please see the descriptions of
 *  obtain and release prior to using in order to understand
 *  the callers responsibilty of managing short interupt disable
 *  times.
 */
typedef struct {
  SMP_lock_spinlock_simple_Control lock;
  uint32_t  count;
  int       cpu_id;
} SMP_lock_spinlock_nested_Control;

/**
 *  @brief Initialize a Lock
 *
 *  This method is used to initialize the lock at @a lock.
 *
 *  @param [in] lock is the address of the lock to obtain.
 */
void _SMP_lock_spinlock_simple_Initialize(
  SMP_lock_spinlock_simple_Control *lock
);

/**
 *  @brief Obtain a Lock
 *
 *  This method is used to obtain the lock at @a lock.
 *
 *  @param [in] lock is the address of the lock to obtain.
 *
 *  @return This method returns with processor interrupts disabled.
 *          The previous level is returned.
 */
ISR_Level _SMP_lock_spinlock_simple_Obtain(
  SMP_lock_spinlock_simple_Control *lock
);

/**
 *  @brief Release a Lock
 *
 *  This method is used to release the lock at @a lock.
 *
 *  @param [in] lock is the address of the lock to obtain.
 */
void _SMP_lock_spinlock_simple_Release(
  SMP_lock_spinlock_simple_Control  *lock,
  ISR_Level                         level
);

/**
 *  @brief Initialize a Lock
 *
 *  This method is used to initialize the lock at @a lock.
 *
 *  @param [in] lock is the address of the lock to obtain.
 */
void _SMP_lock_spinlock_nested_Initialize(
  SMP_lock_spinlock_nested_Control *lock
);

/**
 *  @brief Obtain a Lock
 *
 *  This method is used to obtain the lock at @a lock.  ISR's are
 *  disabled when this routine returns and it is the callers responsibility
 *  to either:
 *
 *   # Do something very short and then call
 *      _SMP_lock_spinlock_nested_Release  or
 *   # Do something very sort, call isr enable, then when ready
 *      call isr_disable and _SMP_lock_spinlock_nested_Release
 *
 *  @param [in] lock is the address of the lock to obtain.
 *
 *  @return This method returns with processor interrupts disabled.
 *          The previous level is returned.
 */
ISR_Level _SMP_lock_spinlock_nested_Obtain(
  SMP_lock_spinlock_nested_Control *lock
);

/**
 *  @brief Release a Lock
 *
 *  This method is used to release the lock at @a lock.  
 *
 *  @note ISR's are reenabled by this method and are expected to be
 *  disabled upon entry to the method.
 *
 *  @param [in] lock is the address of the lock to obtain.
 */
void _SMP_lock_spinlock_nested_Release(
  SMP_lock_spinlock_nested_Control  *lock,
  ISR_Level                         level
);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
