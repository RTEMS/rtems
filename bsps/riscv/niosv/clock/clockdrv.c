/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2024 Kevin Kirspel
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

#include <stdlib.h>
#include <bsp/fatal.h>
#include <bsp/irq.h>
#include <bsp/niosv.h>

#include <rtems/sysinit.h>
#include <rtems/counter.h>
#include <rtems/timecounter.h>
#include <rtems/btimer.h>
#include <rtems/score/riscv-utility.h>

#define BENCHMARK_TIMER_INTERVAL_FREQ 1000000

/* This is defined in dev/clock/clockimpl.h */
void Clock_isr(void *arg);

static volatile uint64_t interval_period;
static volatile uint64_t last_mtimecmp;
static volatile uint64_t benchmark_timer_base;

static struct timecounter niosv_clock_tc;

static uint64_t niosv_mtime_get(void)
{
  uint32_t lowbits;
  uint32_t highbits;

  /* Guard against rollover while acquiring each word */
  do {
    highbits = CLOCK_REGS->mtime_hi;
    lowbits = CLOCK_REGS->mtime_lo;
  } while (CLOCK_REGS->mtime_hi != highbits);

  return (((uint64_t)highbits) << 32) | lowbits;
}

static void niosv_mtimecmp_set(uint64_t next_time)
{
  /*
   * Make sure to set the high word to a max value first to prevent triggering
   * inadvertently
   */
  CLOCK_REGS->mtimecmp_hi = 0xFFFFFFFF;
  CLOCK_REGS->mtimecmp_lo = (uint32_t)next_time;
  CLOCK_REGS->mtimecmp_hi = (uint32_t)(next_time >> 32);
  last_mtimecmp = next_time;
}

static void niosv_clock_exit(void)
{
  clear_csr(mie, MIP_MTIP);
}

static uint32_t niosv_timer_read( void )
{
  /* Write to request snapshot of timer value */
  TIMER_REGS->snap = 0;

  return (0xFFFFFFFF - TIMER_REGS->snap);
}

static uint32_t niosv_tc_get_timecount(struct timecounter *tc)
{
  (void) tc;

  return niosv_timer_read();
}

static void niosv_clock_initialize(void)
{
  uint64_t current_time;

  /* Register the driver exit procedure so we can shutdown */
  atexit(niosv_clock_exit);

  interval_period = (((uint64_t)CLOCK_FREQ) / 1000000) *
    (uint64_t)rtems_configuration_get_microseconds_per_tick();
  niosv_mtimecmp_set(MTIMECMP_MAX_VALUE);

  set_csr(mie, MIP_MTIP);

  current_time = niosv_mtime_get();
  niosv_mtimecmp_set(current_time + interval_period);

  /* Install timecounter */
  niosv_clock_tc.tc_get_timecount = niosv_tc_get_timecount;
  niosv_clock_tc.tc_counter_mask = 0xffffffff;
  niosv_clock_tc.tc_frequency = BENCHMARK_TIMER_INTERVAL_FREQ;
  niosv_clock_tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(&niosv_clock_tc);
}

static void niosv_clock_at_tick(void)
{
  niosv_mtimecmp_set(last_mtimecmp + interval_period);
}

static void niosv_clock_handler_install(void)
{
  rtems_status_code sc;

  sc = rtems_interrupt_handler_install(
    NIOSV_INTERRUPT_VECTOR_TIMER,
    "Clock",
    RTEMS_INTERRUPT_UNIQUE,
    (rtems_interrupt_handler) Clock_isr,
    NULL
  );
  if (sc != RTEMS_SUCCESSFUL) {
    bsp_fatal(RISCV_FATAL_CLOCK_IRQ_INSTALL);
  }
}

static void niosv_counter_initialize( void )
{
  /* Disable timer interrupt, stop timer */
  TIMER_REGS->control = ALTERA_AVALON_TIMER_CONTROL_STOP_MSK;

  /* set period to max value, running timer */
  TIMER_REGS->period = 0xFFFFFFFF;

  /* set prescalar for 1us timer */
  TIMER_REGS->prescalar = (
    BENCHMARK_TIMER_FREQ / BENCHMARK_TIMER_INTERVAL_FREQ
  ) - 1;

  /* For timers that can be stopped, writing to periodl/h
     also stopped the timer and we have to manually start it. */

  TIMER_REGS->control = ALTERA_AVALON_TIMER_CONTROL_CONT_MSK |
                        ALTERA_AVALON_TIMER_CONTROL_START_MSK;
}

void benchmark_timer_initialize(void)
{
  benchmark_timer_base = niosv_timer_read();
}

benchmark_timer_t benchmark_timer_read(void)
{
  uint32_t timer_snap = niosv_timer_read();

  /* Check for wrap around */
  if(benchmark_timer_base < timer_snap) {
    return (timer_snap - benchmark_timer_base);
  }
  return ((0xFFFFFFFF - benchmark_timer_base) + timer_snap + 1);
}

void benchmark_timer_disable_subtracting_average_overhead(
  bool find_average_overhead
)
{
  ( void ) find_average_overhead;
}

uint32_t _CPU_Counter_frequency( void )
{
  return BENCHMARK_TIMER_INTERVAL_FREQ;
}

CPU_Counter_ticks _CPU_Counter_read( void )
{
  return (CPU_Counter_ticks)niosv_timer_read();
}

RTEMS_SYSINIT_ITEM(
  niosv_counter_initialize,
  RTEMS_SYSINIT_CPU_COUNTER,
  RTEMS_SYSINIT_ORDER_FIRST
);

#define Clock_driver_support_at_tick(arg) niosv_clock_at_tick()

#define Clock_driver_support_initialize_hardware() niosv_clock_initialize()

#define Clock_driver_support_install_isr(isr) niosv_clock_handler_install()

#include "../../../shared/dev/clock/clockimpl.h"

