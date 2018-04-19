/**
 * @file
 *
 * @ingroup bsp_clock
 *
 * @brief riscv clock support.
 */

/*
 * riscv_generic Clock driver
 *
 * COPYRIGHT (c) 2015 Hesham Alatary <hesham@alumni.york.ac.uk>
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <rtems.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <rtems/score/riscv-utility.h>
#include <rtems/score/cpu.h>
#include <rtems/timecounter.h>

/* The number of clock cycles before generating a tick timer interrupt. */
#define TTMR_NUM_OF_CLOCK_TICKS_INTERRUPT     1000
#define RISCV_CLOCK_CYCLE_TIME_NANOSECONDS    1

static struct timecounter riscv_generic_tc;

/* CPU counter */
static CPU_Counter_ticks cpu_counter_ticks;

/* This prototype is added here to Avoid warnings */
void Clock_isr(void *arg);

static void riscv_generic_clock_at_tick(void)
{
  REG(MTIME_MM) = 0;
  REG(MTIMECMP_MM) = TTMR_NUM_OF_CLOCK_TICKS_INTERRUPT;

  cpu_counter_ticks += TTMR_NUM_OF_CLOCK_TICKS_INTERRUPT * 10000;
}

static void riscv_generic_clock_handler_install(proc_ptr new_isr)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  _CPU_ISR_install_vector(RISCV_MACHINE_TIMER_INTERRUPT,
                          new_isr,
                          NULL);

  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }
}

static uint32_t riscv_generic_get_timecount(struct timecounter *tc)
{
  uint32_t ticks_since_last_timer_interrupt = REG(MTIME_MM);

  return cpu_counter_ticks + ticks_since_last_timer_interrupt;
}

CPU_Counter_ticks _CPU_Counter_read(void)
{
  return riscv_generic_get_timecount(NULL);
}

static void riscv_generic_clock_initialize(void)
{
  uint32_t mtimecmp = TTMR_NUM_OF_CLOCK_TICKS_INTERRUPT;
  uint64_t frequency = (1000000000 / RISCV_CLOCK_CYCLE_TIME_NANOSECONDS);

  REG(MTIME_MM) = 0;
  REG(MTIMECMP_MM) = TTMR_NUM_OF_CLOCK_TICKS_INTERRUPT;

  /* Enable mtimer interrupts */
  set_csr(mie, MIP_MTIP);
  set_csr(mip, MIP_MTIP);

  /* Initialize timecounter */
  riscv_generic_tc.tc_get_timecount = riscv_generic_get_timecount;
  riscv_generic_tc.tc_counter_mask = 0xffffffff;
  riscv_generic_tc.tc_frequency = frequency;
  riscv_generic_tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(&riscv_generic_tc);
}

static void riscv_generic_clock_cleanup(void)
{
  /* Disable mtimer interrupts */
  clear_csr(mie, MIP_MTIP);
  clear_csr(mip, MIP_MTIP);
}

CPU_Counter_ticks _CPU_Counter_difference(
  CPU_Counter_ticks second,
  CPU_Counter_ticks first
)
{
  return second - first;
}

#define Clock_driver_support_at_tick() riscv_generic_clock_at_tick()

#define Clock_driver_support_initialize_hardware() riscv_generic_clock_initialize()

#define Clock_driver_support_install_isr(isr) \
  riscv_generic_clock_handler_install(isr)

#define Clock_driver_support_shutdown_hardware() riscv_generic_clock_cleanup()

#include "../../../shared/dev/clock/clockimpl.h"
