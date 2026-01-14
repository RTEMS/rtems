/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @brief Clock Tick interrupt connection code.
 */

/*
 * Copyright (c) 1989-1997. On-Line Applications Research Corporation (OAR).
 *
 * Modified to support the MPC750.
 * Modifications Copyright (c) 1999 Eric Valette <eric.valette@free.fr>
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
