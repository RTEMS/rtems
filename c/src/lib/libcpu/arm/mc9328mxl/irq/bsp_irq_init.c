/*
 * Motorola MC9328MXL Interrupt handler
 *
 * Copyright (c) 2004 by Jay Monkman <jtm@lopingdog.com>
 *      
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *
 *  $Id$
 */
#include <irq.h>
#include <bsp.h>
#include <mc9328mxl.h>

extern void default_int_handler();

/* 
 * Interrupt system initialization. Disable interrupts, clear 
 * any that are pending.
 */
void BSP_rtems_irq_mngt_init()
{
#if 0
    /* disable all interrupts */
    AIC_CTL_REG(AIC_IDCR) = 0xffffffff;
#endif

}

