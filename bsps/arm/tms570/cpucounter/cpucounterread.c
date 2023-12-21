/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief This source file contains the CPU Counter implementation.
 *
 * The counters setup functions are these which has been suggested on
 * StackOverflow.  Code is probably for use on Cortex-A without modifications
 * as well.
 *
 * http://stackoverflow.com/questions/3247373/how-to-measure-program-execution-time-in-arm-cortex-a8-processor
 */

/*
 * Copyright (C) 2014 Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
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

#include <rtems/counter.h>
#include <rtems/sysinit.h>

#include <libcpu/arm-cp15.h>

#include <bsp.h>

static void tms570_cpu_counter_initialize(void)
{
  uint32_t cycle_counter;
  uint32_t pmcr;

  cycle_counter = ARM_CP15_PMCLRSET_CYCLE_COUNTER;
  arm_cp15_set_performance_monitors_interrupt_enable_clear(cycle_counter);
  arm_cp15_set_performance_monitors_count_enable_set(cycle_counter);

  pmcr = arm_cp15_get_performance_monitors_control();
  pmcr &= ~ARM_CP15_PMCR_D;
  pmcr |= ARM_CP15_PMCR_E;
  arm_cp15_set_performance_monitors_control(pmcr);
}

uint32_t _CPU_Counter_frequency(void)
{
  return 2 * BSP_PLL_OUT_CLOCK;
}

CPU_Counter_ticks _CPU_Counter_read(void)
{
  return arm_cp15_get_performance_monitors_cycle_count();
}

RTEMS_SYSINIT_ITEM(
  tms570_cpu_counter_initialize,
  RTEMS_SYSINIT_CPU_COUNTER,
  RTEMS_SYSINIT_ORDER_FIRST
);
