/*  timer.c
 *
 *  This file manages the interval timer on the PowerPC MPC860.
 *  NOTE: This is not the PIT, but rather the RTEMS interval
 *        timer
 *  We shall use the bottom 32 bits of the timebase register,
 *
 *  The following was in the 403 version of this file. I don't
 *  know what it means. JTM 5/19/98
 *  NOTE: It is important that the timer start/stop overhead be
 *        determined when porting or modifying this code.
 *
 *  Author: Andy Dachs <a.dachs@sstl.co.uk>
 *  Surrey Satellite Technlogy Limited
 *   Modified for MPC8260
 *   Derived from c/src/lib/libcpu/powerpc/mpc860/timer/timer.c:
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
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems.h>
#include <mpc8260.h>

static volatile rtems_unsigned32 Timer_starting;
static rtems_boolean Timer_driver_Find_average_overhead;

extern rtems_cpu_table Cpu_table;
#define rtems_cpu_configuration_get_timer_average_overhead() \
   (Cpu_table.timer_average_overhead)

#define rtems_cpu_configuration_get_timer_least_valid() \
   (Cpu_table.timer_least_valid)

/*
 *  This is so small that this code will be reproduced where needed.
 */
static inline rtems_unsigned32 get_itimer(void)
{
   rtems_unsigned32 ret;

   asm volatile ("mftb %0" : "=r" ((ret))); /* TBLO */

   return ret;
}

void Timer_initialize(void)
{
  /* set interrupt level and enable timebase. This should never */
  /*  generate an interrupt however. */

/*
  m860.tbscr |= M860_TBSCR_TBIRQ(4) | M860_TBSCR_TBE;
*/
				

  Timer_starting = get_itimer();
}

int Read_timer(void)
{
  rtems_unsigned32 clicks;
  rtems_unsigned32 total;

  clicks = get_itimer();

  total = clicks - Timer_starting;

  if ( Timer_driver_Find_average_overhead == 1 )
    return total;          /* in XXX microsecond units */

  else {
    if ( total < rtems_cpu_configuration_get_timer_least_valid() ) {
      return 0;            /* below timer resolution */
    }
    return (total - rtems_cpu_configuration_get_timer_average_overhead());
  }
}

rtems_status_code Empty_function(void)
{
  return RTEMS_SUCCESSFUL;
}

void Set_find_average_overhead(rtems_boolean find_flag)
{
  Timer_driver_Find_average_overhead = find_flag;
}
