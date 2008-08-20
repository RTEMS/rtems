/*
 * Motorola MC9328MXL Interrupt handler
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
#include <mc9328mxl.h>

extern void default_int_handler(void);

/* 
 * Interrupt system initialization. Disable interrupts, clear 
 * any that are pending.
 */
void BSP_rtems_irq_mngt_init(void)
{
    int i;

    for (i = 0; i < BSP_MAX_INT; i++) {
        bsp_vector_table[i].vector = default_int_handler;
        bsp_vector_table[i].data   = NULL;
    }
}

