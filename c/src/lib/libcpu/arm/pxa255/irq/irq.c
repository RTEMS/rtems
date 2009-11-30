/*
 * PXA255 Interrupt handler by Yang Xi <hiyangxi@gmail.com>
 * Copyright (c) 2004 by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */
#include <bsp.h>
#include <irq.h>
#include <rtems/score/thread.h>
#include <rtems/score/apiext.h>
#include <pxa255.h>

/*
 * This function check that the value given for the irq line
 * is valid.
 */
static int isValidInterrupt(int irq)
{
    if ( (irq < 0) || (irq >= PRIMARY_IRQS)) {
        return 0;
    }
    return 1;
}

/*
 * Installs the interrupt handler.
 */
int BSP_install_rtems_irq_handler  (const rtems_irq_connect_data* irq)
{
    rtems_interrupt_level level;

    if (!isValidInterrupt(irq->name)) {
        return 0;
    }

    /*
     * Check if default handler is actually connected. If not, issue
     * an error. Note: irq->name is a number corresponding to the
     * interrupt number .  We
     * convert it to a long word offset to get source's vector register
     */
        if (IRQ_table[irq->name] != dummy_handler) {
        return 0;
	}

    _CPU_ISR_Disable(level);

    /*
     * store the new handler
     */
    IRQ_table[irq->name] = irq->hdl;

    /*
     * unmask interrupt
     */
    XSCALE_INT_ICMR = XSCALE_INT_ICMR | 1 << irq->name;



    /*
     * Enable interrupt on device
     */
    if(irq->on) {
        irq->on(irq);
    }

    _CPU_ISR_Enable(level);

    return 1;
}

/*
 * Remove and interrupt handler
 */
int BSP_remove_rtems_irq_handler  (const rtems_irq_connect_data* irq)
{
    rtems_interrupt_level level;

    if (!isValidInterrupt(irq->name)) {
        return 0;
    }

    /*
     * Check if the handler is actually connected. If not, issue an error.
     */
    if (IRQ_table[irq->name] != irq->hdl) {
      return 0;
    }
    _CPU_ISR_Disable(level);

    /*
     * mask interrupt
     */
    XSCALE_INT_ICMR  =  XSCALE_INT_ICMR  & (~(1 << irq->name));

    /*
     * Disable interrupt on device
     */
    if(irq->off) {
        irq->off(irq);
    }

    /*
     * restore the default irq value
     */
    IRQ_table[irq->name] = dummy_handler;

    _CPU_ISR_Enable(level);

    return 1;
}
