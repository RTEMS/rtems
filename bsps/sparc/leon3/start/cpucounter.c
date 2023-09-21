/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsSPARCLEON3
 *
 * @brief This source file contains the implementation of the CPU Counter.
 */

/*
 * Copyright (C) 2014, 2023 embedded brains GmbH & Co. KG
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

#include <bsp/leon3.h>
#include <grlib/irqamp.h>

#include <rtems/counter.h>
#include <rtems/sysinit.h>
#include <rtems/timecounter.h>

#if defined(LEON3_HAS_ASR_22_23_UP_COUNTER) || \
  defined(LEON3_PROBE_ASR_22_23_UP_COUNTER)
static uint32_t leon3_timecounter_get_asr_22_23_up_counter(
  struct timecounter *tc
)
{
  return leon3_up_counter_low();
}

static void leon3_counter_use_asr_22_23_up_counter(leon3_timecounter *tc)
{
#if !defined(LEON3_HAS_ASR_22_23_UP_COUNTER)
  tc->base.tc_get_timecount = leon3_timecounter_get_asr_22_23_up_counter;
#endif
  tc->base.tc_frequency = leon3_up_counter_frequency();
}
#endif

/*
 * The following code blocks provide different CPU counter implementations.
 * The implementation used is defined by build configuration options.  For a
 * particular chip, the best available hardware counter module may be selected
 * by build configuration options.  The default implementation tries to select
 * the best module at runtime.
 */

#if defined(LEON3_HAS_ASR_22_23_UP_COUNTER)

CPU_Counter_ticks _CPU_Counter_read(void)
{
  return leon3_up_counter_low();
}

RTEMS_ALIAS(_CPU_Counter_read) uint32_t _SPARC_Counter_read_ISR_disabled(void);

#define LEON3_GET_TIMECOUNT_INIT leon3_timecounter_get_asr_22_23_up_counter

#else /* !LEON3_HAS_ASR_22_23_UP_COUNTER */

/*
 * This is a workaround for:
 * https://gcc.gnu.org/bugzilla//show_bug.cgi?id=69027
 */
__asm__ (
  "\t.section\t\".text\"\n"
  "\t.align\t4\n"
  "\t.globl\t_CPU_Counter_read\n"
  "\t.globl\t_SPARC_Counter_read_ISR_disabled\n"
  "\t.type\t_CPU_Counter_read, #function\n"
  "\t.type\t_SPARC_Counter_read_ISR_disabled, #function\n"
  "_CPU_Counter_read:\n"
  "_SPARC_Counter_read_ISR_disabled:\n"
  "\tsethi\t%hi(leon3_timecounter_instance), %o0\n"
  "\tld	[%o0 + %lo(leon3_timecounter_instance)], %o1\n"
  "\tor\t%o0, %lo(leon3_timecounter_instance), %o0\n"
  "\tor\t%o7, %g0, %g1\n"
  "\tcall\t%o1, 0\n"
  "\t or\t%g1, %g0, %o7\n"
  "\t.previous\n"
);

static uint32_t leon3_timecounter_get_dummy(struct timecounter *base)
{
  leon3_timecounter *tc;
  uint32_t counter;

  tc = (leon3_timecounter *) base;
  counter = tc->software_counter + 1;
  tc->software_counter = counter;
  return counter;
}

#define LEON3_GET_TIMECOUNT_INIT leon3_timecounter_get_dummy

static uint32_t leon3_timecounter_get_counter_down(struct timecounter *base)
{
  leon3_timecounter *tc;

  tc = (leon3_timecounter *) base;
  return -(*tc->counter_register);
}

static void leon3_counter_use_gptimer(
  leon3_timecounter *tc,
  gptimer *gpt
)
{
  gptimer_timer *timer;

  timer = &gpt->timer[LEON3_COUNTER_GPTIMER_INDEX];

  /* Make timer free-running */
  grlib_store_32(&timer->trldval, 0xffffffff);
  grlib_store_32(&timer->tctrl, GPTIMER_TCTRL_EN | GPTIMER_TCTRL_RS);

  tc->counter_register = &timer->tcntval;
  tc->base.tc_get_timecount = leon3_timecounter_get_counter_down;
#if defined(LEON3_PLB_FREQUENCY_DEFINED_BY_GPTIMER)
  tc->base.tc_frequency = LEON3_GPTIMER_0_FREQUENCY_SET_BY_BOOT_LOADER;
#else
  tc->base.tc_frequency = ambapp_freq_get(ambapp_plb(), LEON3_Timer_Adev) /
    (grlib_load_32(&gpt->sreload) + 1);
#endif
}

#if defined(LEON3_IRQAMP_PROBE_TIMESTAMP)

static uint32_t leon3_timecounter_get_counter_up(struct timecounter *base)
{
  leon3_timecounter *tc;

  tc = (leon3_timecounter *) base;
  return *tc->counter_register;
}

static void leon3_counter_use_irqamp_timestamp(
  leon3_timecounter *tc,
  irqamp_timestamp *irqmp_ts
)
{
  /* Enable interrupt timestamping for an arbitrary interrupt line */
  grlib_store_32(&irqmp_ts->itstmpc, IRQAMP_ITSTMPC_TSISEL(1));

  tc->counter_register = &irqmp_ts->itcnt;
  tc->base.tc_get_timecount = leon3_timecounter_get_counter_up;
#if defined(LEON3_PLB_FREQUENCY_DEFINED_BY_GPTIMER)
  tc->base.tc_frequency = leon3_processor_local_bus_frequency();
#else
  tc->base.tc_frequency = ambapp_freq_get(ambapp_plb(), LEON3_IrqCtrl_Adev);
#endif
}

#endif /* LEON3_IRQAMP_PROBE_TIMESTAMP */
#endif /* LEON3_HAS_ASR_22_23_UP_COUNTER */

leon3_timecounter leon3_timecounter_instance = {
  .base = {
    .tc_get_timecount = LEON3_GET_TIMECOUNT_INIT,
    .tc_counter_mask = 0xffffffff,
    .tc_frequency = 1000000000,
    .tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER
  }
};

uint32_t _CPU_Counter_frequency(void)
{
  return leon3_timecounter_instance.base.tc_frequency;
}

static void leon3_counter_initialize(void)
{
#if defined(LEON3_HAS_ASR_22_23_UP_COUNTER)

  leon3_up_counter_enable();
  leon3_counter_use_asr_22_23_up_counter(&leon3_timecounter_instance);

#else /* !LEON3_HAS_ASR_22_23_UP_COUNTER */

  /* Try to find the best CPU counter available */

#if defined(LEON3_IRQAMP_PROBE_TIMESTAMP)
  irqamp_timestamp *irqmp_ts;
#endif
  gptimer *gpt;
  leon3_timecounter *tc;

  tc = &leon3_timecounter_instance;

#if defined(LEON3_PROBE_ASR_22_23_UP_COUNTER)
  leon3_up_counter_enable();

  if (leon3_up_counter_is_available()) {
    /* Use the LEON4 up-counter if available */
    leon3_counter_use_asr_22_23_up_counter(tc);
    return;
  }
#endif

#if defined(LEON3_IRQAMP_PROBE_TIMESTAMP)
  irqmp_ts = irqamp_get_timestamp_registers(LEON3_IrqCtrl_Regs);

  if (irqmp_ts != NULL) {
    /* Use the interrupt controller timestamp counter if available */
    leon3_counter_use_irqamp_timestamp(tc, irqmp_ts);
    return;
  }
#endif

  gpt = LEON3_Timer_Regs;

#if defined(LEON3_GPTIMER_BASE)
  leon3_counter_use_gptimer(tc, gpt);
#else
  if (gpt != NULL) {
    /* Fall back to the first GPTIMER if available */
    leon3_counter_use_gptimer(tc, gpt);
  }
#endif

#endif /* LEON3_HAS_ASR_22_23_UP_COUNTER */
}

RTEMS_SYSINIT_ITEM(
  leon3_counter_initialize,
  RTEMS_SYSINIT_CPU_COUNTER,
  RTEMS_SYSINIT_ORDER_FIRST
);
