/**
 *  @file irq.c
 *
 *  This file contains the implementation of the function described in irq.h.
 */
/*
 *  RTEMS GBA BSP
 *
 *  Copyright (c) 2002 by Jay Monkman <jtm@smoothsmoothie.com>
 *
 *  Copyright (c) 2002 by Charlie Steader <charlies@poliac.com>
 *
 *  Copyright (c) 2004 by Markku Puro <markku.puro@kopteri.net>
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>
#include <irq.h>
#include <gba_registers.h>
#include <rtems/score/thread.h>
#include <rtems/score/apiext.h>


/**
 *  @brief isValidInterrupt function check that the value given for the irq line is valid.
 *
 *  @param  irq irq number
 *  @return status code TRUE/FALSE (0/1)
 */
static int isValidInterrupt(int irq)
{
  if ( (irq < 0) || (irq > BSP_MAX_INT)) {
     return 0;
  }
  return 1;
}

/*
 * ------------------------ RTEMS Single Irq Handler Mngt Routines ----------------
 */


/**
 *  @brief BSP_install_rtems_irq_handler function install rtems irq handler.
 *
 *  @param  irq irq connect data
 *  @return status code TRUE/FALSE (0/1)
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
    HdlTable = (rtems_irq_hdl *) (uint32_t)VECTOR_TABLE;
    if (*(HdlTable + irq->name) != default_int_handler) {
       return 0;
    }

    rtems_interrupt_disable(level);

    /*
     * store the new handler
     */
    *(HdlTable + irq->name) = irq->hdl;

    /*
     * ack pending interrupt
     */
    GBA_REG_IF |= (1 << (irq->name));

    /*
     * initialize the control register for the concerned interrupt
     */
    GBA_REG_IE |= (1 << (irq->name));

    /*
     * Enable interrupt on device
     */
	if (irq->on)
    	irq->on(irq);

    rtems_interrupt_enable(level);

    return 1;
}

/**
 *  @brief BSP_remove_rtems_irq_handler function removes rtems irq handler.
 *
 *  @param  irq irq connect data
 *  @return status code TRUE/FALSE (0/1)
 */
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
    HdlTable = (rtems_irq_hdl *) (uint32_t)VECTOR_TABLE;
    if (*(HdlTable + irq->name) != irq->hdl) {
       return 0;
    }
    rtems_interrupt_disable(level);

    /*
     * mask at INT controller level
     */
    GBA_REG_IE &= ~(1 << irq->name);

    /*
     * Disable interrupt on device
     */
	if (irq->off)
    	irq->off(irq);

    /*
     * restore the default irq value
     */
    *(HdlTable + irq->name) = default_int_handler;

    rtems_interrupt_enable(level);

    return 1;
}


/**
 *  @brief _ThreadProcessSignalsFromIrq function check that the value given for the irq line is valid.
 *
 *  @param  cxt exeption frame
 *  @return None
 */
void _ThreadProcessSignalsFromIrq (CPU_Exception_frame* ctx)
{
  /*
   * Process pending signals that have not already been
   * processed by _Thread_Dispatch. This happens quite
   * unfrequently : the ISR must have posted an action
   * to the current running thread.
   */
  if ( _Thread_Do_post_task_switch_extension ||
       _Thread_Executing->do_post_task_switch_extension )
  {
     _Thread_Executing->do_post_task_switch_extension = FALSE;
     _API_extensions_Run_postswitch();
  }
}
