/*
 *  This routine does the bulk of the system initialisation.
 */

/*
 *  Author:
 *    David Fiddes, D.J@fiddes.surfaid.org
 *    http://www.calm.hw.ac.uk/davidf/coldfire/
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <rtems/rtems/cache.h>

void bsp_start(void)
{
  /* cfinit invalidates cache and sets acr registers */

  /*
   * Enable the cache, we only need to enable the instruction cache as the
   * 532x has a unified data and instruction cache.
   */
  rtems_cache_enable_instruction();
}

uint32_t bsp_get_CPU_clock_speed(void)
{
  return 240000000;
}

uint32_t bsp_get_BUS_clock_speed(void)
{
  return 80000000;
}
