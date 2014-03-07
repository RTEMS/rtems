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
 * This SMP lock API uses a local context for acquire and release pairs.  Such
 * a context may be used to implement for example the Mellor-Crummey and Scott
 * (MCS) locks in the future.
 *
 * @{
 */

/**
 * @brief SMP ticket lock control.
 */
typedef struct {
  Atomic_Uint next_ticket;
  Atomic_Uint now_serving;
} SMP_ticket_lock_Control;

/**
 * @brief SMP ticket lock control initializer for static initialization.
 */
#define SMP_TICKET_LOCK_INITIALIZER \
  { ATOMIC_INITIALIZER_UINT( 0U ), ATOMIC_INITIALIZER_UINT( 0U ) }

/**
 * @brief Initializes an SMP ticket lock.
 *
 * Concurrent initialization leads to unpredictable results.
 *
 * @param[in,out] lock The SMP ticket lock control.
 */
static inline void _SMP_ticket_lock_Initialize( SMP_ticket_lock_Control *lock )
{
  _Atomic_Init_uint( &lock->next_ticket, 0U );
  _Atomic_Init_uint( &lock->now_serving, 0U );
}

/**
 * @brief Destroys an SMP ticket lock.
 *
 * Concurrent destruction leads to unpredictable results.
 *
 * @param[in,out] lock The SMP ticket lock control.
 */
static inline void _SMP_ticket_lock_Destroy( SMP_ticket_lock_Control *lock )
{
  (void) lock;
}

/**
 * @brief Acquires an SMP ticket lock.
 *
 * This function will not disable interrupts.  The caller must ensure that the
 * current thread of execution is not interrupted indefinite once it obtained
 * the SMP ticket lock.
 *
 * @param[in,out] lock The SMP ticket lock control.
 */
static inline void _SMP_ticket_lock_Acquire( SMP_ticket_lock_Control *lock )
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
 * @brief Releases an SMP ticket lock.
 *
 * @param[in,out] lock The SMP ticket lock control.
 */
static inline void _SMP_ticket_lock_Release( SMP_ticket_lock_Control *lock )
{
  unsigned int current_ticket =
    _Atomic_Load_uint( &lock->now_serving, ATOMIC_ORDER_RELAXED );
  unsigned int next_ticket = current_ticket + 1U;

  _Atomic_Store_uint( &lock->now_serving, next_ticket, ATOMIC_ORDER_RELEASE );
}

/**
 * @brief SMP lock control.
 */
typedef struct {
  SMP_ticket_lock_Control ticket_lock;
} SMP_lock_Control;

/**
 * @brief Local SMP lock context for acquire and release pairs.
 */
typedef struct {
  ISR_Level isr_level;
} SMP_lock_Context;

/**
 * @brief SMP lock control initializer for static initialization.
 */
#define SMP_LOCK_INITIALIZER { SMP_TICKET_LOCK_INITIALIZER }

/**
 * @brief Initializes an SMP lock.
 *
 * Concurrent initialization leads to unpredictable results.
 *
 * @param[in,out] lock The SMP lock control.
 */
static inline void _SMP_lock_Initialize( SMP_lock_Control *lock )
{
  _SMP_ticket_lock_Initialize( &lock->ticket_lock );
}

/**
 * @brief Destroys an SMP lock.
 *
 * Concurrent destruction leads to unpredictable results.
 *
 * @param[in,out] lock The SMP lock control.
 */
static inline void _SMP_lock_Destroy( SMP_lock_Control *lock )
{
  _SMP_ticket_lock_Destroy( &lock->ticket_lock );
}

/**
 * @brief Acquires an SMP lock.
 *
 * This function will not disable interrupts.  The caller must ensure that the
 * current thread of execution is not interrupted indefinite once it obtained
 * the SMP lock.
 *
 * @param[in,out] lock The SMP lock control.
 * @param[in,out] context The local SMP lock context for an acquire and release
 * pair.
 */
static inline void _SMP_lock_Acquire(
  SMP_lock_Control *lock,
  SMP_lock_Context *context
)
{
  (void) context;
  _SMP_ticket_lock_Acquire( &lock->ticket_lock );
}

/**
 * @brief Releases an SMP lock.
 *
 * @param[in,out] lock The SMP lock control.
 * @param[in,out] context The local SMP lock context for an acquire and release
 * pair.
 */
static inline void _SMP_lock_Release(
  SMP_lock_Control *lock,
  SMP_lock_Context *context
)
{
  (void) context;
  _SMP_ticket_lock_Release( &lock->ticket_lock );
}

/**
 * @brief Disables interrupts and acquires the SMP lock.
 *
 * @param[in,out] lock The SMP lock control.
 * @param[in,out] context The local SMP lock context for an acquire and release
 * pair.
 */
static inline void _SMP_lock_ISR_disable_and_acquire(
  SMP_lock_Control *lock,
  SMP_lock_Context *context
)
{
  _ISR_Disable_without_giant( context->isr_level );
  _SMP_lock_Acquire( lock, context );
}

/**
 * @brief Releases the SMP lock and enables interrupts.
 *
 * @param[in,out] lock The SMP lock control.
 * @param[in,out] context The local SMP lock context for an acquire and release
 * pair.
 */
static inline void _SMP_lock_Release_and_ISR_enable(
  SMP_lock_Control *lock,
  SMP_lock_Context *context
)
{
  _SMP_lock_Release( lock, context );
  _ISR_Enable_without_giant( context->isr_level );
}

/**@}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* defined( RTEMS_SMP ) */

#endif /* _RTEMS_SCORE_SMPLOCK_H */
