/*  opbintctrl.h
 *
 *  This file contains definitions and declarations for the
 *  Xilinx Off Processor Bus (OPB) Interrupt Controller
 *
 *  Author: Keith Robertson <kjrobert@alumni.uwaterloo.ca>
 *  COPYRIGHT (c) 2005 by Linn Products Ltd, Scotland
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _INCLUDE_OPBINTCTRL_H
#define _INCLUDE_OPBINTCTRL_H

#include <rtems.h>
#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/irq.h>
#include <bspopts.h>
#include RTEMS_XPARAMETERS_H

#define USE_GREG_INTERRUPTS

#ifdef __cplusplus
extern "C" {
#endif


/* extern XIntc InterruptController;
 */


/* Maximum number of IRQs.  Defined in vhdl model */
#define OPB_INTC_IRQ_MAX	  XPAR_INTC_MAX_NUM_INTR_INPUTS

/* Width of INTC registers.  Defined in vhdl model */
#define OPB_INTC_REGISTER_WIDTH   32

/* Base Register address and register offsets.  Defined in vhdl model  */
#define OPB_INTC_BASE		  XPAR_INTC_BASEADDR





/* Interrupt Status Register */
#define OPB_INTC_ISR		0x0
/* Interrupt Pending Register (ISR && IER) */
#define OPB_INTC_IPR		0x4
/* Interrupt Enable Register */
#define OPB_INTC_IER		0x8
/* Interrupt Acknowledge Register */
#define OPB_INTC_IAR		0xC
/* Set Interrupt Enable (same as read/mask/write to IER) */
#define OPB_INTC_SIE		0x10
/* Clear Interrupt Enable (same as read/mask/write to IER) */
#define OPB_INTC_CIE		0x14
/* Interrupt Vector Address (highest priority vector number from IPR) */
#define OPB_INTC_IVR		0x18
/* Master Enable Register */
#define OPB_INTC_MER		0x1C

/* Master Enable Register: Hardware Interrupt Enable */
#define OPB_INTC_MER_HIE	0x2

/* Master Enable Register: Master IRQ Enable */
#define OPB_INTC_MER_ME		0x1

  /*
   * make this fast: is this a opbintc interrupt?
   */
  void BSP_irq_enable_at_opbintc (rtems_irq_number irqnum);

  void BSP_irq_disable_at_opbintc (rtems_irq_number irqnum);
  /*
   *  IRQ Handler: this is called from the primary exception dispatcher
   */
  void BSP_irq_handle_at_opbintc(void);
  /*
   * activate the interrupt controller
   */
  rtems_status_code opb_intc_init(void);

#ifdef __cplusplus
}
#endif

#endif /*  _INCLUDE_OPBINTCTRL_H */
