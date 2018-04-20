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

void __attribute__((weak)) bsp_start(void)
{
}

uint32_t  __attribute__((weak)) bsp_get_CPU_clock_speed(void)
{
  #define DEF_CLOCK_SPEED 8000000.0F //8.0 MHz
  #define MCF_MFD0_2_MASK 0x7000U
  #define MCF_RFD0_2_MASK 0x0700U
  #define MCF_MFD0_2_OFFSET 4U

  #define SPEED_BIAS  ((((MCF_CLOCK_SYNCR & MCF_MFD0_2_MASK) >> 11) + MCF_MFD0_2_OFFSET) / (float)(((MCF_CLOCK_SYNCR & MCF_RFD0_2_MASK)>>7) ? : 1.0F))

  return MCF_CLOCK_SYNCR & MCF_CLOCK_SYNCR_PLLEN ? SPEED_BIAS * DEF_CLOCK_SPEED : DEF_CLOCK_SPEED;
}
