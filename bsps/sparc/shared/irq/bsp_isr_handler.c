/*
*  COPYRIGHT (c) 2015
*  Cobham Gaisler
*
*  The license and distribution terms for this file may be
*  found in the file LICENSE in this distribution or at
*  http://www.rtems.org/license/LICENSE.
*
*/

#include <rtems.h>
#include <bsp.h>
#include <bsp/irq-generic.h>

static inline void bsp_dispatch_irq(int irq)
{
  bsp_interrupt_handler_entry *e =
    &bsp_interrupt_handler_table[bsp_interrupt_handler_index(irq)];

  while (e != NULL) {
    (*e->handler)(e->arg);
    e = e->next;
  }
}

/* Called directly from IRQ trap handler TRAP[0x10..0x1F] = IRQ[0..15] */
void bsp_isr_handler(rtems_vector_number vector)
{
  int irq = vector - 0x10;

  /* Let BSP fixup and/or handle incomming IRQ */
  irq = bsp_irq_fixup(irq);

  bsp_dispatch_irq(irq);
}
