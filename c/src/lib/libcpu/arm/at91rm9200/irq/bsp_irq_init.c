/*
 * Atmel AT91RM9200 Interrupt handler
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
#include <irq.h>
#include <bsp.h>
#include <at91rm9200.h>

extern void default_int_handler(void);

/* 
 * Interrupt system initialization. Disable interrupts, clear 
 * any that are pending.
 */
void BSP_rtems_irq_mngt_init(void)
{
    long *vectorTable;
    int i;

    vectorTable = (long *) VECTOR_TABLE;

    /* Initialize the vector table contents with default handler */
    for (i=0; i<BSP_MAX_INT; i++) {
        *(vectorTable + i) = (long)(default_int_handler);
    }

    /* disable all interrupts */
    AIC_CTL_REG(AIC_IDCR) = 0xffffffff;
}

