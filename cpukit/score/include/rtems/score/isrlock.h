/**
 * @file
 *
 * @ingroup ScoreISRLocks
 *
 * @brief ISR Locks
 */

/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_ISR_LOCK_H
#define _RTEMS_SCORE_ISR_LOCK_H

#include <rtems/score/isrlevel.h>
#include <rtems/score/smplock.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ScoreISRLocks ISR Locks
 *
 * @ingroup ScoreISR
 *
 * @brief Low-level lock to protect critical sections accessed by threads and
 * interrupt service routines.
 *
 * On single processor configurations the ISR locks degrade to simple ISR
 * disable/enable sequences.  No additional storage or objects are required.
 *
 * This synchronization primitive is supported on SMP configurations.  Here SMP
 * locks are used.
 *
 * @{
 */

/**
 * @brief ISR lock control.
 */
typedef struct {
  #if defined( RTEMS_SMP )
    SMP_lock_Control lock;
  #endif
} ISR_lock_Control;

/**
 * @brief Initializer for static initialization of ISR locks.
 */
#if defined( RTEMS_SMP )
  #define ISR_LOCK_INITIALIZER \
    { SMP_LOCK_INITIALIZER }
#else
  #define ISR_LOCK_INITIALIZER \
    { }
#endif

/**
 * @brief Initializes an ISR lock.
 *
 * Concurrent initialization leads to unpredictable results.
 *
 * @param[in,out] _lock The ISR lock control.
 */
#if defined( RTEMS_SMP )
  #define _ISR_lock_Initialize( _lock ) \
    _SMP_lock_Initialize( &( _lock )->lock )
#else
  #define _ISR_lock_Initialize( _lock ) \
    do { \
      (void) _lock; \
    } while (0)
#endif

/**
 * @brief Acquires an ISR lock.
 *
 * Interrupts will be disabled.  On SMP configurations this function acquires
 * an SMP lock.
 *
 * This function can be used in thread and interrupt context.
 *
 * @param[in,out] _lock The ISR lock control.
 * @param[out] _isr_cookie The interrupt status to restore will be returned.
 *
 * @see _ISR_lock_Release_and_ISR_enable().
 */
#if defined( RTEMS_SMP )
  #define _ISR_lock_ISR_disable_and_acquire( _lock, _isr_cookie ) \
    _SMP_lock_ISR_disable_and_acquire( &( _lock )->lock, _isr_cookie )
#else
  #define _ISR_lock_ISR_disable_and_acquire( _lock, _isr_cookie ) \
    do { \
      (void) _lock; \
      _ISR_Disable( _isr_cookie ); \
    } while (0)
#endif

/**
 * @brief Releases an ISR lock.
 *
 * The interrupt status will be restored.  On SMP configurations this function
 * releases an SMP lock.
 *
 * This function can be used in thread and interrupt context.
 *
 * @param[in,out] _lock The ISR lock control.
 * @param[in] _isr_cookie The interrupt status to restore.
 *
 * @see _ISR_lock_ISR_disable_and_acquire().
 */
#if defined( RTEMS_SMP )
  #define _ISR_lock_Release_and_ISR_enable( _lock, _isr_cookie ) \
    _SMP_lock_Release_and_ISR_enable( &( _lock )->lock, _isr_cookie )
#else
  #define _ISR_lock_Release_and_ISR_enable( _lock, _isr_cookie ) \
    do { \
      (void) _lock; \
      _ISR_Enable( _isr_cookie ); \
    } while (0)
#endif

/**
 * @brief Acquires an ISR lock inside an ISR disabled section.
 *
 * The interrupt status will remain unchanged.  On SMP configurations this
 * function acquires an SMP lock.
 *
 * In case the executing context can be interrupted by higher priority
 * interrupts and these interrupts enter the critical section protected by this
 * lock, then the result is unpredictable.
 *
 * @param[in,out] _lock The ISR lock control.
 *
 * @see _ISR_lock_Release().
 */
#if defined( RTEMS_SMP )
  #define _ISR_lock_Acquire( _lock ) \
    _SMP_lock_Acquire( &( _lock )->lock )
#else
  #define _ISR_lock_Acquire( _lock ) \
    do { \
      (void) _lock; \
    } while (0)
#endif

/**
 * @brief Releases an ISR lock inside an ISR disabled section.
 *
 * The interrupt status will remain unchanged.  On SMP configurations this
 * function releases an SMP lock.
 *
 * @param[in,out] _lock The ISR lock control.
 *
 * @see _ISR_lock_Acquire().
 */
#if defined( RTEMS_SMP )
  #define _ISR_lock_Release( _lock ) \
    _SMP_lock_Release( &( _lock )->lock )
#else
  #define _ISR_lock_Release( _lock ) \
    do { \
      (void) _lock; \
    } while (0)
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_ISR_LOCK_H */
