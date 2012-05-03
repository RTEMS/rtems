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
#include <libcpu/c_clock.h>
#include <libcpu/cpuIdent.h>

static rtems_irq_connect_data clockIrqData;
static rtems_irq_connect_data clockIrqData = {BSP_DECREMENTER,
					      clockIsr,
                                              NULL,
					      (rtems_irq_enable)clockOn,
					      (rtems_irq_disable)clockOff,
					      (rtems_irq_is_enabled) clockIsOn};
int BSP_disconnect_clock_handler (void)
{
  return BSP_remove_rtems_irq_handler (&clockIrqData);
}

int BSP_connect_clock_handler (void)
{
  if ( ppc_cpu_is_bookE() )
  	clockIrqData.hdl = clockIsrBookE;

  return BSP_install_rtems_irq_handler (&clockIrqData);
}
