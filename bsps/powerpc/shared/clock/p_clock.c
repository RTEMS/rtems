/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @brief Clock Tick interrupt connection code.
 */

/*
 * Copyright (c) 1989-1997.
 * On-Line Applications Research Corporation (OAR).
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
#include <bsp/irq.h>
#include <libcpu/c_clock.h>
#include <libcpu/cpuIdent.h>

static rtems_irq_connect_data clockIrqData;
static rtems_irq_connect_data clockIrqData = {
  .name = BSP_DECREMENTER,
  .hdl = clockIsr,
  .handle = NULL,
  .on = (rtems_irq_enable)clockOn,
  .off = (rtems_irq_disable)clockOff,
  .isOn = (rtems_irq_is_enabled) clockIsOn
};

int BSP_disconnect_clock_handler(void)
{
  return BSP_remove_rtems_irq_handler(&clockIrqData);
}

int BSP_connect_clock_handler(void)
{
  if ( ppc_cpu_is_bookE() )
    clockIrqData.hdl = clockIsrBookE;

  return BSP_install_rtems_irq_handler(&clockIrqData);
}
