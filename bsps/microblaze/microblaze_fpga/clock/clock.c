/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsMicroblaze
 *
 * @brief MicroBlaze AXI Timer clock support
 */

/*
 * Copyright (C) 2021 On-Line Applications Research Corporation (OAR)
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

#include <bsp/fatal.h>
#include <bsp/timer.h>

#include <rtems.h>
#include <rtems/irq-extension.h>
#include <rtems/timecounter.h>

static rtems_timecounter_simple mblaze_tc;

static uint32_t microblaze_tc_get( rtems_timecounter_simple *tc )
{
  volatile Microblaze_Timer *timer = _Microblaze_Timer;
  return timer->tcr0;
}

static bool microblaze_tc_is_pending( rtems_timecounter_simple *tc )
{
  volatile Microblaze_Timer *timer = _Microblaze_Timer;
  return ( timer->tcsr0 & MICROBLAZE_TIMER_TCSR0_T0INT ) != 0;
}

static uint32_t microblaze_tc_get_timecount( struct timecounter *tc )
{
  return rtems_timecounter_simple_downcounter_get(
    tc,
    microblaze_tc_get,
    microblaze_tc_is_pending
  );
}

static void microblaze_clock_initialize( void )
{
  volatile Microblaze_Timer *timer = _Microblaze_Timer;
  /* Set load register to 0 */
  timer->tlr0 = 0;
  /* Reset the timer and interrupt */
  timer->tcsr0 = MICROBLAZE_TIMER_TCSR0_T0INT | MICROBLAZE_TIMER_TCSR0_LOAD0;
  /* Release the reset */
  timer->tcsr0 = 0;
  /*
   * Enable interrupt, auto reload mode, external interrupt signal,
   * and down counter
   */
  timer->tcsr0 =  MICROBLAZE_TIMER_TCSR0_ARHT0 | MICROBLAZE_TIMER_TCSR0_ENIT0 |
                  MICROBLAZE_TIMER_TCSR0_GENT0 | MICROBLAZE_TIMER_TCSR0_UDT0;

  uint64_t us_per_tick = rtems_configuration_get_microseconds_per_tick();
  uint32_t counter_frequency_in_hz = BSP_MICROBLAZE_FPGA_TIMER_FREQUENCY;
  uint32_t counter_ticks_per_clock_tick =
    ( counter_frequency_in_hz * us_per_tick ) / 1000000;

  /* Set a reset value for the timer counter */
  timer->tlr0 = counter_ticks_per_clock_tick;
  uint32_t control_status_reg = timer->tcsr0;
  /* Load the reset value into the counter register */
  timer->tcsr0 = MICROBLAZE_TIMER_TCSR0_LOAD0;
  /* Enable the timer */
  timer->tcsr0 = control_status_reg | MICROBLAZE_TIMER_TCSR0_ENT0;

  rtems_timecounter_simple_install(
    &mblaze_tc,
    counter_frequency_in_hz,
    counter_ticks_per_clock_tick,
    microblaze_tc_get_timecount
  );
}

static void microblaze_clock_at_tick( rtems_timecounter_simple *tc )
{
  volatile Microblaze_Timer *timer = _Microblaze_Timer;
  if ( ( timer->tcsr0 & MICROBLAZE_TIMER_TCSR0_T0INT ) == 0 ) {
    return;
  }
  /* Clear the interrupt */
  timer->tcsr0 |= MICROBLAZE_TIMER_TCSR0_T0INT;
}

static void microblaze_tc_tick( void )
{
  rtems_timecounter_simple_downcounter_tick(
    &mblaze_tc,
    microblaze_tc_get,
    microblaze_clock_at_tick
  );
}

static void microblaze_clock_handler_install( rtems_interrupt_handler isr )
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_interrupt_handler_install(
    0,
    "Clock",
    RTEMS_INTERRUPT_UNIQUE,
    isr,
    NULL
  );

  if ( sc != RTEMS_SUCCESSFUL ) {
    bsp_fatal( MICROBLAZE_FATAL_CLOCK_IRQ_INSTALL );
  }
}

#define Clock_driver_support_initialize_hardware() microblaze_clock_initialize()
#define Clock_driver_support_install_isr( isr ) \
  microblaze_clock_handler_install( isr )
#define Clock_driver_timecounter_tick() microblaze_tc_tick()

/* Include shared source clock driver code */
#include "../../shared/dev/clock/clockimpl.h"
