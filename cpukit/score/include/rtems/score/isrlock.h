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
 * http://www.rtems.org/license/LICENSE.
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
 *
 * @warning Empty structures are implementation-defined in C.  GCC gives them a
 * size of zero.  In C++ empty structures have a non-zero size.
 */
typedef struct {
#if defined( RTEMS_SMP )
  SMP_lock_Control Lock;
#endif
} ISR_lock_Control;

/**
 * @brief Local ISR lock context for acquire and release pairs.
 */
typedef struct {
#if defined( RTEMS_SMP )
  SMP_lock_Context Lock_context;
#else
  ISR_Level isr_level;
#endif
} ISR_lock_Context;

/**
 * @brief Defines an ISR lock.
 *
 * Do not add a ';' after this macro.
 *
 * @param _designator The designator for the interrupt lock.
 */
#if defined( RTEMS_SMP )
  #define ISR_LOCK_DEFINE( _designator ) ISR_lock_Control _designator;
#else
  #define ISR_LOCK_DEFINE( _designator )
#endif

/**
 * @brief Initializer for static initialization of ISR locks.
 *
 * @param _name The name for the interrupt lock.  It must be a string.  The
 * name is only used if profiling is enabled.
 */
#if defined( RTEMS_SMP )
  #define ISR_LOCK_INITIALIZER( _name ) \
    { SMP_LOCK_INITIALIZER( _name ) }
#else
  #define ISR_LOCK_INITIALIZER( _name ) \
    { }
#endif

/**
 * @brief Initializes an ISR lock.
 *
 * Concurrent initialization leads to unpredictable results.
 *
 * @param[in] _lock The ISR lock control.
 * @param[in] _name The name for the ISR lock.  This name must be a
 * string persistent throughout the life time of this lock.  The name is only
 * used if profiling is enabled.
 */
#if defined( RTEMS_SMP )
  #define _ISR_lock_Initialize( _lock, _name ) \
    _SMP_lock_Initialize( &( _lock )->Lock, _name )
#else
  #define _ISR_lock_Initialize( _lock, _name )
#endif

/**
 * @brief Destroys an ISR lock.
 *
 * Concurrent destruction leads to unpredictable results.
 *
 * @param[in] _lock The ISR lock control.
 */
#if defined( RTEMS_SMP )
  #define _ISR_lock_Destroy( _lock ) \
    _SMP_lock_Destroy( &( _lock )->Lock )
#else
  #define _ISR_lock_Destroy( _lock )
#endif

/**
 * @brief Acquires an ISR lock.
 *
 * Interrupts will be disabled.  On SMP configurations this function acquires
 * an SMP lock.
 *
 * This function can be used in thread and interrupt context.
 *
 * @param[in] _lock The ISR lock control.
 * @param[in] context The local ISR lock context for an acquire and release
 * pair.
 *
 * @see _ISR_lock_Release_and_ISR_enable().
 */
#if defined( RTEMS_SMP )
  #define _ISR_lock_ISR_disable_and_acquire( _lock, _context ) \
    _SMP_lock_ISR_disable_and_acquire( \
      &( _lock )->Lock, \
      &( _context )->Lock_context \
    )
#else
  #define _ISR_lock_ISR_disable_and_acquire( _lock, _context ) \
    _ISR_Disable( ( _context )->isr_level )
#endif

/**
 * @brief Releases an ISR lock.
 *
 * The interrupt status will be restored.  On SMP configurations this function
 * releases an SMP lock.
 *
 * This function can be used in thread and interrupt context.
 *
 * @param[in] _lock The ISR lock control.
 * @param[in] _context The local ISR lock context for an acquire and release
 * pair.
 *
 * @see _ISR_lock_ISR_disable_and_acquire().
 */
#if defined( RTEMS_SMP )
  #define _ISR_lock_Release_and_ISR_enable( _lock, _context ) \
    _SMP_lock_Release_and_ISR_enable( \
      &( _lock )->Lock, \
      &( _context )->Lock_context \
    )
#else
  #define _ISR_lock_Release_and_ISR_enable( _lock, _context ) \
    _ISR_Enable( ( _context )->isr_level )
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
 * @param[in] _lock The ISR lock control.
 * @param[in] _context The local ISR lock context for an acquire and release
 * pair.
 *
 * @see _ISR_lock_Release().
 */
#if defined( RTEMS_SMP )
  #define _ISR_lock_Acquire( _lock, _context ) \
    _SMP_lock_Acquire( \
      &( _lock )->Lock, \
      &( _context )->Lock_context \
    )
#else
  #define _ISR_lock_Acquire( _lock, _context )
#endif

/**
 * @brief Releases an ISR lock inside an ISR disabled section.
 *
 * The interrupt status will remain unchanged.  On SMP configurations this
 * function releases an SMP lock.
 *
 * @param[in] _lock The ISR lock control.
 * @param[in] _context The local ISR lock context for an acquire and release
 * pair.
 *
 * @see _ISR_lock_Acquire().
 */
#if defined( RTEMS_SMP )
  #define _ISR_lock_Release( _lock, _context ) \
    _SMP_lock_Release( \
      &( _lock )->Lock, \
      &( _context )->Lock_context \
    )
#else
  #define _ISR_lock_Release( _lock, _context )
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_ISR_LOCK_H */
