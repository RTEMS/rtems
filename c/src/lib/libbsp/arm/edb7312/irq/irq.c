/*
 * Cirrus EP7312 Intererrupt handler
 *
 * Copyright (c) 2002 by Jay Monkman <jtm@smoothsmoothie.com>
 *	
 * Copyright (c) 2002 by Charlie Steader <charlies@poliac.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *
 *  $Id$
*/
#include <bsp.h>
#include <irq.h>
#include <rtems/score/thread.h>
#include <rtems/score/apiext.h>
#include <ep7312.h>

/*
 * This function check that the value given for the irq line
 * is valid.
 */

static int isValidInterrupt(int irq)
{
  if ( (irq < 0) || (irq > BSP_MAX_INT))
    return 0;
  return 1;
}

/*
 * -------------------- RTEMS Single Irq Handler Mngt Routines ----------------
 */

int BSP_install_rtems_irq_handler  (const rtems_irq_connect_data* irq)
{
    rtems_irq_hdl *HdlTable;
    rtems_interrupt_level level;
    
    if (!isValidInterrupt(irq->name)) {
      return 0;
    }
    /*
     * Check if default handler is actually connected. If not issue an error.
     */
    HdlTable = (rtems_irq_hdl *) VECTOR_TABLE;
    if (*(HdlTable + irq->name) != default_int_handler) {
      return 0;
    }
    
    _CPU_ISR_Disable(level);

    /*
     * store the new handler
     */
    *(HdlTable + irq->name) = irq->hdl;

    /*
     * unmask interrupt
     */
    if(irq->name >= BSP_EXTFIQ && irq->name <= BSP_SSEOTI)
    {
        /* interrupt managed by INTMR1 and INTSR1 */
        *EP7312_INTMR1 |= (1 << irq->name);
    }
    else if(irq->name >= BSP_KBDINT && irq->name <= BSP_SS2TX)
    {
        /* interrupt managed by INTMR2 and INTSR2 */
        *EP7312_INTMR2 |= (1 << (irq->name - 16));
    }
    else if(irq->name >= BSP_UTXINT2 && irq->name <= BSP_URXINT2)
    {
        /* interrupt managed by INTMR2 and INTSR2 */
        *EP7312_INTMR2 |= (1 << (irq->name - 7));
    }
    else if(irq->name == BSP_DAIINT)
    {
        /* interrupt managed by INTMR3 and INTSR3 */
        *EP7312_INTMR3 |= (1 << (irq->name - 21));
    }
    
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

int BSP_remove_rtems_irq_handler  (const rtems_irq_connect_data* irq)
{
    rtems_irq_hdl *HdlTable;
    rtems_interrupt_level level;
  
    if (!isValidInterrupt(irq->name)) {
      return 0;
    }
    /*
     * Check if the handler is actually connected. If not issue an error.
     */
    HdlTable = (rtems_irq_hdl *) VECTOR_TABLE;
    if (*(HdlTable + irq->name) != irq->hdl) {
      return 0;
    }
    _CPU_ISR_Disable(level);

    /*
     * mask interrupt
     */
    if(irq->name >= BSP_EXTFIQ && irq->name <= BSP_SSEOTI)
    {
        /* interrupt managed by INTMR1 and INTSR1 */
        *EP7312_INTMR1 &= ~(1 << irq->name);
    }
    else if(irq->name >= BSP_KBDINT && irq->name <= BSP_SS2TX)
    {
        /* interrupt managed by INTMR2 and INTSR2 */
        *EP7312_INTMR2 &= ~(1 << (irq->name - 16));
    }
    else if(irq->name >= BSP_UTXINT2 && irq->name <= BSP_URXINT2)
    {
        /* interrupt managed by INTMR2 and INTSR2 */
        *EP7312_INTMR2 &= ~(1 << (irq->name - 7));
    }
    else if(irq->name == BSP_DAIINT)
    {
        /* interrupt managed by INTMR3 and INTSR3 */
        *EP7312_INTMR3 &= ~(1 << (irq->name - 21));
    }
    
    /*
     * Disable interrupt on device
     */
    if(irq->off)
        irq->off(irq);
    
    /*
     * restore the default irq value
     */
    *(HdlTable + irq->name) = default_int_handler;
    
    _CPU_ISR_Enable(level);

    return 1;
}


