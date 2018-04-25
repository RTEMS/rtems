/**
 * @file
 *
 * @ingroup tms570_clocks
 *
 * @brief Cortex-R performace counters
 *
 * The counters setup functions are these which has been suggested
 * on StackOverflow
 *
 * Code is probably for use on Cortex-A without modifications as well.
 *
 * http://stackoverflow.com/questions/3247373/how-to-measure-program-execution-time-in-arm-cortex-a8-processor
 */

/*
 * Copyright (c) 2014 Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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

  rtems_counter_initialize_converter(2 * BSP_PLL_OUT_CLOCK);
}

CPU_Counter_ticks _CPU_Counter_read(void)
{
  return arm_cp15_get_performance_monitors_cycle_count();
}

RTEMS_SYSINIT_ITEM(
  tms570_cpu_counter_initialize,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_FIRST
);
