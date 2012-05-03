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
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _IRQ_H_
#define _IRQ_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
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

    /* Base vector for our IRQ handlers. */
#define BSP_IRQ_VECTOR_BASE		BSP_ASM_IRQ_VECTOR_BASE
#define BSP_IRQ_LINES_NUMBER    	17
#define BSP_LOWEST_OFFSET		0
#define BSP_MAX_ON_i8259S		(BSP_IRQ_LINES_NUMBER - 2)
#define BSP_MAX_OFFSET		 	(BSP_IRQ_LINES_NUMBER - 1)
    /*
     * Interrupt offset in comparison to BSP_ASM_IRQ_VECTOR_BASE
     * NB : 1) Interrupt vector number in IDT = offset + BSP_ASM_IRQ_VECTOR_BASE
     * 	    2) The same name should be defined on all architecture
     *	       so that handler connection can be unchanged.
     */
#define BSP_PERIODIC_TIMER      0
#define BSP_KEYBOARD          	1
#define BSP_UART_COM2_IRQ	3
#define BSP_UART_COM1_IRQ	4
#define BSP_UART_COM3_IRQ	5
#define BSP_UART_COM4_IRQ	6
#define BSP_RT_TIMER1	      	8
#define BSP_RT_TIMER3		10
#define BSP_SMP_IPI   		16

#define BSP_INTERRUPT_VECTOR_MIN BSP_LOWEST_OFFSET
#define BSP_INTERRUPT_VECTOR_MAX BSP_MAX_OFFSET

/*
 * Type definition for RTEMS managed interrupts
 */
typedef unsigned short rtems_i8259_masks;

extern  rtems_i8259_masks i8259s_cache;

/*-------------------------------------------------------------------------+
| Function Prototypes.
+--------------------------------------------------------------------------*/
/*
 * ------------------------ Intel 8259 (or emulation) Mngt Routines -------
 */

/*
 * function to disable a particular irq at 8259 level. After calling
 * this function, even if the device asserts the interrupt line it will
 * not be propagated further to the processor
 */
int BSP_irq_disable_at_i8259s        (const rtems_irq_number irqLine);
/*
 * function to enable a particular irq at 8259 level. After calling
 * this function, if the device asserts the interrupt line it will
 * be propagated further to the processor
 */
int BSP_irq_enable_at_i8259s		(const rtems_irq_number irqLine);
/*
 * function to acknoledge a particular irq at 8259 level. After calling
 * this function, if a device asserts an enabled interrupt line it will
 * be propagated further to the processor. Mainly usefull for people
 * writting raw handlers as this is automagically done for rtems managed
 * handlers.
 */
int BSP_irq_ack_at_i8259s           	(const rtems_irq_number irqLine);
/*
 * function to check if a particular irq is enabled at 8259 level. After calling
 */
int BSP_irq_enabled_at_i8259s        	(const rtems_irq_number irqLine);

#ifdef __cplusplus
}
#endif

#endif /* _IRQ_H_ */
