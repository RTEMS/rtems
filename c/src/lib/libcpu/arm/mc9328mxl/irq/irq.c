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
 *  $Id$
 */
#include <bsp.h>
#include <irq.h>
#include <rtems/score/thread.h>
#include <rtems/score/apiext.h>
#include <mc9328mxl.h>

/*
 * This function check that the value given for the irq line
 * is valid.
 */
static int isValidInterrupt(int irq)
{
  if ( (irq < 0) || (irq >= BSP_MAX_INT))
    return 0;
  return 1;
}

/*
 * Installs the interrupt handler.
 *
 * You should only have to add the code to unmask the interrupt.
 *
 */
int BSP_install_rtems_irq_handler  (const rtems_irq_connect_data* irq)
{
    rtems_interrupt_level level;
    rtems_irq_hdl *bsp_tbl;

    bsp_tbl = &bsp_vector_table;
    
    if (!isValidInterrupt(irq->name)) {
      return 0;
    }

    /*
     * Check if default handler is actually connected. If not issue an error.
     */
    if (bsp_tbl[irq->name] != default_int_handler) {
      return 0;
    }

    _CPU_ISR_Disable(level);

    /*
     * store the new handler
     */
    bsp_tbl[irq->name] = irq->hdl;

    /*
     * Enable interrupt on device
     */
    if(irq->on)
    {
    	irq->on(irq);
    }
    
    _CPU_ISR_Enable(level);
    
    return 1;
}

/* 
 * Remove and interrupt handler
 *
 * You should only have to add the code to mask the interrupt.
 *
 */
int BSP_remove_rtems_irq_handler  (const rtems_irq_connect_data* irq)
{
    rtems_interrupt_level level;
    rtems_irq_hdl *bsp_tbl;

    bsp_tbl = &bsp_vector_table;
  
    if (!isValidInterrupt(irq->name)) {
      return 0;
    }
    /*
     * Check if the handler is actually connected. If not issue an error.
     */
    if (bsp_tbl[irq->name] != irq->hdl) {
      return 0;
    }

    _CPU_ISR_Disable(level);


    /*
     * Disable interrupt on device
     */
    if(irq->off) {
        irq->off(irq);
    }
    /*
     * restore the default irq value
     */
    bsp_tbl[irq->name] = default_int_handler;
    

    _CPU_ISR_Enable(level);

    return 1;
}


