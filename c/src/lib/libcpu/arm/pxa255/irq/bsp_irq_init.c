/*
 * PXA255 interrupt controller by Yang Xi <hiyangxi@gmail.com>
 * Copyright (c) 2004 by Jay Monkman <jtm@lopgindog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <irq.h>
#include <bsp.h>
#include <pxa255.h>

void dummy_handler(rtems_irq_hdl_param unused)
{
  printk("I am dummy handler\n");
}

void (*IRQ_table[PRIMARY_IRQS])(rtems_irq_hdl_param param);

/*
 * Interrupt system initialization. Disable interrupts, clear
 * any that are pending.
 */
void BSP_rtems_irq_mngt_init(void)
{
  int i;

  /* Initialize the vector table contents with default handler */
  for (i=0; i<PRIMARY_IRQS; i++) {
    IRQ_table[i] = dummy_handler;
  }

  /* disable all interrupts */
  XSCALE_INT_ICMR = 0x0;

  /* Direct the interrupt to IRQ*/
  XSCALE_INT_ICLR = 0x0;
}

