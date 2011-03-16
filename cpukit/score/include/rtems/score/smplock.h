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
 *
 *  $Id$
 */

#ifndef _RTEMS_LOCK_H
#define _RTEMS_LOCK_H

#include <rtems/score/isr.h>

/**
 *  @defgroup RTEMS Lock Interface
 *
 */

/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  This type is used to lock elements for atomic access.
 *
 *  @note This type may move to RTEMS.
 */
typedef volatile uint32_t SMP_lock_Control;

/**
 *  @brief Initialize a Lock
 *
 *  This method is used to initialize the lock at @a lock.
 *
 *  @param [in] lock is the address of the lock to obtain.
 *
 *  @note This lock may be "too low" here.  It may need to move
 *         out of the BSP area.
 */
void _SMP_lock_Spinlock_Initialize(
  SMP_lock_Control *lock
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
 *
 *  @note This lock may be "too low" here.  It may need to move
 *         out of the BSP area.
 */
ISR_Level _SMP_lock_Spinlock_Obtain(
  SMP_lock_Control *lock
);

/**
 *  @brief Release a Lock
 *
 *  This method is used to release the lock at @a lock.
 *
 *  @param [in] lock is the address of the lock to obtain.
 *
 *  @note This lock may be "too low" here.  It may need to move
 *         out of the BSP area.
 */
void _SMP_lock_Spinlock_Release(
  SMP_lock_Control  *lock,
  ISR_Level         level
);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
