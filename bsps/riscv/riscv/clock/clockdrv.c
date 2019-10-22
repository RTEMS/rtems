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

#include <bsp/fatal.h>
#include <bsp/fdt.h>
#include <bsp/irq.h>
#include <bsp/riscv.h>

#include <rtems/sysinit.h>
#include <rtems/timecounter.h>
#include <rtems/score/cpuimpl.h>
#include <rtems/score/riscv-utility.h>

#include <libfdt.h>

/* This is defined in dev/clock/clockimpl.h */
void Clock_isr(void *arg);

typedef struct {
  struct timecounter base;
  volatile RISCV_CLINT_regs *clint;
  uint32_t interval;
} riscv_timecounter;

static riscv_timecounter riscv_clock_tc;

static void riscv_clock_write_mtimecmp(
  volatile RISCV_CLINT_timer_reg *mtimecmp,
  uint64_t value
)
{
#if __riscv_xlen == 32
  mtimecmp->val_32[0] = 0xffffffff;
  mtimecmp->val_32[1] = (uint32_t) (value >> 32);
  mtimecmp->val_32[0] = (uint32_t) value;
#elif __riscv_xlen == 64
  mtimecmp->val_64 = value;
#endif
}

static uint64_t riscv_clock_read_mtime(volatile RISCV_CLINT_timer_reg *mtime)
{
#if __riscv_xlen == 32
  uint32_t low;
  uint32_t high_0;
  uint32_t high_1;

  do {
    high_0 = mtime->val_32[1];
    low = mtime->val_32[0];
    high_1 = mtime->val_32[1];
  } while (high_0 != high_1);

  return (((uint64_t) high_0) << 32) | low;
#elif __riscv_xlen == 64
  return mtime->val_64;
#endif
}

static void riscv_clock_at_tick(riscv_timecounter *tc)
{
  volatile RISCV_CLINT_regs *clint;
  uint64_t value;

  clint = tc->clint;

  value = clint->mtimecmp[0].val_64;
  value += tc->interval;

  riscv_clock_write_mtimecmp(&clint->mtimecmp[0], value);
}

static void riscv_clock_handler_install(void)
{
  rtems_status_code sc;

  sc = rtems_interrupt_handler_install(
    RISCV_INTERRUPT_VECTOR_TIMER,
    "Clock",
    RTEMS_INTERRUPT_UNIQUE,
    (rtems_interrupt_handler) Clock_isr,
    NULL
  );
  if (sc != RTEMS_SUCCESSFUL) {
    bsp_fatal(RISCV_FATAL_CLOCK_IRQ_INSTALL);
  }
}

static uint32_t riscv_clock_get_timecount(struct timecounter *base)
{
  riscv_timecounter *tc;
  volatile RISCV_CLINT_regs *clint;

  tc = (riscv_timecounter *) base;
  clint = tc->clint;
  return clint->mtime.val_32[0];
}

static uint32_t riscv_clock_get_timebase_frequency(const void *fdt)
{
  int node;
  const fdt32_t *val;
  int len=0;

  node = fdt_path_offset(fdt, "/cpus");

  val = (fdt32_t *) fdt_getprop(fdt, node, "timebase-frequency", &len);

  if (val == NULL || len < 4) {
    int cpu0 = fdt_subnode_offset(fdt, node, "cpu@0");
    val = (fdt32_t *) fdt_getprop(fdt, cpu0, "timebase-frequency", &len);

    if (val == NULL || len < 4) {
      bsp_fatal(RISCV_FATAL_NO_TIMEBASE_FREQUENCY_IN_DEVICE_TREE);
    }
  }
  return fdt32_to_cpu(*val);
}

static void riscv_clock_initialize(void)
{
  const char *fdt;
  riscv_timecounter *tc;
  volatile RISCV_CLINT_regs *clint;
  uint32_t tb_freq;
  uint64_t us_per_tick;
  uint32_t interval;

  fdt = bsp_fdt_get();
  tb_freq = riscv_clock_get_timebase_frequency(fdt);
  us_per_tick = rtems_configuration_get_microseconds_per_tick();
  interval = (uint32_t) ((tb_freq * us_per_tick) / 1000000);
  clint = riscv_clint;
  tc = &riscv_clock_tc;

  tc->clint = clint;
  tc->interval = interval;

  riscv_clock_write_mtimecmp(
    &clint->mtimecmp[0],
    riscv_clock_read_mtime(&clint->mtime) + interval
  );

  /* Enable mtimer interrupts */
  set_csr(mie, MIP_MTIP);

  /* Initialize timecounter */
  tc->base.tc_get_timecount = riscv_clock_get_timecount;
  tc->base.tc_counter_mask = 0xffffffff;
  tc->base.tc_frequency = tb_freq;
  tc->base.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(&tc->base);
}

volatile uint32_t _RISCV_Counter_register;

static void riscv_counter_initialize(void)
{
  _RISCV_Counter_mutable = &riscv_clint->mtime.val_32[0];
}

uint32_t _CPU_Counter_frequency( void )
{
  return riscv_clock_get_timebase_frequency(bsp_fdt_get());
}

CPU_Counter_ticks _CPU_Counter_read( void )
{
  return *_RISCV_Counter;
}

RTEMS_SYSINIT_ITEM(
  riscv_counter_initialize,
  RTEMS_SYSINIT_CPU_COUNTER,
  RTEMS_SYSINIT_ORDER_FIRST
);

#define Clock_driver_support_at_tick() riscv_clock_at_tick(&riscv_clock_tc)

#define Clock_driver_support_initialize_hardware() riscv_clock_initialize()

#define Clock_driver_support_install_isr(isr) \
  riscv_clock_handler_install()

#define CLOCK_DRIVER_USE_ONLY_BOOT_PROCESSOR

#include "../../../shared/dev/clock/clockimpl.h"
