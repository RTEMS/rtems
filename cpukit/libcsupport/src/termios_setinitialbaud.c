/**
 *  @file
 *
 *  @brief Sets the Initial Baud in the Termios Context
 *  @ingroup TermiostypesSupport
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

#include <rtems/termiostypes.h>

int rtems_termios_set_initial_baud(
  struct rtems_termios_tty *tty,
  rtems_termios_baud_t baud
)
{
  int rv = 0;
  speed_t spd = rtems_termios_number_to_baud(baud);

  if ( spd != 0 ) {
    tty->termios.c_ispeed = spd;
    tty->termios.c_ospeed = spd;
  } else {
    rv = -1;
  }

  return rv;
}
