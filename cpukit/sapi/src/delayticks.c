/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/counter.h>

void rtems_counter_delay_ticks( rtems_counter_ticks ticks )
{
  rtems_counter_ticks a = rtems_counter_read();
  rtems_counter_ticks delta = 0;

  do {
    rtems_counter_ticks b;

    ticks -= delta;

    b = rtems_counter_read();
    delta = rtems_counter_difference( b, a );
    a = b;
  } while ( ticks > delta );
}
