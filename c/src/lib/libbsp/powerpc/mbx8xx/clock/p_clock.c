/*
 *  Clock Tick interrupt conexion code.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Modified to support the MPC750.
 *  Modifications Copyright (c) 1999 Eric Valette valette@crf.canon.fr
 */

#include <bsp.h>
#include <bsp/irq.h>
#include <rtems/bspIo.h>

extern void clockOn(void*);
extern void clockOff (void*);
extern int clockIsOn(void*);
extern void Clock_isr(void*);

static rtems_irq_connect_data clockIrqData = {BSP_PERIODIC_TIMER,
					      (rtems_irq_hdl)Clock_isr,
                                              0,
					      (rtems_irq_enable)clockOn,
					      (rtems_irq_disable)clockOff,
					      (rtems_irq_is_enabled)clockIsOn};

int BSP_get_clock_irq_level(void)
{
  /*
   * Caution : if you change this, you must change the
   * definition of BSP_PERIODIC_TIMER accordingly
   */
  return 6;
}

int BSP_disconnect_clock_handler (void)
{
  if (!BSP_get_current_rtems_irq_handler(&clockIrqData)) {
     printk("Unable to stop system clock\n");
    rtems_fatal_error_occurred(1);
  }
  return BSP_remove_rtems_irq_handler (&clockIrqData);
}

int BSP_connect_clock_handler (rtems_irq_hdl hdl)
{
  if (!BSP_get_current_rtems_irq_handler(&clockIrqData)) {
     printk("Unable to get system clock handler\n");
    rtems_fatal_error_occurred(1);
  }
  if (!BSP_remove_rtems_irq_handler (&clockIrqData)) {
   printk("Unable to remove current system clock handler\n");
    rtems_fatal_error_occurred(1);
  }
  /*
   * Reinit structure
   */
  clockIrqData.name = BSP_PERIODIC_TIMER;
  clockIrqData.hdl = (rtems_irq_hdl) hdl;
  clockIrqData.on = (rtems_irq_enable)clockOn;
  clockIrqData.off = (rtems_irq_enable)clockOff;
  clockIrqData.isOn = (rtems_irq_is_enabled)clockIsOn;

  return BSP_install_rtems_irq_handler (&clockIrqData);
}
