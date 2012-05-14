/*  timer.c
 *
 *  This file manages the interval timer on the PowerPC MPC8xx.
 *  NOTE: This is not the PIT, but rather the RTEMS interval
 *        timer
 *  We shall use the bottom 32 bits of the timebase register,
 *
 *  The following was in the 403 version of this file. I don't
 *  know what it means. JTM 5/19/98
 *  NOTE: It is important that the timer start/stop overhead be
 *        determined when porting or modifying this code.
 *
 *  Author: Jay Monkman (jmonkman@frasca.com)
 *  Copywright (C) 1998 by Frasca International, Inc.
 *
 *  Derived from c/src/lib/libcpu/ppc/ppc403/timer/timer.c:
 *
 *  Author:     Andrew Bray <andy@i-cubed.co.uk>
 *
 *  COPYRIGHT (c) 1995 by i-cubed ltd.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of i-cubed limited not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      i-cubed limited makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Derived from c/src/lib/libcpu/hppa1_1/timer/timer.c:
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <mpc8xx.h>

static volatile uint32_t   Timer_starting;
static bool benchmark_timer_find_average_overhead;
extern uint32_t bsp_timer_least_valid;
extern uint32_t bsp_timer_average_overhead;

/*
 *  This is so small that this code will be reproduced where needed.
 */
static inline uint32_t   get_itimer(void)
{
   uint32_t   ret;

   __asm__ volatile ("mftb %0" : "=r" ((ret))); /* TBLO */

   return ret;
}

void benchmark_timer_initialize(void)
{
  /* set interrupt level and enable timebase. This should never */
  /*  generate an interrupt however. */
  m8xx.tbscr |= M8xx_TBSCR_TBIRQ(4) | M8xx_TBSCR_TBE;

  Timer_starting = get_itimer();
}

int benchmark_timer_read(void)
{
  uint32_t   clicks;
  uint32_t   total;

  clicks = get_itimer();

  total = clicks - Timer_starting;

  if ( benchmark_timer_find_average_overhead == 1 )
    return total;          /* in XXX microsecond units */
  else {
    if ( total < bsp_timer_least_valid ) {
      return 0;            /* below timer resolution */
    }
    return (total - bsp_timer_average_overhead);
  }
}

void benchmark_timer_disable_subtracting_average_overhead(bool find_flag)
{
  benchmark_timer_find_average_overhead = find_flag;
}
