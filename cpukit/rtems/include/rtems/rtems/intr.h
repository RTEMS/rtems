/**
 *  @file rtems/rtems/intr.h
 *
 * @defgroup ClassicINTR Interrupts
 *
 * @ingroup ClassicRTEMS
 * @brief Header file for Interrupt Manager
 *
 * This include file contains all the constants and structures associated with
 * the Interrupt Manager.
 */

/* COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_INTR_H
#define _RTEMS_RTEMS_INTR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/rtems/status.h>
#include <rtems/score/isr.h>
#include <rtems/score/isrlock.h>

/**
 *  @defgroup ClassicINTR Interrupts
 *
 *  @ingroup ClassicRTEMS
 *
 *  This encapsulates functionality which XXX
 */
/**@{*/

/**
 *  @brief Interrupt level type.
 */
typedef ISR_Level rtems_interrupt_level;

/**
 *  @brief Control block type used to manage the vectors.
 */
typedef ISR_Vector_number rtems_vector_number;

/**
 *  @brief Return type for interrupt handler.
 */
typedef ISR_Handler rtems_isr;

#if (CPU_SIMPLE_VECTORED_INTERRUPTS == FALSE)

typedef ISR_Handler_entry rtems_isr_entry;

#else
/**
 *  @brief Interrupt handler type.
 *
 *  @see rtems_interrupt_catch()
 */
typedef rtems_isr ( *rtems_isr_entry )(
                 rtems_vector_number
             );

/**
 * @brief RTEMS Interrupt Catch
 *
 * This directive installs @a new_isr_handler as the RTEMS interrupt service
 * routine for the interrupt vector with number @a vector. The previous RTEMS
 * interrupt service routine is returned in @a old_isr_handler.
 *  
 * @param[in] new_isr_handler is the address of interrupt service routine
 * @param[in] vector is the interrupt vector number
 * @param[in] old_isr_handler address at which to store previous ISR address
 * 
 * @retval RTEMS_SUCCESSFUL and *old_isr_handler filled with previous ISR
 * 		address
 */
rtems_status_code rtems_interrupt_catch(
  rtems_isr_entry      new_isr_handler,
  rtems_vector_number  vector,
  rtems_isr_entry     *old_isr_handler
);
#endif

/**
 *  @brief Disable RTEMS Interrupt
 *
 *  @note The interrupt level shall be of type @ref rtems_interrupt_level.
 */
#define rtems_interrupt_disable( _isr_cookie ) \
    _ISR_Disable(_isr_cookie)

/**
 *  @brief Enable RTEMS Interrupt
 *
 *  @note The interrupt level shall be of type @ref rtems_interrupt_level.
 */
#define rtems_interrupt_enable( _isr_cookie ) \
    _ISR_Enable(_isr_cookie)

/**
 *  @brief Flash RTEMS Interrupt
 *
 *  @note The interrupt level shall be of type @ref rtems_interrupt_level.
 */
#define rtems_interrupt_flash( _isr_cookie ) \
    _ISR_Flash(_isr_cookie)

/**
 *  @brief RTEMS Interrupt Is in Progress
 *
 *  A return value of true indicates that the caller is an interrupt service
 *  routine and @b not a thread.  The directives available to an interrupt
 *  service routine are restricted.
 */
#define rtems_interrupt_is_in_progress() \
    _ISR_Is_in_progress()

/**
 *  @brief This routine generates an interrupt.
 *
 *  @note No implementation.
 */
#define rtems_interrupt_cause( _interrupt_to_cause )

/**
 *  @brief This routine clears the specified interrupt.
 *
 *  @note No implementation.
 */
#define rtems_interrupt_clear( _interrupt_to_clear )

/**
 * @defgroup ClassicINTRLocks Interrupt Locks
 *
 * @ingroup ClassicINTR
 *
 * @brief Low-level lock to protect critical sections accessed by threads and
 * interrupt service routines.
 *
 * On single processor configurations the interrupt locks degrade to simple
 * interrupt disable/enable sequences.  No additional storage or objects are
 * required.
 *
 * This synchronization primitive is supported on SMP configurations.  Here SMP
 * locks are used.
 * @{
 */

/**
 * @brief Interrupt lock control.
 */
typedef ISR_lock_Control rtems_interrupt_lock;

/**
 * @brief Initializer for static initialization of interrupt locks.
 */
#define RTEMS_INTERRUPT_LOCK_INITIALIZER ISR_LOCK_INITIALIZER

/**
 * @brief Initializes an interrupt lock.
 *
 * Concurrent initialization leads to unpredictable results.
 *
 * @param[in,out] _lock The interrupt lock.
 */
#define rtems_interrupt_lock_initialize( _lock ) \
  _ISR_lock_Initialize( _lock )

/**
 * @brief Acquires an interrupt lock.
 *
 * Interrupts will be disabled.  On SMP configurations this function acquires
 * an SMP lock.
 *
 * This function can be used in thread and interrupt context.
 *
 * @param[in,out] _lock The interrupt lock.
 * @param[out] _isr_cookie The interrupt status to restore will be returned.
 *
 * @see rtems_interrupt_lock_release().
 */
#define rtems_interrupt_lock_acquire( _lock, _isr_cookie ) \
  _ISR_lock_ISR_disable_and_acquire( _lock, _isr_cookie )

/**
 * @brief Releases an interrupt lock.
 *
 * The interrupt status will be restored.  On SMP configurations this function
 * releases an SMP lock.
 *
 * This function can be used in thread and interrupt context.
 *
 * @param[in,out] _lock The interrupt lock.
 * @param[in] _isr_cookie The interrupt status to restore.
 *
 * @see rtems_interrupt_lock_acquire().
 */
#define rtems_interrupt_lock_release( _lock, _isr_cookie ) \
  _ISR_lock_Release_and_ISR_enable( _lock, _isr_cookie )

/**
 * @brief Acquires an interrupt lock in the corresponding interrupt service
 * routine.
 *
 * The interrupt status will remain unchanged.  On SMP configurations this
 * function acquires an SMP lock.
 *
 * In case the corresponding interrupt service routine can be interrupted by
 * higher priority interrupts and these interrupts enter the critical section
 * protected by this lock, then the result is unpredictable.
 *
 * @param[in,out] _lock The interrupt lock.
 *
 * @see rtems_interrupt_lock_release_isr().
 */
#define rtems_interrupt_lock_acquire_isr( _lock ) \
  _ISR_lock_Acquire( _lock )

/**
 * @brief Releases an interrupt lock in the corresponding interrupt service
 * routine.
 *
 * The interrupt status will remain unchanged.  On SMP configurations this
 * function releases an SMP lock.
 *
 * @param[in,out] _lock The interrupt lock.
 *
 * @see rtems_interrupt_lock_acquire_isr().
 */
#define rtems_interrupt_lock_release_isr( _lock ) \
  _ISR_lock_Release( _lock )

/** @} */

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
