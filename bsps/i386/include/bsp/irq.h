/**
 * @file
 * @ingroup i386_irq
 * @brief Interrupt handlers
 */

/* irq.h
 *
 *  This include file describe the data structure and the functions implemented
 *  by rtems to write interrupt handlers.
 *
 *  CopyRight (C) 1998 valette@crf.canon.fr
 *
 *  This code is heavilly inspired by the public specification of STREAM V2
 *  that can be found at :
 *
 *      <http://www.chorus.com/Documentation/index.html> by following
 *  the STREAM API Specification Document link.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

/**
 * @defgroup i386_irq Interrupt handlers
 * @ingroup RTEMSBSPsI386
 * @brief Data structure and the functions to write interrupt handlers
 * @{
 */

#ifndef _IRQ_H_
#define _IRQ_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @brief
 * Include some preprocessor value also used by assember code
 */

#include <bsp/irq_asm.h>
#include <rtems.h>
#define BSP_SHARED_HANDLER_SUPPORT      1
#include <rtems/irq.h>
#include <rtems/irq-extension.h>

/*-------------------------------------------------------------------------+
| Constants
+--------------------------------------------------------------------------*/

/** @brief Base vector for our IRQ handlers. */
#define BSP_IRQ_VECTOR_BASE		BSP_ASM_IRQ_VECTOR_BASE
#define BSP_IRQ_LINES_NUMBER    	16
#define BSP_IRQ_MAX_ON_i8259A		(BSP_IRQ_LINES_NUMBER - 1)

/*
 * Define the number of valid vectors. This is different to the number of IRQ
 * signals supported. Use this value to allocation vector data or range check.
 */
#define BSP_IRQ_VECTOR_NUMBER        17
#define BSP_IRQ_VECTOR_LOWEST_OFFSET 0
#define BSP_IRQ_VECTOR_MAX_OFFSET    (BSP_IRQ_VECTOR_NUMBER - 1)

/** @brief
 * Interrupt offset in comparison to BSP_ASM_IRQ_VECTOR_BASE
 * NB : 1) Interrupt vector number in IDT = offset + BSP_ASM_IRQ_VECTOR_BASE
 * 	    2) The same name should be defined on all architecture
 *	       so that handler connection can be unchanged.
 */
#define BSP_PERIODIC_TIMER      0 /* fixed on all builds of PC */
#define BSP_KEYBOARD          	1 /* fixed on all builds of PC */
#define BSP_UART_COM2_IRQ	3 /* fixed for ISA bus */
#define BSP_UART_COM1_IRQ	4 /* fixed for ISA bus */
#define BSP_UART_COM3_IRQ	5
#define BSP_UART_COM4_IRQ	6
#define BSP_RT_TIMER1	      	8
#define BSP_RT_TIMER3		10
#define BSP_SMP_IPI   		16 /* not part of the ATPIC */

#define BSP_INTERRUPT_VECTOR_MIN BSP_IRQ_VECTOR_LOWEST_OFFSET
#define BSP_INTERRUPT_VECTOR_MAX BSP_IRQ_VECTOR_MAX_OFFSET

/** @brief
 * Type definition for RTEMS managed interrupts
 */
typedef unsigned short rtems_i8259_masks;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _IRQ_H_ */
