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

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/termiostypes.h>

void rtems_termios_set_best_baud(
  struct termios *term,
  uint32_t        baud
)
{
  const rtems_assoc_t *current = &rtems_termios_baud_table[ 0 ];
  const rtems_assoc_t *last = current;
  speed_t spd;

  while ( current->name != NULL && current->local_value < baud ) {
    last = current;
    ++current;
  }

  if ( current->name != NULL ) {
    uint32_t mid = (last->local_value + current->local_value) / UINT32_C( 2 );

    spd = baud <= mid ? last->remote_value : current->remote_value;
  } else {
    spd = B460800;
  }

  term->c_ispeed = spd;
  term->c_ospeed = spd;
}
