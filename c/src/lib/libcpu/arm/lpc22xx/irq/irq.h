/*
 * Interrupt handler Header file
 *
 * Copyright (c) 2010 embedded brains GmbH.
 *
 * Copyright (c) 2006 by Ray <rayx.cn@gmail.com> to support LPC ARM
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __IRQ_H__
#define __IRQ_H__

#ifndef __asm__

#include <rtems.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>

#endif /* __asm__ */

/* possible interrupt sources on the LPC22xx */
#define LPC22xx_INTERRUPT_WDINT	 0	/* Watchdog int. 0 */
#define LPC22xx_INTERRUPT_RSV0	 1	/* Reserved int. 1 */
#define LPC22xx_INTERRUPT_DBGRX	 2	/* Embedded ICE DbgCommRx receive */
#define LPC22xx_INTERRUPT_DBGTX	 3	/* Embedded ICE DbgCommRx Transmit*/
#define LPC22xx_INTERRUPT_TIMER0 4	/* Timer 0 */
#define LPC22xx_INTERRUPT_TIMER1 5	/* Timer 1 */
#define LPC22xx_INTERRUPT_UART0	 6	/* UART 0 */
#define LPC22xx_INTERRUPT_UART1  7	/* UART 1 */
#define LPC22xx_INTERRUPT_PWM0 	 8	/* PWM */
#define LPC22xx_INTERRUPT_I2C 	 9	/* I2C  */
#define LPC22xx_INTERRUPT_SPI0 	10	/* SPI0 */
#define LPC22xx_INTERRUPT_SPI1 	11	/* SPI1 */
#define LPC22xx_INTERRUPT_PLL 	12	/* PLL */
#define LPC22xx_INTERRUPT_RTC 	13	/* RTC */
#define LPC22xx_INTERRUPT_EINT0	14	/* Externel Interrupt 0 */
#define LPC22xx_INTERRUPT_EINT1	15	/* Externel Interrupt 1 */
#define LPC22xx_INTERRUPT_EINT2	16	/* Externel Interrupt 2 */
#define LPC22xx_INTERRUPT_EINT3	17	/* Externel Interrupt 3 */
#define LPC22xx_INTERRUPT_ADC 	18	/* AD Converter */
/* Following interrupt used by lpc229x */
#define LPC22xx_INTERRUPT_CANERR 19	/* CAN LUTerr interrupt */
#define LPC22xx_INTERRUPT_CAN1TX 20	/* CAN1 Tx interrupt */
#define LPC22xx_INTERRUPT_CAN1RX 21	/* CAN1 Rx interrupt */
#define LPC22xx_INTERRUPT_CAN2TX 22	/* CAN2 Tx interrupt */
#define LPC22xx_INTERRUPT_CAN2RX 23	/* CAN2 Rx interrupt */
#define LPC22xx_INTERRUPT_CAN3TX 24	/* CAN1 Tx interrupt */
#define LPC22xx_INTERRUPT_CAN3RX 25	/* CAN1 Rx interrupt */
#define LPC22xx_INTERRUPT_CAN4TX 26	/* CAN2 Tx interrupt */
#define LPC22xx_INTERRUPT_CAN4RX 27	/* CAN2 Rx interrupt */
#define BSP_MAX_INT              28

#define BSP_INTERRUPT_VECTOR_MIN 0

#define BSP_INTERRUPT_VECTOR_MAX (BSP_MAX_INT - 1)

#define UNDEFINED_INSTRUCTION_VECTOR_ADDR   (*(u_long *)0x00000004L)
#define SOFTWARE_INTERRUPT_VECTOR_ADDR      (*(u_long *)0x00000008L)
#define PREFETCH_ABORT_VECTOR_ADDR          (*(u_long *)0x0000000CL)
#define DATA_ABORT_VECTOR_ADDR              (*(u_long *)0x00000010L)
#define IRQ_VECTOR_ADDR                     (*(u_long *)0x00000018L)
#define FIQ_VECTOR_ADDR                     (*(u_long *)0x0000001CL)

#define DATA_ABORT_ISR_ADDR                 (*(u_long *)0x00000030L)
#define IRQ_ISR_ADDR                        (*(u_long *)0x00000038L)
#define FIQ_ISR_ADDR                        (*(u_long *)0x0000003CL)

#endif /* __IRQ_H__ */
