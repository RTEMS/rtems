/*
 * Copyright (c) 2014, 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/sparcimpl.h>
#include <rtems/config.h>

CPU_Counter_ticks _SPARC_Counter_difference_normal(
  CPU_Counter_ticks second,
  CPU_Counter_ticks first
)
{
  return second - first;
}

CPU_Counter_ticks _SPARC_Counter_difference_clock_period(
  CPU_Counter_ticks second,
  CPU_Counter_ticks first
)
{
  CPU_Counter_ticks period;

  period = rtems_configuration_get_microseconds_per_tick();

  return ( first + period - second ) % period;
}

CPU_Counter_ticks _SPARC_Counter_difference_one(
  CPU_Counter_ticks second,
  CPU_Counter_ticks first
)
{
  return 1;
}
