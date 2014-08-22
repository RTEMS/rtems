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

#include <stdlib.h>

#include <rtems.h>
#include <bsp.h>

static int cpu_counter_initialized;


/**
 * @brief set mode of Cortex-R performance counters
 *
 * Based on example found on http://stackoverflow.com
 *
 * @param[in] do_reset if set, values of the counters are reset
 * @param[in] enable_divider if set, CCNT counts clocks divided by 64
 * @retval Void
 */
static inline void _CPU_Counter_init_perfcounters(
    int32_t do_reset,
    int32_t enable_divider
)
{
  /* in general enable all counters (including cycle counter) */
  int32_t value = 1;

  /* peform reset */
  if (do_reset)
  {
    value |= 2;     /* reset all counters to zero */
    value |= 4;     /* reset cycle counter to zero */
  }

  if (enable_divider)
    value |= 8;     /* enable "by 64" divider for CCNT */

  value |= 16;

  /* program the performance-counter control-register */
  asm volatile ("mcr p15, 0, %0, c9, c12, 0\t\n" :: "r"(value));

  /* enable all counters */
  asm volatile ("mcr p15, 0, %0, c9, c12, 1\t\n" :: "r"(0x8000000f));

  /* clear overflows */
  asm volatile ("mcr p15, 0, %0, c9, c12, 3\t\n" :: "r"(0x8000000f));
}

/**
 * @brief initialize Cortex-R performance counters subsystem
 *
 * Based on example found on http://stackoverflow.com
 *
 * @retval Void
 *
 */
static void _CPU_Counter_initialize(void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);

  if ( cpu_counter_initialized ) {
    rtems_interrupt_enable(level);
    return;
  }

  /* enable user-mode access to the performance counter */
  asm volatile ("mcr p15, 0, %0, c9, c14, 0\n\t" :: "r"(1));

  /* disable counter overflow interrupts (just in case) */
  asm volatile ("mcr p15, 0, %0, c9, c14, 2\n\t" :: "r"(0x8000000f));

  _CPU_Counter_init_perfcounters(false, false);

  cpu_counter_initialized = 1;

  rtems_interrupt_enable(level);
}

/**
 * @brief returns the actual value of Cortex-R cycle counter register
 *
 * The register is incremented at each core clock period
 *
 * @retval x actual core clock counter value
 *
 */
CPU_Counter_ticks _CPU_Counter_read(void)
{
  uint32_t ticks;
  if ( !cpu_counter_initialized ) {
    _CPU_Counter_initialize();
  }
  asm volatile ("mrc p15, 0, %0, c9, c13, 0\n": "=r" (ticks));
  return ticks;
}
