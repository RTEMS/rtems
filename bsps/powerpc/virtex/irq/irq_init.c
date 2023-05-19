/*
 * RTEMS virtex BSP
 *
 * This file contains the irq controller handler.
 *
 * Content moved from opbintctrl.c:
 *
 *  This file contains definitions and declarations for the
 *  Xilinx Off Processor Bus (OPB) Interrupt Controller
 */

/*
 * Author: Keith Robertson <kjrobert@alumni.uwaterloo.ca>
 * COPYRIGHT (c) 2005 Linn Products Ltd, Scotland.
 * Copyright (c) 2007 embedded brains GmbH & Co. KG
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/vectors.h>

#include <libcpu/powerpc-utility.h>

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
  uint32_t ipr;

  /* Get pending interrupts */
  ipr = get_ipr();

  if (ipr != 0) {
    /* Acknowledge all pending interrupts now and service them afterwards */
    set_iar(ipr);

    do {
      /* Get highest priority pending interrupt */
      uint32_t i = 31 - ppc_count_leading_zeros(ipr);

      ipr &= ~(1U << i);

      bsp_interrupt_handler_dispatch(i+BSP_OPBINTC_IRQ_LOWEST_OFFSET);
    } while (ipr != 0);
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

rtems_status_code bsp_interrupt_get_attributes(
  rtems_vector_number         vector,
  rtems_interrupt_attributes *attributes
)
{
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_is_pending(
  rtems_vector_number vector,
  bool               *pending
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(pending != NULL);
  *pending = false;
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_raise(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_clear(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_vector_is_enabled(
  rtems_vector_number vector,
  bool               *enabled
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(enabled != NULL);
  *enabled = false;
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  if (BSP_IS_OPBINTC_IRQ(vector)) {
    BSP_irq_enable_at_opbintc(vector);
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  if (BSP_IS_OPBINTC_IRQ(vector)) {
    BSP_irq_disable_at_opbintc(vector);
  }

  return RTEMS_SUCCESSFUL;
}

static int C_dispatch_irq_handler(BSP_Exception_frame *frame, unsigned int excNum)
{
  BSP_irq_handle_at_opbintc();

  return 0;
}

void bsp_interrupt_facility_initialize(void)
{
  rtems_status_code sc;

  opb_intc_init();

  sc = ppc_exc_set_handler(ASM_EXT_VECTOR, C_dispatch_irq_handler);
  _Assert_Unused_variable_equals(sc, RTEMS_SUCCESSFUL);
}
