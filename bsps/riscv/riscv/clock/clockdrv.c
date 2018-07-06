/**
 * @file
 *
 * @ingroup bsp_clock
 *
 * @brief riscv clock support.
 */

/*
 * Copyright (c) 2018 embedded brains GmbH
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

#include <rtems/timecounter.h>
#include <rtems/score/riscv-utility.h>

#include <bsp.h>
#include <bsp/fatal.h>
#include <bsp/fdt.h>
#include <bsp/irq.h>

#include <dev/irq/clint.h>

#include <libfdt.h>

#define CLINT ((volatile clint_regs *) 0x02000000)

/* This is defined in dev/clock/clockimpl.h */
void Clock_isr(void *arg);

static struct timecounter riscv_clock_tc;

static uint32_t riscv_clock_interval;

static void riscv_clock_at_tick(void)
{
  volatile clint_regs *clint;
  uint64_t cmp;

  clint = CLINT;

  cmp = clint->mtimecmp[0].val_64;
  cmp += riscv_clock_interval;

#if __riscv_xlen == 32
  clint->mtimecmp[0].val_32[0] = 0xffffffff;
  clint->mtimecmp[0].val_32[1] = (uint32_t) (cmp >> 32);
  clint->mtimecmp[0].val_32[0] = (uint32_t) cmp;
#elif __riscv_xlen == 64
  clint->mtimecmp[0].val_64 = cmp;
#endif
}

static void riscv_clock_handler_install(proc_ptr new_isr)
{
  _CPU_ISR_install_vector(RISCV_MACHINE_TIMER_INTERRUPT,
                          new_isr,
                          NULL);
}

static uint32_t riscv_clock_get_timecount(struct timecounter *tc)
{
  volatile clint_regs *clint;

  clint = CLINT;
  return clint->mtime.val_32[0];
}

static uint32_t riscv_clock_get_timebase_frequency(const void *fdt)
{
  int node;
  const uint32_t *val;
  int len;

  node = fdt_path_offset(fdt, "/cpus");
  val = fdt_getprop(fdt, node, "timebase-frequency", &len);
  if (val == NULL || len < 4) {
    bsp_fatal(RISCV_FATAL_NO_TIMEBASE_FREQUENCY_IN_DEVICE_TREE);
  }

  return fdt32_to_cpu(*val);
}

static void riscv_clock_initialize(void)
{
  const char *fdt;
  uint32_t tb_freq;
  uint64_t us_per_tick;

  fdt = bsp_fdt_get();
  tb_freq = riscv_clock_get_timebase_frequency(fdt);
  us_per_tick = rtems_configuration_get_microseconds_per_tick();
  riscv_clock_interval = (uint32_t) ((tb_freq * us_per_tick) / 1000000);

  riscv_clock_at_tick();

  /* Enable mtimer interrupts */
  set_csr(mie, MIP_MTIP);

  /* Initialize timecounter */
  riscv_clock_tc.tc_get_timecount = riscv_clock_get_timecount;
  riscv_clock_tc.tc_counter_mask = 0xffffffff;
  riscv_clock_tc.tc_frequency = tb_freq;
  riscv_clock_tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(&riscv_clock_tc);
}

uint32_t _CPU_Counter_frequency( void )
{
  return riscv_clock_get_timebase_frequency(bsp_fdt_get());
}

#define Clock_driver_support_at_tick() riscv_clock_at_tick()

#define Clock_driver_support_initialize_hardware() riscv_clock_initialize()

#define Clock_driver_support_install_isr(isr) \
  riscv_clock_handler_install(isr)

#include "../../../shared/dev/clock/clockimpl.h"
