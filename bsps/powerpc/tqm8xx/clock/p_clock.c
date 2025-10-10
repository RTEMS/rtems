/* SPDX-License-Identifier: GPL-2.0+-with-RTEMS-exception */

/*
 *  Clock Tick interrupt conexion code.
 */

/*
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  Modified to support the MPC750.
 *  Modifications Copyright (c) 1999 Eric Valette eric.valette@free.fr
 */

#include <bsp.h>
#include <bsp/irq-generic.h>
#include <rtems/bspIo.h>

extern void clockOn(void*);
extern void clockOff (void*);
extern int clockIsOn(void*);
extern void Clock_isr(void*);

static void BSP_clock_hdl(void * arg)
{
  Clock_isr(arg);
}

int BSP_disconnect_clock_handler (void)
{
  rtems_status_code sc;

  clockOff(NULL);
  /*
   * remove interrupt handler
   */
  sc = rtems_interrupt_handler_remove(BSP_PERIODIC_TIMER,
				      BSP_clock_hdl,NULL);

  return sc == RTEMS_SUCCESSFUL;
}

int BSP_connect_clock_handler (rtems_irq_hdl hdl)
{
  (void) hdl;

  rtems_status_code sc;
  /*
   * install interrupt handler
   */
  sc = rtems_interrupt_handler_install(BSP_PERIODIC_TIMER,
				       "PIT clock",0,
				       BSP_clock_hdl,NULL);
  if (sc == RTEMS_SUCCESSFUL) {
    clockOn(NULL);
  }
  return sc == RTEMS_SUCCESSFUL;
}
