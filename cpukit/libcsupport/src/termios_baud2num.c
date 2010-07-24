/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/termios.h>
#include <rtems/termiostypes.h>
#include <rtems/assoc.h>

extern rtems_assoc_t termios_assoc_table[];

int32_t rtems_termios_baud_to_number(
  int termios_baud
)
{
  int baud;

  baud = rtems_assoc_local_by_remote( termios_assoc_table, termios_baud );
  if ( baud == 0 && termios_baud != 0 )
    return -1;

  return baud;
}
