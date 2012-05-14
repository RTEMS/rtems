/*
 *  Support routines for TM27
 */

#include <bsp.h>

rtems_irq_connect_data tm27IrqData = {
  CPU_USIU_EXT_IRQ_7,
  (rtems_irq_hdl)0,
  NULL,
  NULL,
  NULL
};

