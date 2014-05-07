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
 */
typedef struct {
#if defined( RTEMS_SMP )
  SMP_lock_Control lock;
#endif
} ISR_lock_Control;

/**
 * @brief Local ISR lock context for acquire and release pairs.
 */
typedef struct {
#if defined( RTEMS_SMP )
  SMP_lock_Context lock_context;
#else
  ISR_Level isr_level;
#endif
} ISR_lock_Context;

/**
 * @brief Initializer for static initialization of ISR locks.
 *
 * @param _name The name for the interrupt lock.  It must be a string.  The
 * name is only used if profiling is enabled.
 */
#if defined( RTEMS_SMP )
  #define ISR_LOCK_INITIALIZER( name ) \
    { SMP_LOCK_INITIALIZER( name ) }
#else
  #define ISR_LOCK_INITIALIZER( name ) \
    { }
#endif

/**
 * @brief Initializes an ISR lock.
 *
 * Concurrent initialization leads to unpredictable results.
 *
 * @param[in,out] lock The ISR lock control.
 * @param[in] _name The name for the ISR lock.  This name must be a
 * string persistent throughout the life time of this lock.  The name is only
 * used if profiling is enabled.
 */
static inline void _ISR_lock_Initialize(
  ISR_lock_Control *lock,
  const char *name
)
{
#if defined( RTEMS_SMP )
  _SMP_lock_Initialize( &lock->lock, name );
#else
  (void) lock;
  (void) name;
#endif
}

/**
 * @brief Destroys an ISR lock.
 *
 * Concurrent destruction leads to unpredictable results.
 *
 * @param[in,out] lock The ISR lock control.
 */
static inline void _ISR_lock_Destroy( ISR_lock_Control *lock )
{
#if defined( RTEMS_SMP )
  _SMP_lock_Destroy( &lock->lock );
#else
  (void) lock;
#endif
}

/**
 * @brief Acquires an ISR lock.
 *
 * Interrupts will be disabled.  On SMP configurations this function acquires
 * an SMP lock.
 *
 * This function can be used in thread and interrupt context.
 *
 * @param[in,out] lock The ISR lock control.
 * @param[in,out] context The local ISR lock context for an acquire and release
 * pair.
 *
 * @see _ISR_lock_Release_and_ISR_enable().
 */
static inline void _ISR_lock_ISR_disable_and_acquire(
  ISR_lock_Control *lock,
  ISR_lock_Context *context
)
{
#if defined( RTEMS_SMP )
  _SMP_lock_ISR_disable_and_acquire( &lock->lock, &context->lock_context );
#else
  (void) lock;
  _ISR_Disable( context->isr_level );
#endif
}

/**
 * @brief Releases an ISR lock.
 *
 * The interrupt status will be restored.  On SMP configurations this function
 * releases an SMP lock.
 *
 * This function can be used in thread and interrupt context.
 *
 * @param[in,out] lock The ISR lock control.
 * @param[in,out] context The local ISR lock context for an acquire and release
 * pair.
 *
 * @see _ISR_lock_ISR_disable_and_acquire().
 */
static inline void _ISR_lock_Release_and_ISR_enable(
  ISR_lock_Control *lock,
  ISR_lock_Context *context
)
{
#if defined( RTEMS_SMP )
  _SMP_lock_Release_and_ISR_enable( &lock->lock, &context->lock_context );
#else
  (void) lock;
  _ISR_Enable( context->isr_level );
#endif
}

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
 * @param[in,out] lock The ISR lock control.
 * @param[in,out] context The local ISR lock context for an acquire and release
 * pair.
 *
 * @see _ISR_lock_Release().
 */
static inline void _ISR_lock_Acquire(
  ISR_lock_Control *lock,
  ISR_lock_Context *context
)
{
#if defined( RTEMS_SMP )
  _SMP_lock_Acquire( &lock->lock, &context->lock_context );
#else
  (void) lock;
  (void) context;
#endif
}

/**
 * @brief Releases an ISR lock inside an ISR disabled section.
 *
 * The interrupt status will remain unchanged.  On SMP configurations this
 * function releases an SMP lock.
 *
 * @param[in,out] lock The ISR lock control.
 * @param[in,out] context The local ISR lock context for an acquire and release
 * pair.
 *
 * @see _ISR_lock_Acquire().
 */
static inline void _ISR_lock_Release(
  ISR_lock_Control *lock,
  ISR_lock_Context *context
)
{
#if defined( RTEMS_SMP )
  _SMP_lock_Release( &lock->lock, &context->lock_context );
#else
  (void) lock;
  (void) context;
#endif
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_ISR_LOCK_H */
