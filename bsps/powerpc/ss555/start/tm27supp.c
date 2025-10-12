/*
 *  Support routines for TM27
 */

#include <bsp.h>

rtems_irq_connect_data tm27IrqData = {
  .name = CPU_USIU_EXT_IRQ_7,
  .hdl = 0,
  .handle = NULL,
  .on = NULL,
  .off = NULL,
  .isOn = NULL
};
