/**
 * @file
 *
 * @ingroup RTEMSScoreISRLocks
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreISRLocks.
 */

/*
 * Copyright (c) 2013, 2019 embedded brains GmbH.  All rights reserved.
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
 * @defgroup RTEMSScoreISRLocks ISR Locks
 *
 * @ingroup RTEMSScoreISR
 *
 * @brief This group contains the ISR locks implementation.
 *
 * The ISR locks are low-level locks to protect critical sections accessed by
 * threads and interrupt service routines.
 *
 * In uniprocessor configurations the ISR locks degrade to simple ISR
 * disable/enable sequences.  No additional storage or objects are required.
 *
 * This synchronization primitive is supported in SMP configurations.  Here SMP
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
#if defined( RTEMS_PROFILING )
  /**
   * @brief The last interrupt disable instant in CPU counter ticks.
   */
  CPU_Counter_ticks ISR_disable_instant;
#endif
} ISR_lock_Context;

/**
 * @brief Defines an ISR lock member.
 *
 * Do not add a ';' after this macro.
 *
 * @param _designator The designator for the interrupt lock.
 */
#if defined( RTEMS_SMP )
  #define ISR_LOCK_MEMBER( _designator ) ISR_lock_Control _designator;
#else
  #define ISR_LOCK_MEMBER( _designator )
#endif

/**
 * @brief Declares an ISR lock variable.
 *
 * Do not add a ';' after this macro.
 *
 * @param _qualifier The qualifier for the interrupt lock, e.g. extern.
 * @param _designator The designator for the interrupt lock.
 */
#if defined( RTEMS_SMP )
  #define ISR_LOCK_DECLARE( _qualifier, _designator ) \
    _qualifier ISR_lock_Control _designator;
#else
  #define ISR_LOCK_DECLARE( _qualifier, _designator )
#endif

/**
 * @brief Defines an ISR lock variable.
 *
 * Do not add a ';' after this macro.
 *
 * @param _qualifier The qualifier for the interrupt lock, e.g. static.
 * @param _designator The designator for the interrupt lock.
 * @param _name The name for the interrupt lock.  It must be a string.  The
 * name is only used if profiling is enabled.
 */
#if defined( RTEMS_SMP )
  #define ISR_LOCK_DEFINE( _qualifier, _designator, _name ) \
    _qualifier ISR_lock_Control _designator = { SMP_LOCK_INITIALIZER( _name ) };
#else
  #define ISR_LOCK_DEFINE( _qualifier, _designator, _name )
#endif

/**
 * @brief Defines an ISR lock variable reference.
 *
 * Do not add a ';' after this macro.
 *
 * @param _designator The designator for the interrupt lock reference.
 * @param _target The target for the interrupt lock reference.
 */
#if defined( RTEMS_SMP )
  #define ISR_LOCK_REFERENCE( _designator, _target ) \
    ISR_lock_Control *_designator = _target;
#else
  #define ISR_LOCK_REFERENCE( _designator, _target )
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
 * @brief Sets the ISR level in the ISR lock context.
 *
 * @param[out] context The ISR lock context.
 * @param level The ISR level.
 */
RTEMS_INLINE_ROUTINE void _ISR_lock_Context_set_level(
  ISR_lock_Context *context,
  ISR_Level         level
)
{
#if defined( RTEMS_SMP )
  context->Lock_context.isr_level = level;
#else
  context->isr_level = level;
#endif
}

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
 * @brief Sets the name of an ISR lock.
 *
 * @param[out] _lock The ISR lock control.
 * @param _name The name for the ISR lock.  This name must be a string
 *   persistent throughout the life time of this lock.  The name is only used
 *   if profiling is enabled.
 */
#if defined( RTEMS_SMP )
  #define _ISR_lock_Set_name( _lock, _name ) \
    _SMP_lock_Set_name( &( _lock )->Lock, _name )
#else
  #define _ISR_lock_Set_name( _lock, _name )
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
 * @param[in] _context The local ISR lock context for an acquire and release
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
    _ISR_Local_disable( ( _context )->isr_level )
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
    _ISR_Local_enable( ( _context )->isr_level )
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
    do { \
      _Assert( _ISR_Get_level() != 0 ); \
      _SMP_lock_Acquire( \
        &( _lock )->Lock, \
        &( _context )->Lock_context \
      ); \
    } while ( 0 )
#else
  #define _ISR_lock_Acquire( _lock, _context ) \
    do { (void) _context; } while ( 0 )
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
  #define _ISR_lock_Release( _lock, _context ) \
    do { (void) _context; } while ( 0 )
#endif

/**
 * @brief Acquires an ISR lock inside an ISR disabled section (inline).
 *
 * @see _ISR_lock_Acquire().
 */
#if defined( RTEMS_SMP )
  #define _ISR_lock_Acquire_inline( _lock, _context ) \
    do { \
      _Assert( _ISR_Get_level() != 0 ); \
      _SMP_lock_Acquire_inline( \
        &( _lock )->Lock, \
        &( _context )->Lock_context \
      ); \
    } while ( 0 )
#else
  #define _ISR_lock_Acquire_inline( _lock, _context ) \
    do { (void) _context; } while ( 0 )
#endif

/**
 * @brief Releases an ISR lock inside an ISR disabled section (inline).
 *
 * @see _ISR_lock_Release().
 */
#if defined( RTEMS_SMP )
  #define _ISR_lock_Release_inline( _lock, _context ) \
    _SMP_lock_Release_inline( \
      &( _lock )->Lock, \
      &( _context )->Lock_context \
    )
#else
  #define _ISR_lock_Release_inline( _lock, _context ) \
    do { (void) _context; } while ( 0 )
#endif

#if defined( RTEMS_DEBUG )
  /**
   * @brief Returns true, if the ISR lock is owned by the current processor,
   * otherwise false.
   *
   * On uni-processor configurations, this function returns true, if interrupts
   * are disabled, otherwise false.
   *
   * @param[in] _lock The ISR lock control.
   */
  #if defined( RTEMS_SMP )
    #define _ISR_lock_Is_owner( _lock ) \
      _SMP_lock_Is_owner( &( _lock )->Lock )
  #else
    #define _ISR_lock_Is_owner( _lock ) \
      ( _ISR_Get_level() != 0 )
  #endif
#endif

#if defined( RTEMS_PROFILING )
  #define _ISR_lock_ISR_disable_profile( _context ) \
    ( _context )->ISR_disable_instant = _CPU_Counter_read();
#else
  #define _ISR_lock_ISR_disable_profile( _context )
#endif

/**
 * @brief Disables interrupts and saves the previous interrupt state in the ISR
 * lock context.
 *
 * This function can be used in thread and interrupt context.
 *
 * @param[in] _context The local ISR lock context to store the interrupt state.
 *
 * @see _ISR_lock_ISR_enable().
 */
#if defined( RTEMS_SMP )
  #define _ISR_lock_ISR_disable( _context ) \
    do { \
      _ISR_Local_disable( ( _context )->Lock_context.isr_level ); \
      _ISR_lock_ISR_disable_profile( _context ) \
    } while ( 0 )
#else
  #define _ISR_lock_ISR_disable( _context ) \
    do { \
      _ISR_Local_disable( ( _context )->isr_level ); \
      _ISR_lock_ISR_disable_profile( _context ) \
    } while ( 0 )
#endif

/**
 * @brief Restores the saved interrupt state of the ISR lock context.
 *
 * This function can be used in thread and interrupt context.
 *
 * @param[in] _context The local ISR lock context containing the saved
 * interrupt state.
 *
 * @see _ISR_lock_ISR_disable().
 */
#if defined( RTEMS_SMP )
  #define _ISR_lock_ISR_enable( _context ) \
    _ISR_Local_enable( ( _context )->Lock_context.isr_level )
#else
  #define _ISR_lock_ISR_enable( _context ) \
    _ISR_Local_enable( ( _context )->isr_level )
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_ISR_LOCK_H */
