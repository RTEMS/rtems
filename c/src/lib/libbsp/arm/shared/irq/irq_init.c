/* irq_init.c
 *
 *  This file contains the implementation of rtems initialization
 *  related to interrupt handling.
 *
 *  CopyRight (C) 2000 Canon Research Centre France SA.
 *  Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <irq.h>
#include <bsp.h>
#include <rtems/bspIo.h>

/*
 * default int vector
 */
extern void _ISR_Handler(void);

void default_int_handler(void)
{
  printk("raw_idt_notify has been called \n");
}

void  rtems_irq_mngt_init(void)
{
    rtems_interrupt_level       level;

    rtems_interrupt_disable(level);

    /* First, connect the ISR_Handler for IRQ and FIQ interrupts */
    _CPU_ISR_install_vector(ARM_EXCEPTION_IRQ, _ISR_Handler, NULL);
    _CPU_ISR_install_vector(ARM_EXCEPTION_FIQ, _ISR_Handler, NULL);

    /* Initialize the INT at the BSP level */
    BSP_rtems_irq_mngt_init();
}
