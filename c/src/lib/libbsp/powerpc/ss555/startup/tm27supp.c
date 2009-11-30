/*
 *  Support routines for TM27
 *
 *  $Id$
 */

#include <bsp.h>

void tm27On()
{
}

void tm27Off()
{
}

int tm27IsOn()
{
  return 1;
}

rtems_irq_connect_data tm27IrqData = {
  CPU_USIU_EXT_IRQ_7,
  (rtems_irq_hdl)0,
  (rtems_irq_enable)tm27On,
  (rtems_irq_disable)tm27Off,
  (rtems_irq_is_enabled)tm27IsOn
};

