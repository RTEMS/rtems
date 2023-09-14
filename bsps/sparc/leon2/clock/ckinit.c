/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @ingroup RTEMSBSPsSPARCLEON2
 * @brief Clock Tick Device Driver
 *
 *  This routine initializes LEON timer 1 which used for the clock tick.
 *
 *  The tick frequency is directly programmed to the configured number of
 *  microseconds per tick.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified for LEON BSP
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research.
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
#include <bspopts.h>
#include <rtems/sysinit.h>
#include <rtems/timecounter.h>
#include <bsp/sparc-counter.h>

extern int CLOCK_SPEED;

#define LEON2_TIMER_1_FREQUENCY 1000000

static struct timecounter leon2_tc;

static void leon2_clock_init( void )
{
  struct timecounter *tc;

  tc = &leon2_tc;
  tc->tc_get_timecount = _SPARC_Get_timecount_clock;
  tc->tc_counter_mask = 0xffffffff;
  tc->tc_frequency = LEON2_TIMER_1_FREQUENCY;
  tc->tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(tc);
}

static void leon2_clock_at_tick( void )
{
  SPARC_Counter *counter;
  rtems_interrupt_level level;

  counter = &_SPARC_Counter;
  rtems_interrupt_local_disable(level);

  LEON_Clear_interrupt( LEON_INTERRUPT_TIMER1 );
  counter->accumulated += counter->interval;

  rtems_interrupt_local_enable(level);
}

static void leon2_clock_initialize_early( void )
{
  SPARC_Counter *counter;

  LEON_REG.Timer_Reload_1 =
    rtems_configuration_get_microseconds_per_tick() - 1;
  LEON_REG.Timer_Control_1 = (
    LEON_REG_TIMER_COUNTER_ENABLE_COUNTING |
      LEON_REG_TIMER_COUNTER_RELOAD_AT_ZERO |
      LEON_REG_TIMER_COUNTER_LOAD_COUNTER
  );

  counter = &_SPARC_Counter;
  counter->read_isr_disabled = _SPARC_Counter_read_clock_isr_disabled;
  counter->read = _SPARC_Counter_read_clock;
  counter->counter_register = &LEON_REG.Timer_Counter_1;
  counter->pending_register = &LEON_REG.Interrupt_Pending;
  counter->pending_mask = UINT32_C(1) << LEON_INTERRUPT_TIMER1;
  counter->accumulated = rtems_configuration_get_microseconds_per_tick();
  counter->interval = rtems_configuration_get_microseconds_per_tick();
}

RTEMS_SYSINIT_ITEM(
  leon2_clock_initialize_early,
  RTEMS_SYSINIT_CPU_COUNTER,
  RTEMS_SYSINIT_ORDER_FIRST
);

uint32_t _CPU_Counter_frequency( void )
{
  return LEON2_TIMER_1_FREQUENCY;
}

#define Clock_driver_support_install_isr( _new ) \
  (void) rtems_interrupt_handler_install( \
    LEON_INTERRUPT_TIMER1, \
    "Clock", \
    RTEMS_INTERRUPT_SHARED, \
    _new, \
    NULL \
  )

#define Clock_driver_support_at_tick() leon2_clock_at_tick()

#define Clock_driver_support_initialize_hardware() leon2_clock_init()

#include "../../../shared/dev/clock/clockimpl.h"

SPARC_COUNTER_DEFINITION;
