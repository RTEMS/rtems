/*
 *  This file contains the routine console_reserve_resources
 *  for the Score603e console driver.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>

#include "consolebsp.h"

void console_reserve_resources(
  rtems_configuration_table *configuration
)
{
  rtems_termios_reserve_resources( configuration, NUM_Z85C30_PORTS );
}

