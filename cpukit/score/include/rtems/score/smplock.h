/**
 * @file
 *
 * @ingroup ScoreSMPLock
 *
 * @brief SMP Lock API
 */

/*
 * COPYRIGHT (c) 1989-2011.
 * On-Line Applications Research Corporation (OAR).
 *
 * Copyright (c) 2013-2014 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SMPLOCK_H
#define _RTEMS_SCORE_SMPLOCK_H

#include <rtems/score/cpuopts.h>

#if defined( RTEMS_SMP )

#include <rtems/score/atomic.h>
#include <rtems/score/isrlevel.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup ScoreSMPLock SMP Locks
 *
 * @ingroup Score
 *
 * @brief The SMP lock provides mutual exclusion for SMP systems at the lowest
 * level.
 *
 * The SMP lock is implemented as a ticket lock.  This provides fairness in
 * case of concurrent lock attempts.
 *
 * This SMP lock API has a flaw.  It does not provide the ability to use a
 * local context for acquire and release pairs.  Such a context is necessary to
 * implement for example the Mellor-Crummey and Scott (MCS) locks.  The SMP
 * lock is currently used in _Thread_Disable_dispatch() and
 * _Thread_Enable_dispatch() and makes them to a giant lock acquire and
 * release.  Since these functions do not pass state information via a local
 * context there is currently no use case for such a feature.
 *
 * @{
 */

/**
 * @brief SMP lock control.
 */
typedef struct {
  Atomic_Uint next_ticket;
  Atomic_Uint now_serving;
} SMP_lock_Control;

/**
 * @brief SMP lock control initializer for static initialization.
 */
#define SMP_LOCK_INITIALIZER \
  { ATOMIC_INITIALIZER_UINT( 0U ), ATOMIC_INITIALIZER_UINT( 0U ) }

/**
 * @brief Initializes a SMP lock control.
 *
 * Concurrent initialization leads to unpredictable results.
 *
 * @param[out] lock The SMP lock control.
 */
static inline void _SMP_lock_Initialize( SMP_lock_Control *lock )
{
  _Atomic_Init_uint( &lock->next_ticket, 0U );
  _Atomic_Init_uint( &lock->now_serving, 0U );
}

/**
 * @brief Acquires a SMP lock.
 *
 * This function will not disable interrupts.  The caller must ensure that the
 * current thread of execution is not interrupted indefinite once it obtained
 * the SMP lock.
 *
 * @param[in,out] lock The SMP lock control.
 */
static inline void _SMP_lock_Acquire( SMP_lock_Control *lock )
{
  unsigned int my_ticket =
    _Atomic_Fetch_add_uint( &lock->next_ticket, 1U, ATOMIC_ORDER_RELAXED );
  unsigned int now_serving;

  do {
    now_serving =
      _Atomic_Load_uint( &lock->now_serving, ATOMIC_ORDER_ACQUIRE );
  } while ( now_serving != my_ticket );
}

/**
 * @brief Releases a SMP lock.
 *
 * @param[in,out] lock The SMP lock control.
 */
static inline void _SMP_lock_Release( SMP_lock_Control *lock )
{
  unsigned int current_ticket =
    _Atomic_Load_uint( &lock->now_serving, ATOMIC_ORDER_RELAXED );
  unsigned int next_ticket = current_ticket + 1U;

  _Atomic_Store_uint( &lock->now_serving, next_ticket, ATOMIC_ORDER_RELEASE );
}

/**
 * @brief Disables interrupts and acquires the SMP lock.
 *
 * @param[in,out] lock The SMP lock control.
 * @param[out] isr_cookie The ISR cookie.
 */
#define _SMP_lock_ISR_disable_and_acquire( lock, isr_cookie ) \
  do { \
    _ISR_Disable_without_giant( isr_cookie ); \
    _SMP_lock_Acquire( lock ); \
  } while (0)

/**
 * @brief Releases the SMP lock and enables interrupts.
 *
 * @param[in,out] lock The SMP lock control.
 * @param[in] isr_cookie The ISR cookie.
 */
#define _SMP_lock_Release_and_ISR_enable( lock, isr_cookie ) \
  do { \
    _SMP_lock_Release( lock ); \
    _ISR_Enable_without_giant( isr_cookie ); \
  } while (0)

/**@}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* defined( RTEMS_SMP ) */

#endif /* _RTEMS_SCORE_SMPLOCK_H */
