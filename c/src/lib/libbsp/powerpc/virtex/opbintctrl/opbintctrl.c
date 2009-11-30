/*  opbintctrl.c
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

#include <bsp/opbintctrl.h>
#include <rtems.h>
#include <rtems/bspIo.h>
#include <bsp/irq.h>
#include <rtems/powerpc/powerpc.h>

/*
 * Acknowledge a mask of interrupts.
 */
RTEMS_INLINE_ROUTINE void set_iar(uint32_t mask)
{
  *((volatile uint32_t *) (OPB_INTC_BASE + OPB_INTC_IAR)) = mask;
}

/*
 * Set IER state.  Used to (dis)enable a mask of vectors.
 * If you only have to do one, use enable/disable_vector.
 */
RTEMS_INLINE_ROUTINE void set_ier(uint32_t mask)
{
  *((volatile uint32_t *) (OPB_INTC_BASE + OPB_INTC_IER)) = mask;
}

/*
 * Retrieve contents of Interrupt Pending Register
 */
RTEMS_INLINE_ROUTINE uint32_t get_ipr(void)
{
  uint32_t c = *((volatile uint32_t *) (OPB_INTC_BASE + OPB_INTC_IPR));
  return c;
}

void BSP_irq_enable_at_opbintc (rtems_irq_number irqnum)
{
  *((volatile uint32_t *) (OPB_INTC_BASE + OPB_INTC_SIE))
    = 1 << (irqnum - BSP_OPBINTC_IRQ_LOWEST_OFFSET);
}

void BSP_irq_disable_at_opbintc (rtems_irq_number irqnum)
{
  *((volatile uint32_t *) (OPB_INTC_BASE + OPB_INTC_CIE))
    = 1 << (irqnum - BSP_OPBINTC_IRQ_LOWEST_OFFSET);
}

/*
 *  IRQ Handler: this is called from the primary exception dispatcher
 */
void BSP_irq_handle_at_opbintc(void)
{
  uint32_t ipr, iprcopy, mask, i, c;
  rtems_irq_connect_data *tbl_entry;
  iprcopy = ipr = get_ipr();

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

      tbl_entry = &BSP_rtems_irq_tbl[i+BSP_OPBINTC_IRQ_LOWEST_OFFSET];
      if (tbl_entry->hdl != NULL) {
        (tbl_entry->hdl) (tbl_entry->handle);
      } else {
        printk("opbintctrl: Spurious interrupt; IPR 0x%08X, vector 0x%x\n\r",
               iprcopy, i);
      }
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
rtems_status_code opb_intc_init(void)
{
  uint32_t i, mask = 0;

  /* mask off all interrupts */
  set_ier(0x0);

  for (i = 0; i < OPB_INTC_IRQ_MAX; i++) {
    mask |= (1 << i);
  }
  printk("opb_intc_init: mask = 0x%x\n", (unsigned) mask);

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

  return RTEMS_SUCCESSFUL;
}

