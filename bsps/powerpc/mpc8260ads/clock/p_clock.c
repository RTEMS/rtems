/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @brief Clock Tick interrupt conexion code.
 */

/*
 *
 * Copyright (c) 1989-1997 On-Line Applications Research Corporation (OAR).
 *
 * Modified to support the MPC750.
 * Copyright (c) 1999 Eric Valette <eric.valette@free.fr>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <bsp.h>
#include <rtems/bspIo.h>
#include <bsp/irq.h>

extern void clockOn(void*);
extern void clockOff (void*);
extern int clockIsOn(void*);
extern void Clock_isr(void);

static rtems_irq_connect_data clockIrqData = {BSP_PERIODIC_TIMER,
					      (rtems_irq_hdl)Clock_isr,
					      NULL,
					      (rtems_irq_enable)clockOn,
					      (rtems_irq_disable)clockOff,
					      (rtems_irq_is_enabled)clockIsOn};

int BSP_get_clock_irq_level(void)
{
  /*
   * Caution : if you change this, you must change the
   * definition of BSP_PERIODIC_TIMER accordingly
   */
  return BSP_PERIODIC_TIMER;
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
