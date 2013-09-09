/**
 *  @file  rtems/bspsmp.h
 *
 *  @brief Interface Between RTEMS and an SMP Aware BSP
 *
 *  This include file defines the interface between RTEMS and an
 *  SMP aware BSP.  These methods will only be used when RTEMS
 *  is configured with SMP support enabled.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_BSPSMP_H
#define _RTEMS_BSPSMP_H

#include <rtems/score/cpuopts.h>

#if defined (RTEMS_SMP)
#include <rtems/score/percpu.h>

/**
 *  @defgroup RTEMS BSP SMP Interface
 *
 *  @ingroup Score
 *
 *  This defines the interface between RTEMS and the BSP for
 *  SMP support.  The interface uses the term primary
 *  to refer to the "boot" processor and secondary to refer
 *  to the "application" processors.  Different architectures
 *  use different terminology.
 *
 *  It is assumed that when the processor is reset and thus
 *  when RTEMS is initialized, that the primary processor is
 *  the only one executing.  The others are assumed to be in
 *  a quiescent or reset state awaiting a command to come online.
 */

/**@{*/

#ifdef __cplusplus
extern "C" {
#endif


#ifndef ASM

/**
 * @brief Performs BSP specific SMP initialization in the context of the main
 * processor.
 *
 * This function is invoked on the main processor by RTEMS during
 * initialization.  All interrupt stacks are allocated at this point in case
 * the CPU port allocates the interrupt stacks.
 *
 * The BSP may start secondary processors now.
 *
 * @param[in] configured_cpu_count The count of processors requested by the
 * application configuration.
 *
 * @return The count of processors available for the application in the system.
 * This value is less than or equal to the configured count of processors.
 */
uint32_t bsp_smp_initialize( uint32_t configured_cpu_count );

/**
 *  @brief Generate an interprocessor broadcast interrupt.
 *
 *  This method is invoked when RTEMS wants to let all of the other
 *  CPUs know that it has sent them message.  CPUs not including
 *  the originating CPU should receive the interrupt.

 *
 *  @note On CPUs without the capability to generate a broadcast
 *        to all other CPUs interrupt, this can be implemented by
 *        a loop of sending interrupts to specific CPUs.
 */
void bsp_smp_broadcast_interrupt(void);

/**
 * @brief Performs high-level initialization of a secondary processor and runs
 * the application threads.
 *
 * The low-level initialization code must call this function to hand over the
 * control of this processor to RTEMS.  Interrupts must be disabled.  It must
 * be possible to send inter-processor interrupts to this processor.  Since
 * interrupts are disabled the inter-processor interrupt delivery is postponed
 * until interrupts are enabled the first time.  Interrupts are enabled during
 * the execution begin of threads in case they have interrupt level zero (this
 * is the default).
 *
 * The pre-requisites for the call to this function are
 * - disabled interrupts,
 * - delivery of inter-processor interrupts is possible,
 * - a valid stack pointer and enough stack space,
 * - a valid code memory, and
 * - a valid BSS section.
 *
 * This function must not be called by the main processor.  This function does
 * not return to the caller.
 */
void rtems_smp_secondary_cpu_initialize( void )
  RTEMS_COMPILER_NO_RETURN_ATTRIBUTE;

/**
 *  @brief Process the incoming interprocessor request.
 *
 *  This is the method called by the BSP's interrupt handler
 *  to process the incoming interprocessor request.
 */
void rtems_smp_process_interrupt(void);

#endif

#ifdef __cplusplus
}
#endif

#endif

/**@}*/
#endif

/* end of include file */
