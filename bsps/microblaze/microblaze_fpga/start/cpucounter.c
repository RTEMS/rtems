/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsMicroblaze
 *
 * @brief This source file contains the CPU counter support.
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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
#include <bsp/microblaze-fdt-support.h>
#include <bsp/microblaze-timer.h>

#include <rtems/sysinit.h>

/*
 * The device tree gives the address and the frequency of the AXI timer.  The
 * values of the build options stand here until the initialization below reads
 * that tree, so a read before it gets a plausible address.
 */
static volatile Microblaze_Timer *microblaze_counter_timer =
  (volatile Microblaze_Timer *) BSP_MICROBLAZE_FPGA_TIMER_BASE;

static uint32_t
  microblaze_counter_frequency = BSP_MICROBLAZE_FPGA_TIMER_FREQUENCY;

uint32_t _CPU_Counter_frequency( void )
{
  return microblaze_counter_frequency;
}

CPU_Counter_ticks _CPU_Counter_read( void )
{
  return microblaze_counter_timer->tcr1;
}

/*
 * The clock driver and the benchmark timer use the first counter of the AXI
 * timer.  The second counter has no other user outside the tm27 support of
 * the timing tests.  Run it as a free running up counter of the full width:
 * it wraps at the width of the counter, so it needs no interrupt to carry a
 * period and a reader never sees it go backwards.
 */
static void microblaze_counter_initialize( void )
{
  volatile Microblaze_Timer *timer;

  timer = (volatile Microblaze_Timer *) try_get_prop_from_device_tree(
    "xlnx,xps-timer-1.00.a",
    "reg",
    BSP_MICROBLAZE_FPGA_TIMER_BASE
  );
  microblaze_counter_timer = timer;
  microblaze_counter_frequency = try_get_prop_from_device_tree(
    "xlnx,xps-timer-1.00.a",
    "clock-frequency",
    BSP_MICROBLAZE_FPGA_TIMER_FREQUENCY
  );

  timer->tlr1 = 0;

  /* Hold the counter at the load value and clear a pending interrupt */
  timer->tcsr1 = MICROBLAZE_TIMER_TCSR0_T0INT | MICROBLAZE_TIMER_TCSR0_LOAD0;

  /* Release the counter and let it count up with auto reload */
  timer->tcsr1 = MICROBLAZE_TIMER_TCSR0_ARHT0 | MICROBLAZE_TIMER_TCSR0_ENT0;
}

/*
 * getentropy() and the users of rtems_counter_delay_nanoseconds() need no
 * clock driver, so start the counter here rather than in the clock driver.
 */
RTEMS_SYSINIT_ITEM(
  microblaze_counter_initialize,
  RTEMS_SYSINIT_CPU_COUNTER,
  RTEMS_SYSINIT_ORDER_FIRST
);
