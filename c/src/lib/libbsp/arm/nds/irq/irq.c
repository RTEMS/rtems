/*
 * RTEMS for Nintendo DS interrupt manager.
 *
 * Copyright (c) 2008 by Matthieu Bucchianeri <mbucchia@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE
 */

#include <bsp.h>
#include <rtems/irq.h>
#include <nds.h>

/*
 * this function check that the value given for the irq line is valid.
 */

static int
isValidInterrupt (int irq)
{
  if (irq < 0 || irq >= MAX_INTERRUPTS)
    return 0;
  return 1;
}

/*
 * initialize the irq management.
 */

void
BSP_rtems_irq_mngt_init (void)
{
  irqInit ();

  REG_IME = IME_ENABLE;
}

/*
 * install a irq handler.
 */

int
BSP_install_rtems_irq_handler (const rtems_irq_connect_data * irq)
{
  rtems_interrupt_level level;

  if (!isValidInterrupt (irq->name))
    return 0;

  rtems_interrupt_disable (level);

  /*
   * FIXME: irq_hdl will probably not be called with its parameter
   */
  irqSet (irq->name, (VoidFunctionPointer)irq->hdl);

  if (irq->on != NULL)
    irq->on (irq);

  rtems_interrupt_enable (level);

  return 1;
}

/*
 * return the handler hooked to the given irq.
 */

int
BSP_get_current_rtems_irq_handler (rtems_irq_connect_data * irq)
{
  return 0;                     /* FIXME */
}

/*
 * remove & disable given irq.
 */

int
BSP_remove_rtems_irq_handler (const rtems_irq_connect_data * irq)
{
  rtems_interrupt_level level;

  if (!isValidInterrupt (irq->name))
    return 0;

  rtems_interrupt_disable (level);

  if (irq->off != NULL)
    irq->off (irq);

  irqClear (irq->name);

  rtems_interrupt_enable (level);

  return 1;
}
