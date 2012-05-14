/*
 *  ERC32 BSP Delay Method
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>

void rtems_bsp_delay(int usecs)
{
  uint32_t then;

  then  = ERC32_MEC.Real_Time_Clock_Counter;
  then += usecs;

  while (ERC32_MEC.Real_Time_Clock_Counter >= then)
    ;
}
