/*  timer.c
 *
 *  This file manages the interval timer on the PowerPC 403*.
 *  We shall use the bottom 32 bits of the timebase register,
 *
 *  NOTE: It is important that the timer start/stop overhead be
 *        determined when porting or modifying this code.
 *
 *  Author:	Andrew Bray <andy@i-cubed.co.uk>
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
 *  Derived from c/src/lib/libcpu/hppa1.1/timer/timer.c:
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Modifications for PPC405GP by Dennis Ehlin
 *
 *  $Id$
 *
 */

#include <rtems.h>

static volatile rtems_unsigned32 Timer_starting;
static rtems_boolean Timer_driver_Find_average_overhead;

/*
 *  This is so small that this code will be reproduced where needed.
 */
static inline rtems_unsigned32 get_itimer(void)
{
   rtems_unsigned32 ret;

#ifndef ppc405
   asm volatile ("mfspr %0, 0x3dd" : "=r" ((ret))); /* TBLO */
#else /* ppc405 */
/*   asm volatile ("mfspr %0, 0x3dd" : "=r" ((ret)));  TBLO */

   asm volatile ("mfspr %0, 0x10c" : "=r" ((ret))); /* 405GP TBL */
#endif /* ppc405 */

   return ret;
}

void Timer_initialize()
{
  rtems_unsigned32 iocr;

#ifndef ppc405
  asm volatile ("mfdcr %0, 0xa0" : "=r" (iocr)); /* IOCR */
  iocr &= ~4;
  iocr |= 4;  /* Select external timer clock */
  asm volatile ("mtdcr 0xa0, %0" : "=r" (iocr) : "0" (iocr)); /* IOCR */
#else /* ppc405 */
  asm volatile ("mfdcr %0, 0x0b2" : "=r" (iocr));  /*405GP CPC0_CR1 */
/*  asm volatile ("mfdcr %0, 0xa0" : "=r" (iocr)); IOCR */

  /* iocr |= 0x800000;  select external timer clock CETE*/
  iocr &= ~0x800000; /* timer clocked from system clock CETE*/

  asm volatile ("mtdcr 0x0b2, %0" : "=r" (iocr) : "0" (iocr)); /* 405GP CPC0_CR1 */
/*  asm volatile ("mtdcr 0xa0, %0" : "=r" (iocr) : "0" (iocr));  IOCR */
#endif /* ppc405 */

  Timer_starting = get_itimer();
}

int Read_timer()
{
  rtems_unsigned32 clicks;
  rtems_unsigned32 total;

  clicks = get_itimer();

  total = clicks - Timer_starting;

  if ( Timer_driver_Find_average_overhead == 1 )
    return total;          /* in XXX microsecond units */

  else {
    if ( total < rtems_cpu_configuration_get_timer_least_valid() )
      return 0;            /* below timer resolution */
    return (total - rtems_cpu_configuration_get_timer_average_overhead());
  }
}

rtems_status_code Empty_function( void )
{
  return RTEMS_SUCCESSFUL;
}

void Set_find_average_overhead(
  rtems_boolean find_flag
)
{
  Timer_driver_Find_average_overhead = find_flag;
}
