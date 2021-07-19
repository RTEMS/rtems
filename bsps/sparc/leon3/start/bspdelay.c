/**
 *  @file
 *
 *  LEON3 BSP Delay Method
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/leon3.h>

void rtems_bsp_delay(int usecs)
{
  uint32_t then;
  gptimer_timer *regs;

  regs = &LEON3_Timer_Regs->timer[0];
  then  =grlib_load_32(&regs->tcntval);
  then += usecs;

  while (grlib_load_32(&regs->tcntval) >= then)
    ;
}
