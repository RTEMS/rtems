/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>

extern char _CPUClockSpeed[];

uint32_t get_CPU_clock_speed(void)
{
  return( (uint32_t)_CPUClockSpeed);
}
