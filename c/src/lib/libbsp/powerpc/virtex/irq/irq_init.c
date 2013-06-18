/*===============================================================*\
| Project: RTEMS virtex BSP                                       |
+-----------------------------------------------------------------+
| Partially based on the code references which are named below.   |
| Adaptions, modifications, enhancements and any recent parts of  |
| the code are:                                                   |
|                    Copyright (c) 2007                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains the irq controller handler                   |
\*===============================================================*/

/*  Content moved from opbintctrl.c:
 *
 *  This file contains definitions and declarations for the
 *  Xilinx Off Processor Bus (OPB) Interrupt Controller
 *
 *  Author: Keith Robertson <kjrobert@alumni.uwaterloo.ca>
 *  COPYRIGHT (c) 2005 Linn Products Ltd, Scotland.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/vectors.h>

/*
 * Acknowledge a mask of interrupts.
 */
static void set_iar(uint32_t mask)
{
  *((volatile uint32_t *) (OPB_INTC_BASE + OPB_INTC_IAR)) = mask;
}

/*
 * Set IER state.  Used to (dis)enable a mask of vectors.
 * If you only have to do one, use enable/disable_vector.
 */
static void set_ier(uint32_t mask)
{
  *((volatile uint32_t *) (OPB_INTC_BASE + OPB_INTC_IER)) = mask;
}

/*
 * Retrieve contents of Interrupt Pending Register
 */
static uint32_t get_ipr(void)
{
  uint32_t c = *((volatile uint32_t *) (OPB_INTC_BASE + OPB_INTC_IPR));
  return c;
}

static void BSP_irq_enable_at_opbintc (rtems_irq_number irqnum)
{
  *((volatile uint32_t *) (OPB_INTC_BASE + OPB_INTC_SIE))
    = 1 << (irqnum - BSP_OPBINTC_IRQ_LOWEST_OFFSET);
}

static void BSP_irq_disable_at_opbintc (rtems_irq_number irqnum)
{
  *((volatile uint32_t *) (OPB_INTC_BASE + OPB_INTC_CIE))
    = 1 << (irqnum - BSP_OPBINTC_IRQ_LOWEST_OFFSET);
}

/*
 *  IRQ Handler: this is called from the primary exception dispatcher
 */
static void BSP_irq_handle_at_opbintc(void)
{
  uint32_t ipr, mask, i, c;
  ipr = get_ipr();

  c = 0;
  mask = 0;

  for (i = 0;
       (i < BSP_OPBINTC_PER_IRQ_NUMBER)
	 && (ipr != 0);
       i++) {
    c = (1 << i);

    if ((ipr & c) != 0) {
      /* interrupt is asserted */
      mask |= c;
      ipr &= ~c;

      bsp_interrupt_handler_dispatch(i+BSP_OPBINTC_IRQ_LOWEST_OFFSET);
    }
  }

  if (mask) {
    /* ack all the interrupts we serviced */
    set_iar(mask);
  }
}

/*
 * activate the interrupt controller
 */
static void opb_intc_init(void)
{
  uint32_t i, mask = 0;

  /* mask off all interrupts */
  set_ier(0x0);

  for (i = 0; i < OPB_INTC_IRQ_MAX; i++) {
    mask |= (1 << i);
  }

  /* make sure interupt status register is clear before we enable the interrupt controller */
  *((volatile uint32_t *) (OPB_INTC_BASE + OPB_INTC_ISR)) = 0;

  /* acknowledge all interrupt sources */
  set_iar(mask);

  /* Turn on normal hardware operation of interrupt controller */
  *((volatile uint32_t *) (OPB_INTC_BASE + OPB_INTC_MER)) =
    (OPB_INTC_MER_HIE);

  /* Enable master interrupt switch for the interrupt controller */
  *((volatile uint32_t *) (OPB_INTC_BASE + OPB_INTC_MER)) =
    (OPB_INTC_MER_HIE | OPB_INTC_MER_ME);
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (bsp_interrupt_is_valid_vector(vector)) {
    if (BSP_IS_OPBINTC_IRQ(vector)) {
      BSP_irq_enable_at_opbintc(vector);
    }
  } else {
    sc = RTEMS_INVALID_ID;
  }

  return sc;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (bsp_interrupt_is_valid_vector(vector)) {
    if (BSP_IS_OPBINTC_IRQ(vector)) {
      BSP_irq_disable_at_opbintc(vector);
    }
  } else {
    sc = RTEMS_INVALID_ID;
  }

  return sc;
}

static int C_dispatch_irq_handler(BSP_Exception_frame *frame, unsigned int excNum)
{
  BSP_irq_handle_at_opbintc();

  return 0;
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
  opb_intc_init();

  ppc_exc_set_handler(ASM_EXT_VECTOR, C_dispatch_irq_handler);

  return RTEMS_SUCCESSFUL;
}
