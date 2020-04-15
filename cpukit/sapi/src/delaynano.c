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

void rtems_counter_delay_nanoseconds( uint32_t nanoseconds )
{
  rtems_counter_ticks ticks =
    rtems_counter_nanoseconds_to_ticks( nanoseconds );

  rtems_counter_delay_ticks( ticks );
}
