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
#include <rtems/timecounter.h>

/* The number of clock cycles before generating a tick timer interrupt. */
#define TTMR_NUM_OF_CLOCK_TICKS_INTERRUPT     0x09ED9
#define OR1K_CLOCK_CYCLE_TIME_NANOSECONDS     10

static struct timecounter or1ksim_tc;

/* CPU counter */
static CPU_Counter_ticks cpu_counter_ticks;

static void generic_or1k_clock_at_tick(void)
{
  uint32_t TTMR;

 /* For TTMR register,
  * The least significant 28 bits are the number of clock cycles
  * before generating a tick timer interrupt. While the most
  * significant 4 bits are used for mode configuration, tick timer
  * interrupt enable and pending interrupts status.
  */
  TTMR = (CPU_OR1K_SPR_TTMR_MODE_RESTART | CPU_OR1K_SPR_TTMR_IE |
           (TTMR_NUM_OF_CLOCK_TICKS_INTERRUPT & CPU_OR1K_SPR_TTMR_TP_MASK)
         ) & ~(CPU_OR1K_SPR_TTMR_IP);

  _OR1K_mtspr(CPU_OR1K_SPR_TTMR, TTMR);
  _OR1K_mtspr(CPU_OR1K_SPR_TTCR, 0);

  cpu_counter_ticks += TTMR_NUM_OF_CLOCK_TICKS_INTERRUPT;
}

static void generic_or1k_clock_handler_install(CPU_ISR_handler new_isr)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  _CPU_ISR_install_vector(OR1K_EXCEPTION_TICK_TIMER,
                          new_isr,
                          NULL);

  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }
}

static uint32_t or1ksim_get_timecount(struct timecounter *tc)
{
  (void) tc;

  uint32_t ticks_since_last_timer_interrupt;

  ticks_since_last_timer_interrupt = _OR1K_mfspr(CPU_OR1K_SPR_TTCR);

  return cpu_counter_ticks + ticks_since_last_timer_interrupt;
}

CPU_Counter_ticks _CPU_Counter_read(void)
{
  return or1ksim_get_timecount(NULL);
}

static void generic_or1k_clock_initialize(void)
{
  uint64_t frequency = (1000000000 / OR1K_CLOCK_CYCLE_TIME_NANOSECONDS);
  uint32_t TTMR;

 /* For TTMR register,
  * The least significant 28 bits are the number of clock cycles
  * before generating a tick timer interrupt. While the most
  * significant 4 bits are used for mode configuration, tick timer
  * interrupt enable and pending interrupts status.
  */

  /* FIXME: Long interval should pass since initializing the tick timer
   * registers fires exceptions dispite interrupts has not been enabled yet.
   */
  TTMR = (CPU_OR1K_SPR_TTMR_MODE_RESTART | CPU_OR1K_SPR_TTMR_IE |
           (0xFFED9 & CPU_OR1K_SPR_TTMR_TP_MASK)
         ) & ~(CPU_OR1K_SPR_TTMR_IP);

  _OR1K_mtspr(CPU_OR1K_SPR_TTMR, TTMR);
  _OR1K_mtspr(CPU_OR1K_SPR_TTCR, 0);

  /* Initialize timecounter */
  or1ksim_tc.tc_get_timecount = or1ksim_get_timecount;
  or1ksim_tc.tc_counter_mask = 0xffffffff;
  or1ksim_tc.tc_frequency = frequency;
  or1ksim_tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(&or1ksim_tc);
}

#define Clock_driver_support_at_tick(arg) generic_or1k_clock_at_tick()

#define Clock_driver_support_initialize_hardware() generic_or1k_clock_initialize()

#define Clock_driver_support_install_isr(isr) \
  generic_or1k_clock_handler_install((CPU_ISR_handler) isr)

#include "../../../shared/dev/clock/clockimpl.h"
