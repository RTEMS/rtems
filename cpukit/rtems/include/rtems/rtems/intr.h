/**
 *  @file rtems/rtems/intr.h
 *
 *  @brief Header file for the Interrupt Manager.
 *
 *  This include file contains all the constants and structures associated with
 *  the Interrupt Manager.
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_INTR_H
#define _RTEMS_RTEMS_INTR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/isr.h>

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
 *  @brief Implementation of the rtems_interrupt_catch directive.
 *
 *  This directive installs @a new_isr_handler as the RTEMS interrupt service
 *  routine for the interrupt vector with number @a vector.  The previous RTEMS
 *  interrupt service routine is returned in @a old_isr_handler.
 */
rtems_status_code rtems_interrupt_catch(
  rtems_isr_entry      new_isr_handler,
  rtems_vector_number  vector,
  rtems_isr_entry     *old_isr_handler
);
#endif

/**
 *  @brief Disables all maskable interrupts and returns the previous level in
 *  @a _isr_cookie.
 *
 *  @note The interrupt level shall be of type @ref rtems_interrupt_level.
 */
#define rtems_interrupt_disable( _isr_cookie ) \
    _ISR_Disable(_isr_cookie)

/**
 *  @brief Enables maskable interrupts to the level indicated by @a
 *  _isr_cookie.
 *
 *  @note The interrupt level shall be of type @ref rtems_interrupt_level.
 */
#define rtems_interrupt_enable( _isr_cookie ) \
    _ISR_Enable(_isr_cookie)

/**
 *  @brief Temporarily enables maskable interrupts to the level in @a
 *  _isr_cookie before redisabling them.
 *
 *  @note The interrupt level shall be of type @ref rtems_interrupt_level.
 */
#define rtems_interrupt_flash( _isr_cookie ) \
    _ISR_Flash(_isr_cookie)

/**
 *  @brief Returns true if the processor is currently servicing an interrupt
 *  and false otherwise.
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

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
