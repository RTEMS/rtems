/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDriverClockImpl
 *
 * @brief This source file contains the implementation of the or1k Clock
 *   Driver.
 */

/*
 * COPYRIGHT (c) 2014-2015 Hesham ALMatary <heshamelmatary@gmail.com>
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

#include <rtems.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/generic_or1k.h>
#include <rtems/score/cpu.h>
#include <rtems/score/or1k-utility.h>
#include <rtems/sysinit.h>
#include <rtems/timecounter.h>

static struct timecounter or1ksim_tc;

/*
 * The number of tick timer counts of one clock tick.  The tick timer counts
 * at OR1K_BSP_TIMER_FREQ.  The time period field of the tick timer mode
 * register is 28 bits wide, which is more than a second at that frequency.
 *
 * This is computed once and not in the tick handler, which must not need a
 * 64-bit division.
 */
static uint32_t generic_or1k_clock_ticks;

CPU_Counter_ticks _CPU_Counter_read(void)
{
  return _OR1K_mfspr(CPU_OR1K_SPR_TTCR);
}

uint32_t _CPU_Counter_frequency(void)
{
  return OR1K_BSP_TIMER_FREQ;
}

static uint32_t or1ksim_get_timecount(struct timecounter *tc)
{
  (void) tc;

  return _CPU_Counter_read();
}

/*
 * Run the tick timer in continue mode, so that its counter is free running.
 * The counter is the CPU counter and the timecounter of this BSP, and a system
 * which needs no clock driver gets it this way as well.  The other modes reset
 * or stop the counter when the time period elapses.
 */
static void generic_or1k_counter_initialize(void)
{
  _OR1K_mtspr(CPU_OR1K_SPR_TTCR, 0);
  _OR1K_mtspr(CPU_OR1K_SPR_TTMR, CPU_OR1K_SPR_TTMR_MODE_CONT);
}

RTEMS_SYSINIT_ITEM(
  generic_or1k_counter_initialize,
  RTEMS_SYSINIT_CPU_COUNTER,
  RTEMS_SYSINIT_ORDER_FIRST
);

/*
 * The number of tick timer counts which the time period must stay ahead of the
 * counter.  The counter runs while the handler computes the time period and
 * writes it.
 */
#define GENERIC_OR1K_TICK_MARGIN 64

/*
 * Set the time period of the next tick.  Writing the mode register with a
 * clear pending interrupt bit acknowledges the interrupt.
 */
static void generic_or1k_set_next_tick(uint32_t next)
{
  _OR1K_mtspr(
    CPU_OR1K_SPR_TTMR,
    CPU_OR1K_SPR_TTMR_MODE_CONT | CPU_OR1K_SPR_TTMR_IE |
      ( next & CPU_OR1K_SPR_TTMR_TP_MASK )
  );
}

/*
 * The timer compares the time period with the low bits of the counter.  A time
 * period which the counter reached matches only after the counter runs through
 * the field once, which takes seconds.  Take the counter as the base of the
 * next tick if the handler was late, so that the clock keeps running.
 */
static void generic_or1k_clock_at_tick(void)
{
  rtems_interrupt_level level;
  uint32_t             next;
  uint32_t             distance;

  rtems_interrupt_local_disable(level);

  next = _OR1K_mfspr(CPU_OR1K_SPR_TTMR) + generic_or1k_clock_ticks;
  distance = ( next - _OR1K_mfspr(CPU_OR1K_SPR_TTCR) ) &
    CPU_OR1K_SPR_TTMR_TP_MASK;

  if (
    distance < GENERIC_OR1K_TICK_MARGIN ||
      distance > CPU_OR1K_SPR_TTMR_TP_MASK / 2
  ) {
    next = _OR1K_mfspr(CPU_OR1K_SPR_TTCR) + generic_or1k_clock_ticks;
  }

  generic_or1k_set_next_tick(next);

  rtems_interrupt_local_enable(level);
}

static void generic_or1k_clock_handler_install(CPU_ISR_handler new_isr)
{
  _CPU_ISR_install_vector(OR1K_EXCEPTION_TICK_TIMER,
                          new_isr,
                          NULL);
}

static void generic_or1k_clock_initialize(void)
{
  uint64_t us_per_tick;

  us_per_tick = rtems_configuration_get_microseconds_per_tick();
  generic_or1k_clock_ticks =
    (uint32_t) ( ( us_per_tick * OR1K_BSP_TIMER_FREQ ) / 1000000 );

  generic_or1k_set_next_tick(
    _OR1K_mfspr(CPU_OR1K_SPR_TTCR) + generic_or1k_clock_ticks
  );

  /* Initialize timecounter */
  or1ksim_tc.tc_get_timecount = or1ksim_get_timecount;
  or1ksim_tc.tc_counter_mask = 0xffffffff;
  or1ksim_tc.tc_frequency = OR1K_BSP_TIMER_FREQ;
  or1ksim_tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(&or1ksim_tc);
}

#define Clock_driver_support_at_tick(arg) generic_or1k_clock_at_tick()

#define Clock_driver_support_initialize_hardware() generic_or1k_clock_initialize()

#define Clock_driver_support_install_isr(isr) \
  generic_or1k_clock_handler_install((CPU_ISR_handler) isr)

#include "../../../shared/dev/clock/clockimpl.h"
