/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
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

#include <rtems/bspIo.h>
#include <rtems/libio.h>

#include "consolesimple.h"

ssize_t _Console_simple_Read(
  rtems_libio_t *iop,
  void          *buffer,
  size_t         count
)
{
  char    *buf;
  ssize_t  i;
  ssize_t  n;

  buf = buffer;
  n = (ssize_t) count;

  for ( i = 0; i < n; ++i ) {
    int c;

    while ( true ) {
      c = getchark();
      if ( c != -1 ) {
        break;
      }

      (void) rtems_task_wake_after( 1 );
    }

    buf[ i ] = (char) c;
  }

  return n;
}
