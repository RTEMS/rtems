/*
 * Interrupt handler Header file
 *
 * Copyright (c) 2004 by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.com/license/LICENSE.
 *
 *
 *  $Id$
 */

#ifndef __IRQ_H__
#define __IRQ_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __asm__

/*
 * Include some preprocessor value also used by assember code
 */

#include <rtems/irq.h>
#include <rtems.h>
#include <at91rm9200.h>

extern void default_int_handler(rtems_irq_hdl_param unused);
/***********************************************************************
 * Constants
 **********************************************************************/
/* possible interrupt sources on the AT91RM9200 */
#define AT91RM9200_INT_FIQ        0
#define AT91RM9200_INT_SYSIRQ     1
#define AT91RM9200_INT_PIOA       2
#define AT91RM9200_INT_PIOB       3
#define AT91RM9200_INT_PIOC       4
#define AT91RM9200_INT_PIOD       5
#define AT91RM9200_INT_US0        6
#define AT91RM9200_INT_US1        7
#define AT91RM9200_INT_US2        8
#define AT91RM9200_INT_US3        9
#define AT91RM9200_INT_MCI       10
#define AT91RM9200_INT_UDP       11
#define AT91RM9200_INT_TWI       12
#define AT91RM9200_INT_SPI       13
#define AT91RM9200_INT_SSC0      14
#define AT91RM9200_INT_SSC1      15
#define AT91RM9200_INT_SSC2      16
#define AT91RM9200_INT_TC0       17
#define AT91RM9200_INT_TC1       18
#define AT91RM9200_INT_TC2       19
#define AT91RM9200_INT_TC3       20
#define AT91RM9200_INT_TC4       21
#define AT91RM9200_INT_TC5       22
#define AT91RM9200_INT_UHP       23
#define AT91RM9200_INT_EMAC      24
#define AT91RM9200_INT_IRQ0      25
#define AT91RM9200_INT_IRQ1      26
#define AT91RM9200_INT_IRQ2      27
#define AT91RM9200_INT_IRQ3      28
#define AT91RM9200_INT_IRQ4      28
#define AT91RM9200_INT_IRQ5      30
#define AT91RM9200_INT_IRQ6      31
#define AT91RM9200_MAX_INT       32

/* vector table used by shared/irq_init.c */
/* we can treat the AT91RM9200 AIC_SVR_BASE as */
/* a vector table */
#define VECTOR_TABLE AIC_SVR_BASE

/*
 * function to initialize the interrupt for a specific BSP
 */
void BSP_rtems_irq_mngt_init();

#endif /* __asm__ */

#ifdef __cplusplus
}
#endif

#endif /* __IRQ_H__ */
