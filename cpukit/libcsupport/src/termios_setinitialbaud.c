/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
  tcflag_t c_cflag_baud = rtems_termios_number_to_baud(baud);

  if ( c_cflag_baud != 0 ) {
    tcflag_t cbaud = CBAUD;

    tty->termios.c_cflag = (tty->termios.c_cflag & ~cbaud) | c_cflag_baud;
  } else {
    rv = -1;
  }

  return rv;
}
