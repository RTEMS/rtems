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
 *  $Id$
 */
#include <bsp.h>
#include <irq.h>
#include <rtems/score/thread.h>
#include <rtems/score/apiext.h>
#include <at91rm9200.h>

/*
 * This function check that the value given for the irq line
 * is valid.
 */
static int isValidInterrupt(int irq)
{
    if ( (irq < 0) || (irq >= AT91RM9200_MAX_INT)) {
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
     * sources PID (see the at91rm9200_pid for this mapping).  We 
     * convert it to a long word offset to get source's vector register 
     */
    if (AIC_SVR_REG(irq->name * 4) != (uint32_t) default_int_handler) {
        return 0;
    }
    
    rtems_interrupt_disable(level);
    
    /*
     * store the new handler
     */
    AIC_SVR_REG(irq->name * 4) = (uint32_t) irq->hdl;
    
    /*
     * unmask interrupt
     */
    AIC_CTL_REG(AIC_IECR) = 1 << irq->name;
    
    /*
     * Enable interrupt on device
     */
    if(irq->on) {
        irq->on(irq);
    }
    
    rtems_interrupt_enable(level);
    
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
    if (AIC_SVR_REG(irq->name * 4) != (uint32_t) irq->hdl) {
      return 0;
    }
    rtems_interrupt_disable(level);

    /*
     * mask interrupt
     */
    AIC_CTL_REG(AIC_IDCR) = 1 << irq->name;
    
    /*
     * Disable interrupt on device
     */
    if(irq->off) {
        irq->off(irq);
    }

    /*
     * restore the default irq value
     */
    AIC_SVR_REG(irq->name * 4) = (uint32_t) default_int_handler;
    
    rtems_interrupt_enable(level);

    return 1;
}
