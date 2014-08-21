/**
 * @file
 *
 * @ingroup tms570_clocks
 *
 * @brief System clocks.
 */

/*
 * Copyright (c) 2014 Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * Based on LPC24xx and LPC1768 BSP
 * by embedded brains GmbH and others
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <stdlib.h>

#include <rtems.h>
#include <bsp.h>


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
  asm volatile ("mrc p15, 0, %0, c9, c13, 0\n": "=r" (ticks));
  return ticks;
}
