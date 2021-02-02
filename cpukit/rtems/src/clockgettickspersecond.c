/**
 * @file
 *
 * @ingroup RTEMSImplClassicClock
 *
 * @brief This source file contains the implementation of
 *   rtems_clock_get_ticks_per_second().
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/clock.h>

static rtems_interval _RTEMS_Clock_get_ticks_per_second( void )
{
  return rtems_clock_get_ticks_per_second();
}

#undef rtems_clock_get_ticks_per_second

rtems_interval rtems_clock_get_ticks_per_second( void )
{
  return _RTEMS_Clock_get_ticks_per_second();
}
