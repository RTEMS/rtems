/*
 *  This routine returns control from RTEMS to the monitor.
 */

/*
 * Copyright (c) 2005 Eric Norum <eric@norum.ca>
 *
 * COPYRIGHT (c) 2005.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>

void bsp_fatal_extension(
  rtems_fatal_source source,
  bool always_set_to_false,
  rtems_fatal_code error
)
{
  bsp_reset();
}
